#pragma once
#include <vector>

namespace turboquant {
    class Rotation {
    public:
        static std::vector<double> rotate2D(const std::vector<double>& v, double angle);
        static std::vector<double> rotate3D_Z(const std::vector<double>& v, double angle);
    };
}
