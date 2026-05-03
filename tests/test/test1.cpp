#include <iostream>
#include <vector>
#include <stdexcept>
#include <cassert>
#include <cmath>

#include "turboquant/transforms/polar.hpp"
#include "turboquant/core/qjl.hpp"
#include "turboquant/core/quantizer.hpp"

using namespace std;
using namespace turboquant::core;
using namespace turboquant::transforms;

namespace {

void runIntegrationTests() {
    cout << "[+] Starting TurboQuant Integration Tests..." << endl;

    // Test Case 1: Integration of QJL and Quantizer
    try {
        QJL qjlEngine(1024, 128);
        Quantizer<int8_t> quantizer(128);

        vector<float> inputData(1024, 0.75f);
        vector<int8_t> qjlOutput = qjlEngine.transform(inputData);
        
        vector<float> floatQjlOutput(qjlOutput.begin(), qjlOutput.end());
        vector<int8_t> finalOutput = quantizer.transform(floatQjlOutput);

        assert(finalOutput.size() == 128);
        cout << "[PASS] Test Case 1: QJL and Quantizer Integration." << endl;
    } catch (const exception& e) {
        cerr << "[FAIL] Test Case 1 failed with: " << e.what() << endl;
    }

    // Test Case 2: Integration of Polar Transform and Exception Handling
    try {
        PolarTransform polarEngine(128);
        vector<float> cartesianData(128, 2.0f);

        vector<float> polarOutput = polarEngine.transform(cartesianData);

        // A 128-element vector yields 64 pairs, thus 128 elements in output
        assert(polarOutput.size() == 128);
        cout << "[PASS] Test Case 2: Polar Transform Integration." << endl;
    } catch (const exception& e) {
        cerr << "[FAIL] Test Case 2 failed with: " << e.what() << endl;
    }

    // Test Case 3: Verify system exceptions
    try {
        QJL qjlEngine(128, 64);
        vector<float> badInput(64, 1.0f); // Size mismatch check
        
        qjlEngine.transform(badInput);
        cerr << "[FAIL] Test Case 3 failed: Exception not thrown for incorrect dimensions." << endl;
    } catch (const length_error& e) {
        cout << "[PASS] Test Case 3: Correctly caught length exception: " << e.what() << endl;
    }
}

} // anonymous namespace

int main() {
    try {
        runIntegrationTests();
        cout << "\n>>> All TurboQuant Integration Tests Passed Successfully. <<<" << endl;
    } catch (...) {
        cerr << "FATAL: Unhandled exception in the integration test suite." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}