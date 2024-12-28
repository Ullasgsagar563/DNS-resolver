# DNS Resolver

A C++ DNS resolver with caching and recursive query capabilities using the Poco C++ Libraries. This project includes extensive testing, performance optimization, and detailed documentation.

## Setup Instructions

### Prerequisites

- C++ compiler (e.g., `g++`)
- CMake
- Poco C++ Libraries (`libpoco-dev`)

### Installation

1. **Install Dependencies:**
    ```bash
    sudo apt-get update
    sudo apt-get install g++ cmake libpoco-dev
    ```

2. **Clone the Repository:**
    ```bash
    git clone <repository-url>
    cd <repository-directory>
    ```

3. **Build the Project:**
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

## Usage Guidelines

### Running the DNS Resolver

To run the DNS resolver, execute the `DNSResolverTest` binary:
```bash
./DNSResolverTest
```
To run resolver
```bash
./dns_resolver
```
