#include "core/Config.hpp"
#include "utils/Logger.hpp"

#include <fstream>
#include <sstream>
#include <cctype>
#include <map>
#include <stdexcept>

namespace qt {

Distribution distributionFromString(const std::string& s) {
    if (s == "clusters") return Distribution::Clusters;
    if (s == "dense")    return Distribution::Dense;
    return Distribution::Uniform;
}

std::string distributionToString(Distribution d) {
    switch (d) {
        case Distribution::Clusters: return "clusters";
        case Distribution::Dense:    return "dense";
        default:                     return "uniform";
    }
}

// ---------------------------------------------------------------------------
// Parser JSON minimalista (anónimo, solo visible en este archivo).
// Soporta el subconjunto necesario para los archivos de configuración:
// objeto plano con valores number / string / bool. No es un parser general,
// pero evita depender de librerías externas (no hay red para instalarlas).
// ---------------------------------------------------------------------------
namespace {

struct JsonParser {
    const std::string& s;
    size_t i = 0;
    explicit JsonParser(const std::string& src) : s(src) {}

    void skipWs() {
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    }

    bool parseObject(std::map<std::string, std::string>& out) {
        skipWs();
        if (i >= s.size() || s[i] != '{') return false;
        ++i; // consumir '{'
        skipWs();
        if (i < s.size() && s[i] == '}') { ++i; return true; }

        while (i < s.size()) {
            skipWs();
            std::string key;
            if (!parseString(key)) return false;
            skipWs();
            if (i >= s.size() || s[i] != ':') return false;
            ++i; // consumir ':'
            skipWs();
            std::string value;
            if (!parseValue(value)) return false;
            out[key] = value;
            skipWs();
            if (i < s.size() && s[i] == ',') { ++i; continue; }
            if (i < s.size() && s[i] == '}') { ++i; return true; }
            return false;
        }
        return false;
    }

    bool parseString(std::string& out) {
        skipWs();
        if (i >= s.size() || s[i] != '"') return false;
        ++i;
        std::ostringstream oss;
        while (i < s.size() && s[i] != '"') {
            if (s[i] == '\\' && i + 1 < s.size()) { ++i; oss << s[i]; }
            else oss << s[i];
            ++i;
        }
        if (i >= s.size()) return false;
        ++i; // consumir comilla de cierre
        out = oss.str();
        return true;
    }

    // Devuelve el valor como string crudo (number/bool/string).
    bool parseValue(std::string& out) {
        skipWs();
        if (i >= s.size()) return false;
        if (s[i] == '"') return parseString(out);

        // number, true, false, null
        std::ostringstream oss;
        while (i < s.size() && s[i] != ',' && s[i] != '}' &&
               !std::isspace(static_cast<unsigned char>(s[i]))) {
            oss << s[i];
            ++i;
        }
        out = oss.str();
        return !out.empty();
    }
};

double toDouble(const std::map<std::string, std::string>& m,
                const std::string& key, double def) {
    auto it = m.find(key);
    if (it == m.end()) return def;
    try { return std::stod(it->second); } catch (...) { return def; }
}

int toInt(const std::map<std::string, std::string>& m,
          const std::string& key, int def) {
    auto it = m.find(key);
    if (it == m.end()) return def;
    try { return static_cast<int>(std::stod(it->second)); } catch (...) { return def; }
}

std::string toStr(const std::map<std::string, std::string>& m,
                  const std::string& key, const std::string& def) {
    auto it = m.find(key);
    return it == m.end() ? def : it->second;
}

} // namespace anónimo

bool Config::loadFromFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        logError("No se pudo abrir el archivo de configuracion: " + path);
        return false;
    }
    std::stringstream buffer;
    buffer << f.rdbuf();
    std::string content = buffer.str();

    std::map<std::string, std::string> kv;
    JsonParser parser(content);
    if (!parser.parseObject(kv)) {
        logError("JSON invalido en: " + path);
        return false;
    }

    n             = toInt(kv,    "n",             n);
    width         = toDouble(kv, "width",         width);
    height        = toDouble(kv, "height",        height);
    capacity      = toInt(kv,    "capacity",      capacity);
    maxDepth      = toInt(kv,    "max_depth",     maxDepth);

    radiusMin     = toDouble(kv, "radius_min",    radiusMin);
    radiusMax     = toDouble(kv, "radius_max",    radiusMax);
    // Permitir "radius" único como alias de min=max
    if (kv.count("radius")) {
        radiusMin = radiusMax = toDouble(kv, "radius", radiusMin);
    }
    speedMin      = toDouble(kv, "speed_min",     speedMin);
    speedMax      = toDouble(kv, "speed_max",     speedMax);

    distribution  = distributionFromString(toStr(kv, "distribution",
                                                  distributionToString(distribution)));
    numClusters   = toInt(kv,    "num_clusters",  numClusters);
    clusterStd    = toDouble(kv, "cluster_std",   clusterStd);
    denseFraction = toDouble(kv, "dense_fraction", denseFraction);
    denseSize     = toDouble(kv, "dense_size",    denseSize);

    dt            = toDouble(kv, "dt",            dt);
    frames        = toInt(kv,    "frames",        frames);
    queryRadius   = toDouble(kv, "query_radius",  queryRadius);
    seed          = static_cast<uint64_t>(toInt(kv, "seed", static_cast<int>(seed)));

    return true;
}

void Config::printSummary() const {
    std::ostringstream oss;
    oss << "Config { n=" << n
        << ", espacio=" << width << "x" << height
        << ", capacidad=" << capacity
        << ", dist=" << distributionToString(distribution)
        << ", radio=[" << radiusMin << "," << radiusMax << "]"
        << ", vel=[" << speedMin << "," << speedMax << "]"
        << ", queryR=" << queryRadius
        << ", frames=" << frames
        << ", seed=" << seed << " }";
    logInfo(oss.str());
}

} // namespace qt
