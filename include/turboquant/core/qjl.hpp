#pragma once

#include <vector>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <cstdint> // Added the missing header for int8_t

using namespace std;

namespace turboquant::core {

/**
 * @brief Quantized Johnson-Lindenstrauss (QJL) Transform Engine.
 * Projects high-dimensional vectors to lower dimensions and applies integer quantization.
 */
class QJL {
public:
    /**
     * @brief Construct a new QJL engine.
     * @param inputDim Original dimensionality of input vectors.
     * @param targetDim Target dimensionality after projection.
     */
    explicit QJL(size_t inputDim = 1024, size_t targetDim = 128) 
        : inputDim_(inputDim), targetDim_(targetDim) {
        if (targetDim_ > inputDim_) {
            throw invalid_argument("[QJL Error] Target dimension cannot exceed input dimension.");
        }
        if (inputDim_ == 0 || targetDim_ == 0) {
            throw invalid_argument("[QJL Error] Dimensions must be greater than zero.");
        }
    }

    virtual ~QJL() = default;

    /**
     * @brief Transforms and quantizes input vector.
     * @param input High-dimensional float vector.
     * @return std::vector<int8_t> Quantized projected vector.
     */
    virtual vector<int8_t> transform(const vector<float>& input) const {
        if (input.size() != inputDim_) {
            throw length_error("[QJL Error] Input dimension size mismatch.");
        }

        try {
            // Project and quantize values using sign projection
            vector<int8_t> output;
            output.reserve(targetDim_);

            for (size_t i = 0; i < targetDim_; ++i) {
                float val = 0.0f;
                for (size_t j = 0; j < (inputDim_ / targetDim_); ++j) {
                    val += input[i * (inputDim_ / targetDim_) + j];
                }
                
                // Sign quantization to +1 or -1
                output.push_back(val > 0 ? 1 : -1);
            }

            return output;

        } catch (const exception& e) {
            cerr << "[QJL Exception] " << e.what() << endl;
            throw;
        }
    }

    size_t getInputDim() const { return inputDim_; }
    size_t getTargetDim() const { return targetDim_; }

private:
    size_t inputDim_;
    size_t targetDim_;
};

} // namespace turboquant::core