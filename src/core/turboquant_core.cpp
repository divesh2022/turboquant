#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <cstring>
#include <iomanip>

using namespace std;

namespace turboquant {
    namespace nn {

        struct alignas(8) QuantizedBlock {
            int8_t quantized_sign;
            float mean;
            float scale;
        };

        class TensorCompressor {
        public:
            TensorCompressor(size_t input_dim, size_t target_dim) 
                : input_dim_(input_dim), target_dim_(target_dim), step_(input_dim / target_dim) {
                if (input_dim % target_dim != 0) {
                    throw std::invalid_argument("Input dimension must be a multiple of target dimension.");
                }
            }

            std::vector<QuantizedBlock> compress(const float* input_data) {
                std::vector<QuantizedBlock> compressed;
                compressed.reserve(target_dim_);

                for (size_t i = 0; i < target_dim_; ++i) {
                    size_t startIdx = i * step_;
                    size_t endIdx = startIdx + step_;

                    float sum = 0.0f;
                    float min_val = 1e20f;
                    float max_val = -1e20f;

                    for (size_t j = startIdx; j < endIdx; ++j) {
                        float val = input_data[j];
                        sum += val;
                        if (val < min_val) min_val = val;
                        if (val > max_val) max_val = val;
                    }

                    float mean = sum / static_cast<float>(step_);
                    float scale = (max_val - min_val) / 2.0f;
                    if (scale < 1e-6f) scale = 1.0f;

                    float centered_sum = 0.0f;
                    for (size_t j = startIdx; j < endIdx; ++j) {
                        centered_sum += (input_data[j] - mean);
                    }

                    int8_t q_sign = (centered_sum >= 0.0f) ? 1 : -1;
                    compressed.push_back({q_sign, mean, scale});
                }
                return compressed;
            }

            std::vector<float> decompress(const std::vector<QuantizedBlock>& compressed_data) {
                std::vector<float> output_data(input_dim_, 0.0f);
                for (size_t i = 0; i < target_dim_; ++i) {
                    float sign = static_cast<float>(compressed_data[i].quantized_sign);
                    float mean = compressed_data[i].mean;
                    float scale = compressed_data[i].scale;

                    size_t startIdx = i * step_;
                    size_t endIdx = startIdx + step_;

                    for (size_t j = startIdx; j < endIdx; ++j) {
                        output_data[j] = mean + (sign * scale);
                    }
                }
                return output_data;
            }

            void decompress(const QuantizedBlock* compressed_data, float* output_data) {
                for (size_t i = 0; i < target_dim_; ++i) {
                    float sign = static_cast<float>(compressed_data[i].quantized_sign);
                    float mean = compressed_data[i].mean;
                    float scale = compressed_data[i].scale;

                    size_t startIdx = i * step_;
                    size_t endIdx = startIdx + step_;

                    for (size_t j = startIdx; j < endIdx; ++j) {
                        output_data[j] = mean + (sign * scale);
                    }
                }
            }

        private:
            size_t input_dim_;
            size_t target_dim_;
            size_t step_;
        };
    }
}

extern "C" {
    __declspec(dllexport) turboquant::nn::TensorCompressor* create_tensor_compressor(size_t input_dim, size_t target_dim) {
        return new turboquant::nn::TensorCompressor(input_dim, target_dim);
    }

    __declspec(dllexport) void delete_tensor_compressor(turboquant::nn::TensorCompressor* ptr) {
        delete ptr;
    }

    __declspec(dllexport) void compress_tensor(
        turboquant::nn::TensorCompressor* ptr, 
        const float* input, 
        turboquant::nn::QuantizedBlock* output) {
        std::vector<turboquant::nn::QuantizedBlock> res = ptr->compress(input);
        std::memcpy(output, res.data(), res.size() * sizeof(turboquant::nn::QuantizedBlock));
    }

    __declspec(dllexport) void decompress_tensor(
        turboquant::nn::TensorCompressor* ptr, 
        const turboquant::nn::QuantizedBlock* input, 
        float* output) {
        ptr->decompress(input, output);
    }
}

int main() {
    constexpr size_t INPUT_DIM = 16;
    constexpr size_t TARGET_DIM = 4;

    vector<float> inputData = {
        40.0052f, 14.0013f, 37.1357f, 42.5485f,
        -35.3085f, -44.8199f, -94.6600f, 11.9438f,
        -40.0949f, 59.7408f, 95.9132f, 24.6260f,
        72.4825f, 50.0488f, 80.0415f, -11.6856f
    };

    try {
        // Corrected namespace creation using the class defined above
        turboquant::nn::TensorCompressor compressor(INPUT_DIM, TARGET_DIM);

        vector<turboquant::nn::QuantizedBlock> compressed = compressor.compress(inputData.data());
        vector<float> decompressed = compressor.decompress(compressed);

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

        double compressionLoss = 10.0 * log10(originalL2Norm / errorL2Norm);

        cout << fixed << setprecision(4);
        cout << "\n================= TurboQuant Optimization =================" << endl;
        cout << "Original Dimensions   : " << INPUT_DIM << " | Targets: " << TARGET_DIM << endl;
        cout << "Mean Squared Error    : " << mse << endl;
        cout << "Compression Loss      : " << compressionLoss << " dB" << endl;
        cout << "===========================================================" << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}