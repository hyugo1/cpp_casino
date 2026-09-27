# 1. Set the minimum version of CMake required
cmake_minimum_required(VERSION 3.15)

# 2. Define the project name and the programming language (CXX means C++)
project(MyProject CXX)

# 3. Specify the C++ standard (optional but recommended)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. Create an executable target (Output_Name Source_Files...)
add_executable(main main.cpp)


# cmake -S . -B build
# cmake --build build