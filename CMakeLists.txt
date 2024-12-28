cmake_minimum_required(VERSION 3.10)

# Project Name
project(DNSResolver)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Poco library (assuming it's installed or provided)
find_package(Poco REQUIRED Net)

# Add your main executable
add_executable(dns_resolver
    src/main.cpp
    src/DNSResolver.cpp
    src/DNSQuery.cpp
)

# Include Poco headers
target_include_directories(dns_resolver PRIVATE include)
target_link_libraries(dns_resolver PRIVATE Poco::Net)

# Enable testing
enable_testing()

# Add your test executable
add_executable(DNSResolverTest
    tests/test.cpp 
    src/DNSResolver.cpp
)

# Include the 'include' directory for the test target to find header files
target_include_directories(DNSResolverTest PRIVATE include)

# Link the test executable with Poco::Net (no CppUnit needed)
target_link_libraries(DNSResolverTest PRIVATE Poco::Net)

# Enable testing with CTest (CMake's testing tool)
add_test(NAME DNSResolverTest COMMAND DNSResolverTest)

# Optionally: Define the GoogleTest location if necessary
# You can specify the location of the CppUnit package or link it as a submodule

# Enable testing (optional but ensures CTest can find it)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Install dependencies (optional step depending on your project needs)
# include_directories(${GTEST_INCLUDE_DIRS})  # If CppUnit is not found globally
