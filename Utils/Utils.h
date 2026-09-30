#pragma once

#include <iostream>
#include <limits>

namespace Utils {
    template <typename T>
    bool tryRead(T& value) {
        if (!(std::cin >> value)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return false;
        }
        return true;
    }
}