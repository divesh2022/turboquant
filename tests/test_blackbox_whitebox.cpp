/**
 * @file test_blackbox_whitebox.cpp
 * @brief Comprehensive Black Box and White Box Test Suite for TurboQuant
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
#include <cstdlib>

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
// BLACK BOX TESTS
// ============================================================================

namespace BlackBoxTests {
    // ... (unchanged QJL and Rotation tests)

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
                for (size_t i = 0; i < output.size(); i += 2) {
                    assert(output[i] >= 0.0f);
                }
            } catch (const exception& e) {
                result.passed = false;
                result.message = e.what();
            }
        });
        result.execution_time_ms = time;
        return result;
    }
} // namespace BlackBoxTests

// ============================================================================
// WHITE BOX TESTS
// ============================================================================

namespace WhiteBoxTests {

    // ... (unchanged QJL and Rotation tests)

    TestResult testPolarTransformMagnitudeCalculation() {
        TestResult result{"PolarTransform: Magnitude Calculation Accuracy", true, "", 0.0};
        double time = measureTime([&]() {
            try {
                PolarTransform polar(4);
                vector<float> input = {3.0f, 4.0f, 0.0f, 0.0f};
                auto output = polar.transform(input);

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

                vector<float> input1 = {1.0f, 0.0f, 0.0f, 0.0f};
                auto output1 = polar.transform(input1);
                assert(abs(output1[1]) < 0.01f);

                vector<float> input2 = {0.0f, 1.0f, 0.0f, 0.0f};
                auto output2 = polar.transform(input2);
                assert(abs(output2[3] - 1.5708f) < 0.01f);
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
// MAIN
// ============================================================================

int main() {
    cout << "\n" << string(80, '=') << endl;
    cout << "    TurboQuant: Comprehensive Black Box & White Box Test Suite" << endl;
    cout << string(80, '=') << endl;

    TestSuite suite;

    // Run BLACK BOX TESTS
    cout << "\n[1/2] Running BLACK BOX TESTS..." << endl;
    cout << string(80, '-') << endl;
    suite.addResult(BlackBoxTests::testPolarTransformValidInput());

    // Run WHITE BOX TESTS
    cout << "\n[2/2] Running WHITE BOX TESTS..." << endl;
    cout << string(80, '-') << endl;
    suite.addResult(WhiteBoxTests::testPolarTransformMagnitudeCalculation());
    suite.addResult(WhiteBoxTests::testPolarTransformPhaseCalculation());

    suite.printReport();
    return suite.allPassed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
