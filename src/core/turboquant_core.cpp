#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <cstring>
#include <iomanip>

using namespace std;

namespace turboquant::nn {

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

} // namespace turboquant::nn

extern "C" {
    turboquant::nn::TensorCompressor* create_tensor_compressor(size_t input_dim, size_t target_dim) {
        return new turboquant::nn::TensorCompressor(input_dim, target_dim);
    }

    void delete_tensor_compressor(turboquant::nn::TensorCompressor* ptr) {
        delete ptr;
    }

    void compress_tensor(
        turboquant::nn::TensorCompressor* ptr, 
        const float* input, 
        turboquant::nn::QuantizedBlock* output) {
        std::vector<turboquant::nn::QuantizedBlock> res = ptr->compress(input);
        std::memcpy(output, res.data(), res.size() * sizeof(turboquant::nn::QuantizedBlock));
    }

    void decompress_tensor(
        turboquant::nn::TensorCompressor* ptr, 
        const turboquant::nn::QuantizedBlock* input, 
        float* output) {
        ptr->decompress(input, output);
    }
}
