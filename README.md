
---

# TurboQuant 

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![Version](https://img.shields.io/badge/version-1.0.0-brightgreen.svg)]()
[![Platform](https://img.shields.io/badge/platform-Linux%20|%20Windows-lightgrey.svg)]()

**TurboQuant** is a modular, low-latency numerical computation and quantitative engine optimized for memory-constrained, high-frequency trading (HFT) environments and high-dimensional matrix manipulations.

---

## Architecture Overview

The framework is designed to have zero dependencies by default, utilizing efficient SIMD data alignment, standard exception handling, and highly-optimized template structures.



---

## Core Features

* **QJL (Quantized Johnson-Lindenstrauss) Transform:** High-dimensional vector projection and quantization engine.
* **Random Projection:** Gaussian random projection matrix mapping designed to preserve Euclidean distances.
* **Transforms:** Modular Cartesian to polar coordinate transformation modules.
* **Quantizer Module:** Flexible scalar quantization parameterized for varying bit-depth precision.
* **Rotation Module:** Givens-like planar rotation matrix computations optimized for high dimensions.

---

## Getting Started

### Prerequisites

* **Compiler:** Supporting C++17 (GCC 9+ or Clang 10+).
* **Build System:** CMake (version 3.12 or higher) or GNU Make.
* **Optional Dependency:** Intel oneAPI Math Kernel Library (MKL).

### Quick Setup & Build

Clone the repository and run the build sequence in your terminal:

```bash
# Clone and navigate
git clone https://github.com/your-username/turboquant.git
cd turboquant

# Build via CMake
mkdir -p build && cd build
cmake ..
cmake --build .
```

---

## Running the Test Suite

TurboQuant uses native integration tests to verify internal operations and avoid One Definition Rule (ODR) violations. Run tests using the following targets:

```bash
# Run unit tests
ctest --output-on-failure

# Or run manual test binaries
./build/test_qjl
./build/test_turboquant
```

---

## Project Structure

```text
turboquant/
├── include/
│   └── turboquant/
│       ├── core/
│       │   ├── qjl.hpp
│       │   ├── quantizer.hpp
│       │   └── rotation.hpp
│       ├── transforms/
│       │   ├── polar.hpp
│       │   └── random_projection.hpp
│       └── turboquant.hpp
├── src/
│   ├── transforms/
│   │   └── polar.cpp
├── tests/
│   ├── test_qjl.cpp
│   └── test_turboquant.cpp
├── cmake/
│   └── FindMKL.cmake
├── CMakeLists.txt
├── Makefile
└── README.md
```

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.