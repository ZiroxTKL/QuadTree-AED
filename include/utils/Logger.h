#pragma once
#include <string>
#include <iostream>
#include <mutex>

// Logger minimalista con niveles. Escribe a stderr para no mezclarse
// con la salida de datos (CSV) que va a stdout o a archivo.
namespace qt {

    enum class LogLevel { Debug = 0, Info = 1, Warn = 2, Error = 3 };

    class Logger {
    public:
        static Logger& instance() {
            static Logger inst;
            return inst;
        }

        void setLevel(LogLevel lvl) { level_ = lvl; }

        void log(LogLevel lvl, const std::string& msg) {
            if (lvl < level_) return;
            std::lock_guard<std::mutex> lock(mutex_);
            std::cerr << "[" << tag(lvl) << "] " << msg << std::endl;
        }

        void debug(const std::string& m) { log(LogLevel::Debug, m); }
        void info(const std::string& m)  { log(LogLevel::Info, m); }
        void warn(const std::string& m)  { log(LogLevel::Warn, m); }
        void error(const std::string& m) { log(LogLevel::Error, m); }

    private:
        Logger() = default;

        static const char* tag(LogLevel lvl) {
            switch (lvl) {
                case LogLevel::Debug: return "DEBUG";
                case LogLevel::Info:  return "INFO ";
                case LogLevel::Warn:  return "WARN ";
                case LogLevel::Error: return "ERROR";
            }
            return "?????";
        }

        LogLevel level_ = LogLevel::Info;
        std::mutex mutex_;
    };

    // Atajos
    inline void logInfo(const std::string& m)  { Logger::instance().info(m); }
    inline void logWarn(const std::string& m)  { Logger::instance().warn(m); }
    inline void logError(const std::string& m) { Logger::instance().error(m); }
    inline void logDebug(const std::string& m) { Logger::instance().debug(m); }

} // namespace qt
