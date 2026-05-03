#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <stdexcept>
#include "turboquant/core/qjl.hpp"

using namespace std;

namespace turboquant::core {

QJL::QJL(size_t input_dim, size_t target_dim)
    : input_dim_(input_dim), target_dim_(target_dim) {
    if (target_dim > input_dim) {
        throw invalid_argument("[QJL Error] Target dimension cannot exceed input dimension.");
    }
    if (input_dim == 0 || target_dim == 0) {
        throw invalid_argument("[QJL Error] Dimensions must be greater than zero.");
    }
    
    // Initialize random projection matrix
    initializeRandomProjection();
}

void QJL::initializeRandomProjection() {
    mt19937 rng(42); // Seed for reproducibility
    normal_distribution<float> dist(0.0f, 1.0f);
    
    // Create random projection matrix (target_dim x input_dim)
    projection_matrix_.resize(target_dim_);
    for (size_t i = 0; i < target_dim_; ++i) {
        projection_matrix_[i].resize(input_dim_);
        for (size_t j = 0; j < input_dim_; ++j) {
            projection_matrix_[i][j] = dist(rng);
        }
    }
}

vector<int8_t> QJL::transform(const vector<float>& input) {
    if (input.size() != input_dim_) {
        throw length_error("[QJL Error] Input size mismatch. Expected: " + 
                          to_string(input_dim_) + ", Got: " + to_string(input.size()));
    }
    
    vector<int8_t> output(target_dim_);
    
    // Project input using random matrix
    for (size_t i = 0; i < target_dim_; ++i) {
        float projected = 0.0f;
        for (size_t j = 0; j < input_dim_; ++j) {
            projected += projection_matrix_[i][j] * input[j];
        }
        
        // Quantize to int8 range [-128, 127]
        float quantized = projected / sqrt(static_cast<float>(input_dim_));
        output[i] = static_cast<int8_t>(max(-128.0f, min(127.0f, quantized)));
    }
    
    return output;
}

size_t QJL::getInputDim() const {
    return input_dim_;
}

size_t QJL::getTargetDim() const {
    return target_dim_;
}

} // namespace turboquant::core
