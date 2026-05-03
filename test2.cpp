#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <random>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <fstream>

namespace turboquant {
    void initializeSystem() { 
        std::ios_base::sync_with_stdio(false); 
        std::cin.tie(nullptr); 
    }

    namespace core {
        // Quantized block structure for storing scales and signs per target index
        struct alignas(8) QuantizedBlock {
            int8_t quantized_sign;
            float mean;
            float scale;
        };

        class ProjectedCompressor {
        public:
            ProjectedCompressor(size_t inputDim, size_t targetDim)
                : inputDim_(inputDim), targetDim_(targetDim), step_(inputDim / targetDim) {
                if (inputDim % targetDim != 0) {
                    throw std::invalid_argument("Input dimension must be a multiple of target dimension.");
                }
            }

            std::vector<QuantizedBlock> compress(const std::vector<float>& input) {
                if (input.size() != inputDim_) {
                    throw std::length_error("Input dimension size mismatch.");
                }

                std::vector<QuantizedBlock> compressed;
                compressed.reserve(targetDim_);

                for (size_t i = 0; i < targetDim_; ++i) {
                    size_t startIdx = i * step_;
                    size_t endIdx = startIdx + step_;

                    float sum = 0.0f;
                    float min_val = 1e20f;
                    float max_val = -1e20f;

                    for (size_t j = startIdx; j < endIdx; ++j) {
                        float val = input[j];
                        sum += val;
                        if (val < min_val) min_val = val;
                        if (val > max_val) max_val = val;
                    }

                    float mean = sum / static_cast<float>(step_);
                    float scale = (max_val - min_val) / 2.0f;
                    if (scale < 1e-6f) scale = 1.0f;

                    float centered_sum = 0.0f;
                    for (size_t j = startIdx; j < endIdx; ++j) {
                        centered_sum += (input[j] - mean);
                    }

                    int8_t q_sign = (centered_sum >= 0.0f) ? 1 : -1;
                    compressed.push_back({q_sign, mean, scale});
                }

                return compressed;
            }

            std::vector<float> decompress(const std::vector<QuantizedBlock>& compressed) const {
                if (compressed.size() != targetDim_) {
                    throw std::length_error("Compressed dimension size mismatch.");
                }

                std::vector<float> reconstructed(inputDim_, 0.0f);

                for (size_t i = 0; i < targetDim_; ++i) {
                    float sign = static_cast<float>(compressed[i].quantized_sign);
                    float mean = compressed[i].mean;
                    float scale = compressed[i].scale;

                    size_t startIdx = i * step_;
                    size_t endIdx = startIdx + step_;

                    for (size_t j = startIdx; j < endIdx; ++j) {
                        reconstructed[j] = mean + (sign * scale);
                    }
                }

                return reconstructed;
            }

        private:
            size_t inputDim_;
            size_t targetDim_;
            size_t step_;
        };
    }
}

int main() {
    turboquant::initializeSystem();

    constexpr size_t INPUT_DIM = 16;
    constexpr size_t TARGET_DIM = 4; // Testing the 4:1 configuration

    std::vector<float> inputData = {
        40.0052f, 14.0013f, 37.1357f, 42.5485f,
        -35.3085f, -44.8199f, -94.6600f, 11.9438f,
        -40.0949f, 59.7408f, 95.9132f, 24.6260f,
        72.4825f, 50.0488f, 80.0415f, -11.6856f
    };

    turboquant::core::ProjectedCompressor compressor(INPUT_DIM, TARGET_DIM);

    std::vector<turboquant::core::QuantizedBlock> compressed = compressor.compress(inputData);
    std::vector<float> decompressed = compressor.decompress(compressed);

    double originalL2Norm = 0.0;
    double errorL2Norm = 0.0;
    double mse = 0.0;

    for (size_t i = 0; i < INPUT_DIM; ++i) {
        originalL2Norm += inputData[i] * inputData[i];
        double diff = inputData[i] - decompressed[i];
        errorL2Norm += diff * diff;
        mse += diff * diff;
    }
    mse /= INPUT_DIM;

    double l2RelativeError = std::sqrt(errorL2Norm / originalL2Norm);
    double compressionLoss = 10.0 * std::log10(originalL2Norm / errorL2Norm);
    double fitScore = 1.0 - std::min(1.0, l2RelativeError / 0.42);

    std::ofstream outFile("compression_output.txt");
    if (outFile.is_open()) {
        outFile << "===== TurboQuant Validation Output =====" << std::endl;
        outFile << "Original Dimensions   : " << INPUT_DIM << " | Targets: " << TARGET_DIM << std::endl;
        outFile << "Mean Squared Error    : " << mse << std::endl;
        outFile << "Compression Loss      : " << compressionLoss << " dB" << std::endl;
        outFile << "Model Fit Score       : " << fitScore << std::endl;
        outFile << "----------------------------------------" << std::endl;
        for (size_t i = 0; i < INPUT_DIM; ++i) {
            outFile << "Idx " << i << " | Original: " << inputData[i] << " | Reconstructed: " << decompressed[i] << std::endl;
        }
        outFile << "========================================" << std::endl;
        outFile.close();
    }

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\n================= Compression Analysis =================" << std::endl;
    std::cout << "Original Dimensions   : " << INPUT_DIM << " | Targets: " << TARGET_DIM << std::endl;
    std::cout << "Mean Squared Error    : " << mse << std::endl;
    std::cout << "Compression Loss      : " << compressionLoss << " dB" << std::endl;
    std::cout << "L2 Relative Error     : " << l2RelativeError << std::endl;
    std::cout << "Model Fit Score       : " << fitScore << std::endl;
    std::cout << "Data saved to compression_output.txt" << std::endl;
    std::cout << "========================================================" << std::endl;

    return 0;
}