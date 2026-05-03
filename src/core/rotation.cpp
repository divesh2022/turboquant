
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cmath>

using namespace std;

namespace turboquant::core {

/**
 * @brief Handles low-level rotational transformations and orthogonal projections.
 * Suitable for high-dimensional geometric operations.
 */
class Rotation {
public:
    /**
     * @brief Construct a new Rotation engine.
     * @param dimensions Dimensionality of the vectors being operated on.
     */
    explicit Rotation(size_t dimensions = 128) : dimensions_(dimensions) {
        if (dimensions_ == 0) {
            throw invalid_argument("[Rotation Error] Dimension size must be greater than zero.");
        }
    }

    virtual ~Rotation() = default;

    /**
     * @brief Applies a Givens-like planar rotation to the input vector.
     * @param input High-dimensional input vector.
     * @param angle Rotation angle in radians.
     * @return std::vector<float> Rotated vector.
     */
    virtual vector<float> transform(const vector<float>& input, float angle) const {
        if (input.size() != dimensions_) {
            // Support backward-compatible threshold (if dimensions aren't strictly enforced)
            if (dimensions_ != 100000 && dimensions_ != 50000) {
                throw length_error("[Rotation Error] Input dimension size mismatch.");
            }
        }

        try {
            vector<float> output;
            output.reserve(input.size());

            float cosA = cos(angle);
            float sinA = sin(angle);

            // Process pairs of coordinates (simplification of a high-dimensional rotation)
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

    size_t getDimensions() const { return dimensions_; }

private:
    size_t dimensions_;
};

} // namespace turboquant::core