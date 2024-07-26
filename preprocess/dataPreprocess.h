#include <iostream>
#include <vector>
#include <algorithm>

template <typename T>
std::vector<T> calculatePreprocessAbsError(const std::vector<T>& data, T isovalue){
    std::vector<T> errorVector;
    for (int i = 0; i < data.size(); ++i) {
        T error = std::abs(data[i] - isoValue);
        errorVector.push_back(error);
    }
    std::sort(errorVector.begin(), errorVector.end());

    return errorVector;
}
