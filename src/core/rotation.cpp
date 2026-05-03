#include "turboquant/core/rotation.hpp"
#include <cmath>
#include <stdexcept>

namespace turboquant::core {

Rotation::Rotation(size_t dimensions) : dimensions_(dimensions) {
    if (dimensions == 0) {
        throw std::invalid_argument("[Rotation Error] Dimensions must be greater than zero.");
    }
}

std::vector<float> Rotation::transform(const std::vector<float>& input, float angle) const {
    if (input.size() != dimensions_) {
        throw std::length_error("[Rotation Error] Input size mismatch.");
    }

    std::vector<float> output(input.size());

    // Apply a simple Givens-like rotation to the first two coordinates
    if (dimensions_ >= 2) {
        output[0] = input[0] * std::cos(angle) - input[1] * std::sin(angle);
        output[1] = input[0] * std::sin(angle) + input[1] * std::cos(angle);

        // Leave the rest unchanged
        for (size_t i = 2; i < dimensions_; ++i) {
            output[i] = input[i];
        }
    } else {
        output = input; // trivial case
    }

    return output;
}

size_t Rotation::getDimensions() const {
    return dimensions_;
}

} // namespace turboquant::core
