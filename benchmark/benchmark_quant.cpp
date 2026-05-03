#include <iostream>
#include <vector>
#include <chrono>
#include <memory>
#include <string>
#include <stdexcept>

// TurboQuant headers
#include "turboquant/core/qjl.hpp"
#include "turboquant/transforms/polar.hpp"

using namespace std;

namespace turboquant::bench {

/**
 * @brief Base interface for all benchmarks.
 */
class IQuantBenchmark {
public:
    virtual ~IQuantBenchmark() = default;
    virtual string getName() const = 0;
    virtual void run(size_t iterations) = 0;
};

/**
 * @brief Template benchmark runner for quantizers.
 * @tparam Quantizer The quantization class to benchmark.
 */
template <typename Quantizer>
class QuantizationBenchmark : public IQuantBenchmark {
public:
    QuantizationBenchmark(string name, size_t inputDim, size_t targetDim)
        : name_(move(name)), inputDim_(inputDim), targetDim_(targetDim) {
        // Initialize sample data
        data_.resize(inputDim_, 1.5f);
    }

    string getName() const override { return name_; }

    void run(size_t iterations) override {
        try {
            Quantizer quantizer(inputDim_, targetDim_);
            auto start = chrono::high_resolution_clock::now();

            for (size_t i = 0; i < iterations; ++i) {
                auto result = quantizer.transform(data_);
                if (result.empty()) {
                    throw runtime_error("Quantization returned empty output.");
                }
            }

            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = end - start;
            cout << "[+] " << name_ << " completed " << iterations
                 << " iterations in " << elapsed.count() << " seconds." << endl;

        } catch (const exception& e) {
            cerr << "[-] Error in " << name_ << ": " << e.what() << endl;
            throw; // propagate to main
        }
    }

private:
    string name_;
    size_t inputDim_;
    size_t targetDim_;
    vector<float> data_;
};

} // namespace turboquant::bench

// ---------------- MAIN DRIVER ----------------
int main() {
    using namespace turboquant::bench;

    try {
        // Create benchmarks
        vector<unique_ptr<IQuantBenchmark>> benchmarks;
        benchmarks.push_back(make_unique<QuantizationBenchmark<turboquant::core::QJL>>(
            "QJL Benchmark", 128, 64));

        // Run all benchmarks
        for (auto& bench : benchmarks) {
            cout << "Running: " << bench->getName() << endl;
            bench->run(1000); // run 1000 iterations
        }

        cout << "[SUCCESS] All benchmarks completed." << endl;
        return 0;

    } catch (const exception& e) {
        cerr << "[FATAL] Benchmarking failed: " << e.what() << endl;
        return 1;
    }
}
