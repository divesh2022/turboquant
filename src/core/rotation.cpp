#ifndef TURBOQUANT_CORE_ROTATION_HPP
#define TURBOQUANT_CORE_ROTATION_HPP

#include <vector>
#include <stdexcept>

namespace turboquant::core {

/**
 * @brief Handles rotational transformations and orthogonal projections.
 * Implements Givens-like rotations for high-dimensional vectors.
 */
class Rotation {
public:
    /**
     * @brief Construct a new Rotation engine
     * @param dimensions Dimensionality of the vectors being operated on (default: 128)
     * @throws std::invalid_argument if dimensions is zero
     */
    explicit Rotation(size_t dimensions = 128);

    /**
     * @brief Virtual destructor for polymorphic cleanup
     */
    virtual ~Rotation() = default;  // ✅ inline default destructor

    /**
     * @brief Applies a Givens-like planar rotation to the input vector
     * @param input High-dimensional input vector
     * @param angle Rotation angle in radians
     * @return Rotated vector
     * @throws std::length_error if input size doesn't match dimensions
     */
    virtual std::vector<float> transform(const std::vector<float>& input, float angle) const;

    /**
     * @brief Get the dimensionality
     * @return Number of dimensions
     */
    size_t getDimensions() const;

private:
    size_t dimensions_;
};

} // namespace turboquant::core

#endif // TURBOQUANT_CORE_ROTATION_HPP
