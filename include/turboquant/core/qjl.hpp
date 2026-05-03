#pragma once
#include <vector>

namespace turboquant {
    class QJL {
    public:
        QJL() = default;
        double compute(const std::vector<double>& input);
    };
}
