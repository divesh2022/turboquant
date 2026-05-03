#pragma once
#include <vector>

namespace turboquant {
    class Quantizer {
    public:
        Quantizer() = default;
        std::vector<int> quantize(const std::vector<double>& input);
    };
}
