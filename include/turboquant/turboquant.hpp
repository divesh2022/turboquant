#pragma once

#include "core/qjl.hpp"
#include "core/quantizer.hpp"
#include "core/rotation.hpp"
#include "transforms/polar.hpp"
#include "transforms/random_projection.hpp"
#pragma once

// Core Modules
#include "turboquant/core/qjl.hpp"
#include "turboquant/core/quantizer.hpp"
#include "turboquant/core/rotation.hpp"

// Transformation Modules
#include "turboquant/transforms/polar.hpp"
#include "turboquant/transforms/random_projection.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

namespace turboquant {

/**
 * @brief Main system version information.
 */
constexpr const char* VERSION = "1.0.0";

/**
 * @brief Utility function to verify the initialization of the library modules.
 */
inline void initializeSystem() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout << "--- TurboQuant System v" << VERSION << " Initialized ---\n";
}

} // namespace turboquant