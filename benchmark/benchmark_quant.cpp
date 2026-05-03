// give code for benmark_quant.cpp keep it modular , object oriented and use C++17 features and do execption handelling

#include <iostream>
#include <vector>
#include <chrono>
#include <memory>
#include <string>
#include <stdexcept>
#include <type_traits>

// Assuming these headers exist in your include path
#include "turboquant/core/qjl.hpp"
#include "turboquant/transforms/polar.hpp"

// Directive requested
using namespace std;

namespace turboquant::bench {

/**
 * @brief Base class for Quantization Benchmarks.
 * Uses RAII and provides a clean interface for different algorithms.
 */
class IQuantBenchmark {
public:
    virtual ~IQuantBenchmark() = default;
    virtual string getName() const = 0;
    virtual void run(size_t iterations) = 0;
};

/**
 * @brief Template-based runner to keep the benchmark modular.
 * @tparam Quantizer The quantization logic class to test.
 */
template <typename Quantizer>
class QuantizationBenchmark : public IQuantBenchmark {
public:
    explicit QuantizationBenchmark(string name, size_t dataSize) 
        : name_(move(name)), dataSize_(dataSize) {
        // Initialize sample data
        data_.resize(dataSize_, 1.5f); 
    }

    string getName() const override { return name_; }

    void run(size_t iterations) override {
        Quantizer quantizer;
        
        try {
            for (size_t i = 0; i < iterations; ++i) {
                // Ensure the work isn't optimized away by using the result
                auto result = quantizer.transform(data_);
                if (result.empty()) {
                    throw runtime_error("Quantization returned empty output.");
                }
            }
        } catch (const exception& e) {
            cerr << "[-] Error in " << name_ << ": " << e.what() << endl;
            throw; // Re-throw to be caught by the main runner
        }
    }

private:
    string name_;
    size_t dataSize_;
    vector<float> data_;
};

/**
 * @brief Orchestrator for running all registered benchmarks.
 */
class BenchmarkEngine {
public:
    void addBenchmark(unique_ptr<IQuantBenchmark> bench) {
        benchmarks_.push_back(move(bench));
    }

    void executeAll(size_t iterations) {
        cout << "--- Starting Benchmarks (" << iterations << " iterations) ---" << endl;
        
        for (const auto& bench : benchmarks_) {
            try {
                auto start = chrono::high_resolution_clock::now();
                
                bench->run(iterations);
                
                auto end = chrono::high_resolution_clock::now();
                chrono::duration<double, milli> elapsed = end - start;
                
                cout << "[PASS] " << bench->getName() << ": " 
                     << elapsed.count() << " ms (Avg: " 
                     << (elapsed.count() / iterations) << " ms/op)" << endl;
            } catch (...) {
                cout << "[FAIL] " << bench->getName() << " failed due to an exception." << endl;
            }
        }
    }

private:
    vector<unique_ptr<IQuantBenchmark>> benchmarks_;
};

} // namespace turboquant::bench

int main() {
    using namespace turboquant::bench;

    try {
        BenchmarkEngine engine;

        // Modular registration of different quantizers
        engine.addBenchmark(make_unique<QuantizationBenchmark<turboquant::core::QJL>>(
            "QJL_Quantizer_Large", 100'000));

        // Polar Transform
        engine.addBenchmark(make_unique<QuantizationBenchmark<turboquant::transforms::PolarTransform>>(
            "Polar_Transform_Standard", 50'000));

        // Run the suite
        engine.executeAll(100);

    } catch (const exception& e) {
        cerr << "FATAL: Benchmark engine crashed: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}