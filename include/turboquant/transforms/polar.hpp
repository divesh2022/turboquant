#pragma once

#include <vector>
#include <stdexcept>
#include <iostream>
#include <cmath>

using namespace std;

namespace turboquant::transforms {

/**
 * @brief Handles polar transformations for multi-dimensional signals.
 * Converts Cartesian coordinates to magnitude and phase representations.
 */
class PolarTransform {
public:
    /**
     * @brief Construct a new Polar Transform object.
     * @param dimensions Dimensionality of the input vectors.
     */
    explicit PolarTransform(size_t dimensions = 128);

    virtual ~PolarTransform() = default;

    /**
     * @brief Transforms Cartesian coordinate input into magnitude and phase representation.
     * @param input High-dimensional Cartesian input vector.
     * @return std::vector<float> Interleaved magnitude/phase values.
     */
    virtual vector<float> transform(const vector<float>& input) const;

    /**
     * @brief Get the configured dimensions of the transform.
     */
    size_t getDimensions() const { return dimensions_; }

private:
    size_t dimensions_;
};

} // namespace turboquant::transforms