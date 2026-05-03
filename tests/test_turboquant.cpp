
// tests/test_turboquant.cpp
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cassert>
#include <cstdlib>
#include <random>
#include "turboquant/core/qjl.hpp"

using namespace std;
using namespace turboquant::core;

namespace {

void runIntegrationTests() {
    cout << "[+] Starting TurboQuant Integration Tests..." << endl;

    // Test Case 1: Consistent output for repeated identical inputs
    {
        QJL qjl(512, 64);
        vector<float> inputData(512);
        for (size_t i = 0; i < inputData.size(); ++i) {
            inputData[i] = (i % 2 == 0) ? 0.0f : 1.0f;
        }

        auto outputA = qjl.transform(inputData);
        auto outputB = qjl.transform(inputData);

        assert(outputA.size() == 64);
        assert(outputB.size() == 64);
        assert(outputA == outputB);

        cout << "[PASS] Test Case 1: Repeated identical inputs produce consistent results." << endl;
    }

    // Test Case 2: Output retains valid quantized range for random data
    {
        QJL qjl(512, 64);
        vector<float> inputData(512);
        mt19937 rng(12345);
        uniform_real_distribution<float> dist(-1.0f, 1.0f);

        for (auto& value : inputData) {
            value = dist(rng);
        }

        auto output = qjl.transform(inputData);
        assert(output.size() == 64);

        for (auto value : output) {
            int iv = static_cast<int>(value);
            assert(iv >= -128 && iv <= 127);
        }

        cout << "[PASS] Test Case 2: Random input produces output in expected quantized range." << endl;
    }

    // Test Case 3: Integration-level invalid buffer length handling
    {
        QJL qjl(512, 64);
        vector<float> badInput(513, 0.0f);

        bool threw = false;
        try {
            qjl.transform(badInput);
        } catch (const length_error&) {
            threw = true;
        } catch (const invalid_argument&) {
            threw = true;
        }

        assert(threw);
        cout << "[PASS] Test Case 3: Integration rejects invalid input buffer length." << endl;
    }
}

} // anonymous namespace

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    try {
        runIntegrationTests();
        cout << "\n>>> All TurboQuant Integration Tests Passed Successfully. <<<" << endl;
    } catch (const exception& e) {
        cerr << "\nFATAL: Integration tests failed with exception: " << e.what() << endl;
        return EXIT_FAILURE;
    } catch (...) {
        cerr << "\nFATAL: Unhandled exception in the TurboQuant integration test suite." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}