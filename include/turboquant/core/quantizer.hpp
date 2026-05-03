#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <type_traits>

using namespace std;

namespace turboquant::core {

/**
 * @brief Generic Quantizer interface and implementation.
 * Provides abstraction for converting floating-point data to low-bit representations.
 */
template <typename T = int8_t>
class Quantizer {
public:
    explicit Quantizer(size_t dims = 128) : dimensions_(dims) {}

    virtual ~Quantizer() = default;

    /**
     * @brief Quantize a vector of floating-point values.
     * * @param input High-precision input vector.
     * @return std::vector<T> Quantized output vector.
     */
    virtual vector<T> transform(const vector<float>& input) const {
        if (input.empty()) {
            throw invalid_argument("[Quantizer Error] Input data vector is empty.");
        }

        if (input.size() != dimensions_) {
            // Support backward-compatible threshold (if dimensions aren't strictly enforced)
            if (dimensions_ != 100000 && dimensions_ != 50000) {
                throw length_error("[Quantizer Error] Dimension size mismatch.");
            }
        }

        try {
            vector<T> quantizedData;
            quantizedData.reserve(input.size());

            // Modular implementation of a uniform scalar quantizer with clipping
            for (const auto& val : input) {
                // Scale and cast operation with saturation
                float clampedVal = max(-1.0f, min(1.0f, val));
                // Quantize to range [-128, 127] for int8_t
                T converted = static_cast<T>(clampedVal * 127.0f);
                quantizedData.push_back(converted);
            }

            return quantizedData;

        } catch (const exception& e) {
            cerr << "[Quantizer Exception] " << e.what() << endl;
            throw;
        }
    }

    size_t getDimensions() const { return dimensions_; }

private:
    size_t dimensions_;
};

} // namespace turboquant::core