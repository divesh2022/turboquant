#include <vector>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include "turboquant/core/rotation.hpp"

using namespace std;

namespace turboquant::core {

Rotation::Rotation(size_t dimensions) : dimensions_(dimensions) {
    if (dimensions_ == 0) {
        throw invalid_argument("[Rotation Error] Dimension size must be greater than zero.");
    }
}

Rotation::~Rotation() = default;

vector<float> Rotation::transform(const vector<float>& input, float angle) const {
    if (input.size() != dimensions_) {
        throw length_error("[Rotation Error] Input dimension size mismatch. Expected: " + 
            to_string(dimensions_) + ", Got: " + to_string(input.size()));
    }

    try {
        vector<float> output;
        output.reserve(input.size());

        float cosA = cos(angle);
        float sinA = sin(angle);

        // Process pairs of coordinates (Givens rotation)
        for (size_t i = 0; i < input.size(); ++i) {
            if (i + 1 < input.size()) {
                float x1 = input[i];
                float x2 = input[i + 1];

                // Apply 2D rotation matrix
                output.push_back(x1 * cosA - x2 * sinA);
                output.push_back(x1 * sinA + x2 * cosA);
                
                i++; // Increment index since we consumed two elements
            } else {
                // Handle edge case for odd-dimensional vectors
                output.push_back(input[i] * cosA);
            }
        }

        return output;

    } catch (const exception& e) {
        cerr << "[Rotation Exception] " << e.what() << endl;
        throw;
    }
}

size_t Rotation::getDimensions() const {
    return dimensions_;
}

} // namespace turboquant::core
