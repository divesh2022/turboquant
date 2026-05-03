#pragma once
#include <vector>
#include <cstdint>

namespace turboquant::core {

class QJL {
public:
    // Constructor
    QJL(size_t input_dim, size_t target_dim);

    // Transform input vector into quantized output
    std::vector<int8_t> transform(const std::vector<float>& input);

    // Accessors
    size_t getInputDim() const;
    size_t getTargetDim() const;

private:
    size_t input_dim_;
    size_t target_dim_;
    std::vector<std::vector<float>> projection_matrix_;

    // Helper to initialize random projection matrix
    void initializeRandomProjection();
};

} // namespace turboquant::core
