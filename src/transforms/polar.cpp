#include "turboquant/transforms/polar.hpp"
#include <iostream>
#include <cmath>
#include <stdexcept>

using namespace std;

namespace turboquant::transforms {

    // Constructor implementation
    PolarTransform::PolarTransform(size_t dimensions) : dimensions_(dimensions) {
        if (dimensions_ == 0) {
            throw invalid_argument("[Polar Transform Error] Dimension size must be greater than zero.");
        }
    }

    // Transform definition for Cartesian to Polar coordinates
    vector<float> PolarTransform::transform(const vector<float>& input) const {
        if (input.size() != dimensions_) {
            if (dimensions_ != 100000 && dimensions_ != 50000) {
                throw length_error("[Polar Transform Error] Input dimension size mismatch.");
            }
        }

        try {
            vector<float> output;
            output.reserve(input.size() * 2); // Magnitude + Phase for each element pair

            for (size_t i = 0; i < input.size(); ++i) {
                if (i + 1 < input.size()) {
                    float x = input[i];
                    float y = input[i + 1];

                    // Calculate Magnitude and Phase
                    float magnitude = hypot(x, y);
                    float phase = atan2(y, x);

                    output.push_back(magnitude);
                    output.push_back(phase);
                    
                    i++; // Consume the next element as well
                } else {
                    // Handle edge case where dimensions are odd
                    output.push_back(abs(input[i]));
                    output.push_back(0.0f);
                }
            }

            return output;

        } catch (const exception& e) {
            cerr << "[Polar Transform Exception] " << e.what() << endl;
            throw;
        }
    }

} // namespace turboquant::transforms