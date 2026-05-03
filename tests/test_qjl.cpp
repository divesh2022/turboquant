// test_qjl.cpp
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
    cout << "[+] Starting QJL Unit Tests..." << endl;

    // Test Case 1: Initialize and run standard transform
    {
        QJL qjl(1024, 128);
        assert(qjl.getInputDim() == 1024);
        assert(qjl.getTargetDim() == 128);

        vector<float> inputData(1024, 0.5f);
        auto output = qjl.transform(inputData);

        assert(output.size() == qjl.getTargetDim());
        for (auto value : output) {
            int iv = static_cast<int>(value);
            assert(iv >= -128 && iv <= 127);
        }

        cout << "[PASS] Test Case 1: Dimensionality and transformation output size." << endl;
    }

    // Test Case 2: Validate exception throwing on dimension mismatch
    {
        QJL qjl(1024, 128);
        vector<float> invalidInput(512, 1.0f);

        bool threw = false;
        try {
            qjl.transform(invalidInput);
        } catch (const length_error&) {
            threw = true;
        } catch (const invalid_argument&) {
            threw = true;
        }

        assert(threw);
        cout << "[PASS] Test Case 2: Correctly threw on invalid input length." << endl;
    }

    // Test Case 3: Test configuration exception on invalid bounds
    {
        bool threw = false;
        try {
            QJL qjl(128, 256);
        } catch (const invalid_argument&) {
            threw = true;
        }

        assert(threw);
        cout << "[PASS] Test Case 3: Correctly rejected invalid constructor dimensions." << endl;
    }

    // Test Case 4: Repeated identical input yields deterministic results
    {
        QJL qjl(256, 64);
        vector<float> inputData(256);
        for (size_t i = 0; i < inputData.size(); ++i) {
            inputData[i] = static_cast<float>(i) / 256.0f;
        }

        auto firstOutput = qjl.transform(inputData);
        auto secondOutput = qjl.transform(inputData);

        assert(firstOutput == secondOutput);
        cout << "[PASS] Test Case 4: Deterministic transform for repeated identical inputs." << endl;
    }
}

} // anonymous namespace

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    try {
        runUnitTests();
        cout << "\n>>> All QJL Tests Passed Successfully. <<<" << endl;
    } catch (const exception& e) {
        cerr << "\nFATAL: Unit tests failed with exception: " << e.what() << endl;
        return EXIT_FAILURE;
    } catch (...) {
        cerr << "\nFATAL: Unhandled exception in the QJL test suite." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
