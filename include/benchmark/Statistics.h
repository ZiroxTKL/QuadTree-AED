#pragma once
#include <vector>
#include <cstdint>
#include <cmath>
#include <limits>
#include <cstddef>

namespace qt {

    // Acumula una serie de muestras (una por frame) y calcula estadísticas:
    // promedio, mínimo, máximo y desviación estándar.
    class Statistics {
    public:
        void add(double value) {
            samples_.push_back(value);
            sum_ += value;
            if (value < min_) min_ = value;
            if (value > max_) max_ = value;
        }

        double mean() const {
            return samples_.empty() ? 0.0 : sum_ / static_cast<double>(samples_.size());
        }

        double min() const { return samples_.empty() ? 0.0 : min_; }
        double max() const { return samples_.empty() ? 0.0 : max_; }

        double stddev() const {
            if (samples_.size() < 2) return 0.0;
            double m = mean();
            double acc = 0.0;
            for (double v : samples_) {
                double d = v - m;
                acc += d * d;
            }
            return std::sqrt(acc / static_cast<double>(samples_.size() - 1));
        }

        std::size_t count() const { return samples_.size(); }

    private:
        std::vector<double> samples_;
        double sum_ = 0.0;
        double min_ = std::numeric_limits<double>::max();
        double max_ = std::numeric_limits<double>::lowest();
    };

} // namespace qt
