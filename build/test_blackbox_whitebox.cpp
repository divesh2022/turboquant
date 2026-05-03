/**
 * @file test_blackbox_whitebox.cpp
 * @brief Comprehensive Black Box and White Box Test Suite for TurboQuant
 * 
 * BLACK BOX TESTS: Test external behavior and API contracts
 * WHITE BOX TESTS: Test internal implementation and state
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include <stdexcept>
#include <random>
#include <chrono>
#include <iomanip>
#include <algorithm>

#include "turboquant/core/qjl.hpp"
#include "turboquant/core/rotation.hpp"
#include "turboquant/transforms/polar.hpp"

using namespace std;
using namespace turboquant;
using namespace turboquant::core;
using namespace turboquant::transforms;

// ============================================================================
// TEST UTILITIES & HELPERS
// ============================================================================

struct TestResult {
    string name;
    bool passed;
    string message;
    double execution_time_ms;
};

class TestSuite {
private:
    vector<TestResult> results_;
    size_t total_tests_ = 0;
    size_t passed_tests_ = 0;

public:
    void addResult(const TestResult& result) {
        results_.push_back(result);
        total_tests_++;
        if (result.passed) {
            passed_tests_++;
        }
    }

    void printReport() const {
        cout << "\n" << string(80, '=') << endl;
        cout << "                    TEST EXECUTION REPORT" << endl;
        cout << string(80, '=') << endl;

        for (const auto& result : results_) {
            string status = result.passed ? "[✓ PASS]" : "[✗ FAIL]";
            cout << left << setw(20) << status
                 << setw(50) << result.name
                 << right << setw(10) << fixed << setprecision(4)
                 << result.execution_time_ms << " ms" << endl;
            if (!result.message.empty() && !result.passed) {
                cout << "  └─ Error: " << result.message << endl;
            }
        }

        cout << "\n" << string(80, '-') << endl;
        cout << "Total: " << total_tests_ << " | Passed: " << passed_tests_
             << " | Failed: " << (total_tests_ - passed_tests_) << endl;
        cout << "Success Rate: " << fixed << setprecision(2)
             << (100.0 * passed_tests_ / total_tests_) << "%" << endl;
        cout << string(80, '=') << endl;
    }

    bool allPassed() const { return passed_tests_ == total_tests_; }
};

// Helper to measure execution time
template <typename Func>
double measureTime(Func f) {
    auto start = chrono::high_resolution_clock::now();
    f();
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(end - start).count();
}

// ============================================================================
// BLACK BOX TESTS - Test External Behavior & API Contracts
// ============================================================================

namespace BlackBoxTests {

    TestResult testQJLConstructorValidDimensions() {
        TestResult result{"QJL: Valid Constructor Dimensions", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                QJL qjl(1024, 128);
                assert(qjl.getInputDim() == 1024);
                assert(qjl.getTargetDim() == 128);
            } catch (const exception& e) {
                result.passed = false;
                result.message = e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testQJLConstructorInvalidDimensions() {
        TestResult result{"QJL: Invalid Constructor (target > input)", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                QJL qjl(128, 256); // Invalid: target > input
                result.passed = false;
                result.message = "Should throw invalid_argument";
            } catch (const invalid_argument&) {
                result.passed = true;
            } catch (const exception& e) {
                result.passed = false;
                result.message = string("Wrong exception: ") + e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testQJLConstructorZeroDimensions() {
        TestResult result{"QJL: Invalid Constructor (zero dimensions)", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                QJL qjl(0, 0);
                result.passed = false;
                result.message = "Should throw invalid_argument";
            } catch (const invalid_argument&) {
                result.passed = true;
            } catch (const exception& e) {
                result.passed = false;
                result.message = string("Wrong exception: ") + e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testQJLTransformValidInput() {
        TestResult result{"QJL: Transform Valid Input", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                QJL qjl(512, 64);
                vector<float> input(512, 0.5f);
                auto output = qjl.transform(input);

                assert(output.size() == 64);
                for (auto val : output) {
                    assert(val >= -128 && val <= 127);
                }
            } catch (const exception& e) {
                result.passed = false;
                result.message = e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testQJLTransformInvalidInputSize() {
        TestResult result{"QJL: Transform Invalid Input Size", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                QJL qjl(512, 64);
                vector<float> input(256, 0.5f); // Wrong size
                auto output = qjl.transform(input);
                result.passed = false;
                result.message = "Should throw length_error";
            } catch (const length_error&) {
                result.passed = true;
            } catch (const exception& e) {
                result.passed = false;
                result.message = string("Wrong exception: ") + e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testQJLDeterministicOutput() {
        TestResult result{"QJL: Deterministic Output", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                QJL qjl(256, 32);
                vector<float> input(256);
                for (size_t i = 0; i < 256; ++i) {
                    input[i] = sin(static_cast<float>(i) / 256.0f);
                }

                auto output1 = qjl.transform(input);
                auto output2 = qjl.transform(input);
                assert(output1 == output2);
            } catch (const exception& e) {
                result.passed = false;
                result.message = e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testRotationConstructorValid() {
        TestResult result{"Rotation: Valid Constructor", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                Rotation rot(256);
                assert(rot.getDimensions() == 256);
            } catch (const exception& e) {
                result.passed = false;
                result.message = e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testRotationConstructorZeroDimension() {
        TestResult result{"Rotation: Invalid Constructor (zero dimensions)", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                Rotation rot(0);
                result.passed = false;
                result.message = "Should throw invalid_argument";
            } catch (const invalid_argument&) {
                result.passed = true;
            } catch (const exception& e) {
                result.passed = false;
                result.message = string("Wrong exception: ") + e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testRotationTransformValidInput() {
        TestResult result{"Rotation: Transform Valid Input", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                Rotation rot(128);
                vector<float> input(128, 1.0f);
                auto output = rot.transform(input, 0.785398f); // 45 degrees

                assert(output.size() == 128);
                // Check that output is different from input (after rotation)
                bool changed = false;
                for (size_t i = 0; i < 128; ++i) {
                    if (abs(output[i] - input[i]) > 1e-6f) {
                        changed = true;
                        break;
                    }
                }
                assert(changed);
            } catch (const exception& e) {
                result.passed = false;
                result.message = e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testRotationTransformInvalidSize() {
        TestResult result{"Rotation: Transform Invalid Size", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                Rotation rot(128);
                vector<float> input(256, 1.0f); // Wrong size
                auto output = rot.transform(input, 0.785398f);
                result.passed = false;
                result.message = "Should throw length_error";
            } catch (const length_error&) {
                result.passed = true;
            } catch (const exception& e) {
                result.passed = false;
                result.message = string("Wrong exception: ") + e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testPolarTransformValidInput() {
        TestResult result{"PolarTransform: Valid Transform", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                PolarTransform polar(256);
                vector<float> input(256);
                for (size_t i = 0; i < 256; ++i) {
                    input[i] = static_cast<float>(i % 10);
                }
                auto output = polar.transform(input);

                assert(output.size() == input.size() * 2);
                // Check magnitude values are non-negative
                for (size_t i = 0; i < output.size(); i += 2) {
                    assert(output[i] >= 0.0f); // Magnitude must be >= 0
                }
            } catch (const exception& e) {
                result.passed = false;
                result.message = e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testPolarTransformZeroDimension() {
        TestResult result{"PolarTransform: Invalid Constructor", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                PolarTransform polar(0);
                result.passed = false;
                result.message = "Should throw invalid_argument";
            } catch (const invalid_argument&) {
                result.passed = true;
            } catch (const exception& e) {
                result.passed = false;
                result.message = string("Wrong exception: ") + e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

} // namespace BlackBoxTests

// ============================================================================
// WHITE BOX TESTS - Test Internal Implementation & State
// ============================================================================

namespace WhiteBoxTests {

    TestResult testQJLProjectionMatrixInitialization() {
        TestResult result{"QJL: Projection Matrix Initialization", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                QJL qjl(512, 128);
                
                // Run transform to ensure matrix is initialized
                vector<float> input(512, 1.0f);
                auto output = qjl.transform(input);

                // Verify output properties
                assert(output.size() == 128);
                
                // Check that different runs with different inputs produce different outputs
                vector<float> input2(512, 2.0f);
                auto output2 = qjl.transform(input2);
                bool different = false;
                for (size_t i = 0; i < 128; ++i) {
                    if (output[i] != output2[i]) {
                        different = true;
                        break;
                    }
                }
                assert(different);
            } catch (const exception& e) {
                result.passed = false;
                result.message = e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testQJLQuantizationRange() {
        TestResult result{"QJL: Quantization Range Enforcement", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                QJL qjl(1024, 256);
                
                // Test with extreme input values
                vector<float> extreme_input(1024);
                for (size_t i = 0; i < 1024; ++i) {
                    extreme_input[i] = (i % 2 == 0) ? 1000.0f : -1000.0f;
                }
                
                auto output = extreme_input.size() > 0 ? qjl.transform(extreme_input) : vector<int8_t>();
                
                // Verify all outputs are within quantization range
                for (auto val : output) {
                    assert(val >= -128 && val <= 127);
                }
            } catch (const exception& e) {
                result.passed = false;
                result.message = e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testRotationPreservesNorm() {
        TestResult result{"Rotation: Norm Preservation (Approximate)", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                Rotation rot(128);
                vector<float> input(128);
                
                // Initialize with unit vector
                mt19937 rng(42);
                normal_distribution<float> dist(0.0f, 1.0f);
                float norm_input = 0.0f;
                for (size_t i = 0; i < 128; ++i) {
                    input[i] = dist(rng);
                    norm_input += input[i] * input[i];
                }
                norm_input = sqrt(norm_input);

                // Apply rotation
                auto output = rot.transform(input, 0.5f);
                
                // Calculate output norm
                float norm_output = 0.0f;
                for (auto val : output) {
                    norm_output += val * val;
                }
                norm_output = sqrt(norm_output);

                // Rotations should approximately preserve norm (within 10% tolerance)
                float tolerance = 0.1f * norm_input;
                assert(abs(norm_output - norm_input) < tolerance);
            } catch (const exception& e) {
                result.passed = false;
                result.message = e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testRotationMultipleAngles() {
        TestResult result{"Rotation: Different Angles Produce Different Results", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                Rotation rot(64);
                vector<float> input(64, 1.5f);

                auto output1 = rot.transform(input, 0.0f);     // No rotation
                auto output2 = rot.transform(input, 1.5708f);  // 90 degrees
                auto output3 = rot.transform(input, 3.14159f); // 180 degrees

                // Verify different angles produce different outputs
                bool diff_90 = false, diff_180 = false;
                for (size_t i = 0; i < 64; ++i) {
                    if (abs(output1[i] - output2[i]) > 1e-4f) diff_90 = true;
                    if (abs(output1[i] - output3[i]) > 1e-4f) diff_180 = true;
                }
                assert(diff_90 && diff_180);
            } catch (const exception& e) {
                result.passed = false;
                result.message = e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testPolarTransformMagnitudeCalculation() {
        TestResult result{"PolarTransform: Magnitude Calculation Accuracy", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                PolarTransform polar(4); // Small dimension for manual verification
                
                // Known values: (3, 4) should give magnitude 5
                vector<float> input = {3.0f, 4.0f, 0.0f, 0.0f};
                auto output = polar.transform(input);

                // First pair: magnitude should be 5.0
                float expected_magnitude = 5.0f;
                float actual_magnitude = output[0];
                
                assert(abs(actual_magnitude - expected_magnitude) < 0.01f);
            } catch (const exception& e) {
                result.passed = false;
                result.message = e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testPolarTransformPhaseCalculation() {
        TestResult result{"PolarTransform: Phase Calculation", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                PolarTransform polar(4);
                
                // Point on positive x-axis: phase should be 0
                vector<float> input1 = {1.0f, 0.0f, 0.0f, 0.0f};
                auto output1 = polar.transform(input1);
                assert(abs(output1[1]) < 0.01f); // Phase ≈ 0

                // Point on positive y-axis: phase should be π/2
                vector<float> input2 = {0.0f, 1.0f, 0.0f, 0.0f};
                auto output2 = polar.transform(input2);
                assert(abs(output2[3] - 1.5708f) < 0.01f); // Phase ≈ π/2
            } catch (const exception& e) {
                result.passed = false;
                result.message = e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

    TestResult testQJLConsistencyAcrossRuns() {
        TestResult result{"QJL: Consistency Across Multiple Runs", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                QJL qjl(256, 32);
                vector<float> input(256, 2.5f);

                vector<vector<int8_t>> outputs;
                for (int i = 0; i < 5; ++i) {
                    outputs.push_back(qjl.transform(input));
                }

                // All outputs should be identical
                for (size_t i = 1; i < outputs.size(); ++i) {
                    assert(outputs[0] == outputs[i]);
                }
            } catch (const exception& e) {
                result.passed = false;
                result.message = e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }

} // namespace WhiteBoxTests

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main() {
    cout << "\n" << string(80, '=') << endl;
    cout << "    TurboQuant: Comprehensive Black Box & White Box Test Suite" << endl;
    cout << string(80, '=') << endl;

    TestSuite suite;

    // Run BLACK BOX TESTS
    cout << "\n[1/2] Running BLACK BOX TESTS (External Behavior)..." << endl;
    cout << string(80, '-') << endl;

    suite.addResult(BlackBoxTests::testQJLConstructorValidDimensions());
    suite.addResult(BlackBoxTests::testQJLConstructorInvalidDimensions());
    suite.addResult(BlackBoxTests::testQJLConstructorZeroDimensions());
    suite.addResult(BlackBoxTests::testQJLTransformValidInput());
    suite.addResult(BlackBoxTests::testQJLTransformInvalidInputSize());
    suite.addResult(BlackBoxTests::testQJLDeterministicOutput());
    suite.addResult(BlackBoxTests::testRotationConstructorValid());
    suite.addResult(BlackBoxTests::testRotationConstructorZeroDimension());
    suite.addResult(BlackBoxTests::testRotationTransformValidInput());
    suite.addResult(BlackBoxTests::testRotationTransformInvalidSize());
    suite.addResult(BlackBoxTests::testPolarTransformValidInput());
    suite.addResult(BlackBoxTests::testPolarTransformZeroDimension());

    // Run WHITE BOX TESTS
    cout << "\n[2/2] Running WHITE BOX TESTS (Internal Implementation)..." << endl;
    cout << string(80, '-') << endl;

    suite.addResult(WhiteBoxTests::testQJLProjectionMatrixInitialization());
    suite.addResult(WhiteBoxTests::testQJLQuantizationRange());
    suite.addResult(WhiteBoxTests::testRotationPreservesNorm());
    suite.addResult(WhiteBoxTests::testRotationMultipleAngles());
    suite.addResult(WhiteBoxTests::testPolarTransformMagnitudeCalculation());
    suite.addResult(WhiteBoxTests::testPolarTransformPhaseCalculation());
    suite.addResult(WhiteBoxTests::testQJLConsistencyAcrossRuns());

    // Print final report
    suite.printReport();

    return suite.allPassed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
