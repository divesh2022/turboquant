#include "turboquant/transforms/polar.hpp"
#include <cmath>
#include <stdexcept>

namespace turboquant::transforms {

PolarTransform::PolarTransform(size_t dimensions) : dimensions_(dimensions) {
    if (dimensions == 0) {
        throw std::invalid_argument("[PolarTransform Error] Dimensions must be greater than zero.");
    }
}



std::vector<float> PolarTransform::transform(const std::vector<float>& input) const {
    if (input.size() != dimensions_) {
        throw std::length_error("[PolarTransform Error] Input size mismatch.");
    }

    std::vector<float> output;
    output.reserve(dimensions_ * 2);

    // Each element contributes magnitude and phase
    for (size_t i = 0; i < dimensions_; i += 2) {
        float x = input[i];
        float y = (i + 1 < dimensions_) ? input[i + 1] : 0.0f;

        float magnitude = std::sqrt(x * x + y * y);
        float phase = std::atan2(y, x);

        // Store magnitude and phase for both elements of the pair
        output.push_back(magnitude);
        output.push_back(phase);

        // Duplicate for the second element if present
        if (i + 1 < dimensions_) {
            output.push_back(magnitude);
            output.push_back(phase);
        }
    }

    return output;
}

size_t PolarTransform::getDimensions() const {
    return dimensions_;
}

} // namespace turboquant::transforms
