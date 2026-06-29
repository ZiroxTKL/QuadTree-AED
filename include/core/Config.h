#pragma once
#include <string>
#include <cstdint>

namespace qt {

// Tipos de distribución inicial de las partículas.
enum class Distribution {
    Uniform,   // distribución uniforme en todo el espacio
    Clusters,  // varios grupos (clusters) gaussianos
    Dense      // una zona pequeña de alta densidad + resto disperso
};

Distribution distributionFromString(const std::string& s);
std::string  distributionToString(Distribution d);

// Configuración completa de una corrida. Se llena con valores por
// defecto, luego se sobreescribe desde un JSON y/o desde la línea de comandos.
struct Config {
    // --- Parámetros generales ---
    int    n          = 1000;     // número de objetos
    double width      = 1000.0;   // ancho del espacio 2D
    double height     = 1000.0;   // alto del espacio 2D
    int    capacity   = 4;        // capacidad máxima por nodo del QuadTree
    int    maxDepth   = 16;       // profundidad máxima (evita subdivisión infinita)

    // --- Radios y velocidades ---
    double radiusMin  = 3.0;
    double radiusMax  = 6.0;
    double speedMin   = 20.0;     // unidades por segundo
    double speedMax   = 80.0;

    // --- Distribución inicial ---
    Distribution distribution = Distribution::Uniform;
    int    numClusters    = 5;    // para Clusters
    double clusterStd     = 40.0; // desviación de cada cluster
    double denseFraction  = 0.6;  // fracción de partículas en la zona densa (Dense)
    double denseSize      = 0.15; // tamaño de la zona densa relativo al espacio

    // --- Simulación ---
    double dt          = 1.0 / 60.0;  // paso de tiempo por frame (segundos)
    int    frames      = 300;         // frames a simular en modo benchmark
    double queryRadius = 30.0;        // radio de vecindad por defecto en queries

    uint64_t seed = 42;               // semilla de aleatoriedad

    // Carga desde un archivo JSON. Devuelve true si tuvo éxito.
    bool loadFromFile(const std::string& path);

    // Imprime un resumen legible a stderr (vía Logger).
    void printSummary() const;
};

} // namespace qt
