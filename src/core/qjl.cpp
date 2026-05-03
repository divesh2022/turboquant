#include <iostream>
#include <vector>
#include "turboquant/core/qjl.hpp"

int main() {
    try {
        turboquant::core::QJL qjl(1024, 128);
        vector<float> data(1024, 0.5f);
        
        auto result = qjl.transform(data);
        std::cout << "Transformation successful! Output size: " << result.size() << std::endl;
    } catch (const exception& e) {
        std::cerr << e.what() << '\n';
    }
    return 0;
}