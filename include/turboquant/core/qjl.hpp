#ifndef TURBOQUANT_CORE_QJL_HPP
#define TURBOQUANT_CORE_QJL_HPP

#include <vector>
#include <cstdint>
#include <stdexcept>

namespace turboquant::core {

/**
 * @brief Quantized Johnson-Lindenstrauss Transform (QJL)
 * 
 * Implements high-dimensional vector projection and quantization
 * using random projection matrices and int8 quantization.
 */
class QJL {
public:
    /**
     * @brief Construct QJL transformer
     * @param input_dim Input vector dimensionality
     * @param target_dim Target output dimensionality
     * @throws invalid_argument if dimensions are invalid
     */
    QJL(size_t input_dim, size_t target_dim);

    /**
     * @brief Transform input vector to quantized lower dimension
     * @param input Input vector of size input_dim
     * @return Quantized vector of size target_dim with int8 values [-128, 127]
     * @throws length_error if input size doesn't match input_dim
     */
    std::vector<int8_t> transform(const std::vector<float>& input);

    /**
     * @brief Get input dimensionality
     * @return Input dimension
     */
    size_t getInputDim() const;

    /**
     * @brief Get target dimensionality
     * @return Target dimension
     */
    size_t getTargetDim() const;

private:
    size_t input_dim_;
    size_t target_dim_;
    std::vector<std::vector<float>> projection_matrix_;

    /**
     * @brief Initialize random projection matrix
     */
    void initializeRandomProjection();
};

} // namespace turboquant::core

#endif // TURBOQUANT_CORE_QJL_HPP
