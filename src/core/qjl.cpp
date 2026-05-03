#include "turboquant/core/qjl.hpp"
#include <random>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <sstream>   // for safe string formatting

namespace turboquant::core {

// Constructor with explicit dimensions
QJL::QJL(size_t input_dim, size_t target_dim)
    : input_dim_(input_dim), target_dim_(target_dim) {
    if (target_dim > input_dim) {
        throw std::invalid_argument("[QJL Error] Target dimension cannot exceed input dimension.");
    }
    if (input_dim == 0 || target_dim == 0) {
        throw std::invalid_argument("[QJL Error] Dimensions must be greater than zero.");
    }
    initializeRandomProjection();
}

// Default constructor for tests/benchmarks
QJL::QJL() : input_dim_(128), target_dim_(64) {
    initializeRandomProjection();
}

// Initialize random projection matrix
void QJL::initializeRandomProjection() {
    std::mt19937 rng(42); // fixed seed for reproducibility
    std::normal_distribution<float> dist(0.0f, 1.0f);

    projection_matrix_.resize(target_dim_);
    for (size_t i = 0; i < target_dim_; ++i) {
        projection_matrix_[i].resize(input_dim_);
        for (size_t j = 0; j < input_dim_; ++j) {
            projection_matrix_[i][j] = dist(rng);
        }
    }
}

// Transform input vector into quantized output
std::vector<int8_t> QJL::transform(const std::vector<float>& input) {
    if (input.size() != input_dim_) {
        std::ostringstream oss;
        oss << "[QJL Error] Input size mismatch. Expected: "
            << input_dim_ << ", Got: " << input.size();
        throw std::length_error(oss.str());
    }

    std::vector<int8_t> output(target_dim_);
    for (size_t i = 0; i < target_dim_; ++i) {
        float projected = 0.0f;
        for (size_t j = 0; j < input_dim_; ++j) {
            projected += projection_matrix_[i][j] * input[j];
        }

        // Normalize and quantize to int8 range [-128, 127]
        float quantized = projected / std::sqrt(static_cast<float>(input_dim_));
        output[i] = static_cast<int8_t>(std::max(-128.0f, std::min(127.0f, quantized)));
    }
    return output;
}

// Accessors
size_t QJL::getInputDim() const {
    return input_dim_;
}

size_t QJL::getTargetDim() const {
    return target_dim_;
}

} // namespace turboquant::core
