#include <iostream>
#include <vector>
#include <stdexcept>
#include <cassert>
#include <cstdlib>
#include "turboquant/core/qjl.hpp"

using namespace std;
using namespace turboquant::core;

namespace {

void runUnitTests() {
    cout << "[+] Starting enhanced QJL Unit Tests..." << endl;

    // Test Case 1: Initialize and run standard transform
    try {
        QJL qjl(1024, 128);
        
        // Ensure that dimensions are correctly stored
        assert(qjl.getInputDim() == 1024);
        assert(qjl.getTargetDim() == 128);

        vector<float> inputData(1024, 0.5f);
        vector<int8_t> output = qjl.transform(inputData);

        assert(output.size() == qjl.getTargetDim());
        cout << "[PASS] Test Case 1: Dimensionality and Transformation." << endl;
    } catch (const exception& e) {
        cerr << "[FAIL] Test Case 1 failed with: " << e.what() << endl;
        throw;
    }

    // Test Case 2: Validate exception throwing on dimension mismatch
    try {
        QJL qjl(1024, 128);
        vector<float> invalidInput(512, 1.0f); // Size mismatch

        // This should throw an exception
        qjl.transform(invalidInput);
        
        // If we reach here, the test failed
        cerr << "[FAIL] Test Case 2 failed: Exception not thrown for dimension mismatch." << endl;
        exit(EXIT_FAILURE);
    } catch (const length_error& e) {
        cout << "[PASS] Test Case 2: Correctly caught expected length_error: " << e.what() << endl;
    } catch (...) {
        cerr << "[FAIL] Test Case 2 failed: Caught unexpected exception type." << endl;
        exit(EXIT_FAILURE);
    }

    // Test Case 3: Test configuration exception on invalid bounds
    try {
        // Target dimension exceeds input dimension
        QJL qjl(128, 256);
        cerr << "[FAIL] Test Case 3 failed: Constructor did not throw on invalid dimensions." << endl;
        exit(EXIT_FAILURE);
    } catch (const invalid_argument& e) {
        cout << "[PASS] Test Case 3: Correctly caught expected invalid_argument: " << e.what() << endl;
    }
}

} // anonymous namespace

int main() {
    // Set proper terminal output handling
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    try {
        runUnitTests();
        cout << "\n>>> All QJL Tests Passed Successfully. <<<" << endl;
    } catch (const exception& e) {
        cerr << "\nFATAL: Unit tests failed with exception: " << e.what() << endl;
        return EXIT_FAILURE;
    } catch (...) {
        cerr << "\nFATAL: Unhandled exception in the test suite." << endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}