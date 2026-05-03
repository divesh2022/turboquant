#pragma once
#include <vector>
#include <stdexcept>

namespace turboquant::transforms {

/**
 * @brief Polar coordinate transform for high-dimensional vectors.
 * Converts each Cartesian coordinate into (magnitude, phase).
 */
class PolarTransform {
public:
    explicit PolarTransform(size_t dimensions);

    virtual ~PolarTransform() = default;

    /**
     * @brief Applies polar transform to the input vector
     * @param input High-dimensional input vector
     * @return Output vector containing magnitude and phase for each element
     * @throws std::length_error if input size doesn't match dimensions
     */
    virtual std::vector<float> transform(const std::vector<float>& input) const;

    size_t getDimensions() const;

private:
    size_t dimensions_;
};

} // namespace turboquant::transforms
