#pragma once
#include <iostream>
#include <string>
#include <cmath>

// Micro-framework de pruebas sin dependencias externas.
// Cada archivo de test define casos con TEST_CASE y usa CHECK / CHECK_NEAR.
// Al final llama a qt_test::summary() que devuelve el código de salida.
namespace qt_test {

    inline int& passed() { static int p = 0; return p; }
    inline int& failed() { static int f = 0; return f; }

    inline void check(bool cond, const std::string& expr, const char* file, int line) {
        if (cond) {
            ++passed();
        } else {
            ++failed();
            std::cerr << "  [FAIL] " << file << ":" << line << "  " << expr << "\n";
        }
    }

    inline void checkNear(double a, double b, double tol,
                          const std::string& expr, const char* file, int line) {
        check(std::fabs(a - b) <= tol, expr, file, line);
    }

    inline int summary(const std::string& name) {
        std::cout << name << ": " << passed() << " ok, " << failed() << " fallidas\n";
        return failed() == 0 ? 0 : 1;
    }

} // namespace qt_test

#define CHECK(cond)            qt_test::check((cond), #cond, __FILE__, __LINE__)
#define CHECK_NEAR(a, b, tol)  qt_test::checkNear((a), (b), (tol), #a " ~= " #b, __FILE__, __LINE__)
#define SECTION(name)          std::cout << "-- " << name << "\n"
