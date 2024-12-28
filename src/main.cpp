#include <bits/stdc++.h>
#include "DNSResolver.h"

// ANSI color codes for terminal output
namespace Color {
    const std::string GREEN = "\033[32m";
    const std::string RED = "\033[31m";
    const std::string YELLOW = "\033[33m";
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
}

// Test result structure
struct TestResult {
    std::string name;
    bool passed;
    std::string error_message;
    double duration;
};

class TestRunner {
private:
    std::vector<TestResult> results;
    std::chrono::steady_clock::time_point start_time;

public:
    void startTesting() {
        start_time = std::chrono::steady_clock::now();
        std::cout << Color::BOLD << "\n=================== DNS Resolver Test Suite ===================" 
                  << Color::RESET << "\n" << std::endl;
    }

    void runTest(const std::string& testName, std::function<void()> testFunction) {
        auto test_start = std::chrono::steady_clock::now();
        TestResult result{testName, true, "", 0.0};
        
        try {
            testFunction();
        } catch (const std::exception& e) {
            result.passed = false;
            result.error_message = e.what();
        }

        auto test_end = std::chrono::steady_clock::now();
        result.duration = std::chrono::duration<double>(test_end - test_start).count();
        
        // Print immediate result
        std::cout << (result.passed ? Color::GREEN : Color::RED)
                  << "[" << (result.passed ? "PASSED" : "FAILED") << "] " 
                  << Color::RESET << testName 
                  << " (" << std::fixed << std::setprecision(3) << result.duration << "s)";
        
        if (!result.passed) {
            std::cout << "\n" << Color::RED << "    Error: " << result.error_message << Color::RESET;
        }
        std::cout << std::endl;
        
        results.push_back(result);
    }

    void printSummary() {
        auto end_time = std::chrono::steady_clock::now();
        double total_duration = std::chrono::duration<double>(end_time - start_time).count();
        
        int passed = std::count_if(results.begin(), results.end(), 
                                 [](const TestResult& r) { return r.passed; });
        int failed = results.size() - passed;

        std::cout << "\n=================== Test Summary ===================" << std::endl;
        
        // Print failed tests summary if any
        if (failed > 0) {
            std::cout << Color::RED << "\nFailed Tests:" << Color::RESET << std::endl;
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << "  " << result.name << ": " << result.error_message << std::endl;
                }
            }
        }

        // Print statistics
        std::cout << "\nTest Statistics:" << std::endl;
        std::cout << "  Total Tests: " << results.size() << std::endl;
        std::cout << Color::GREEN << "  Passed: " << passed << Color::RESET << std::endl;
        if (failed > 0) {
            std::cout << Color::RED << "  Failed: " << failed << Color::RESET << std::endl;
        } else {
            std::cout << "  Failed: " << failed << std::endl;
        }
        std::cout << "  Total Time: " << std::fixed << std::setprecision(2) 
                  << total_duration << "s" << std::endl;

        // Print final status
        std::cout << "\n" << (failed == 0 ? Color::GREEN : Color::RED)
                  << "================= " 
                  << (failed == 0 ? "All Tests Passed" : "Some Tests Failed")
                  << " =================" 
                  << Color::RESET << std::endl;
    }
};

// Test functions
void testBasicResolution() {
    DNSResolver resolver;
    std::string domain = "github.com";
    DNSResolver::ResolverOptions options;
    auto result = resolver.resolve(domain, options);

    if (result.empty()) {
        throw std::runtime_error("Resolution failed for github.com");
    }

    // Verify that at least one valid IP address is returned
    bool hasValidIP = false;
    for (const auto& ip : result) {
        if (!ip.empty()) {
            hasValidIP = true;
            break;
        }
    }
    if (!hasValidIP) {
        throw std::runtime_error("No valid IP addresses returned for github.com");
    }
}

void testRecursiveResolution() {
    DNSResolver resolver;
    std::string domain = "github.com";
    DNSResolver::ResolverOptions options;
    options.recursive = true;
    auto result = resolver.resolve(domain, options);

    if (result.empty()) {
        throw std::runtime_error("Recursive resolution failed for github.com");
    }
}

void testCaching() {
    DNSResolver resolver;
    std::string domain = "github.com";
    DNSResolver::ResolverOptions options;
    options.use_cache = true;

    // Clear any existing cache
    resolver.clearCache();

    // First resolution (should query DNS)
    auto start_uncached = std::chrono::high_resolution_clock::now();
    auto result1 = resolver.resolve(domain, options);
    auto duration_uncached = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - start_uncached);

    if (result1.empty()) {
        throw std::runtime_error("First resolution failed for " + domain);
    }

    // Small delay to ensure timing difference
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Second resolution (should use cache)
    auto start_cached = std::chrono::high_resolution_clock::now();
    auto result2 = resolver.resolve(domain, options);
    auto duration_cached = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - start_cached);

    if (result2.empty()) {
        throw std::runtime_error("Cached resolution failed for " + domain);
    }

    // Check if results match
    if (result1 != result2) {
        throw std::runtime_error("Cached result does not match uncached result for " + domain);
    }

    // Verify cached resolution is faster
    if (duration_cached >= duration_uncached) {
        throw std::runtime_error("Cached resolution is not faster than uncached resolution");
    }

    std::cout << "Uncached resolution took: " << duration_uncached.count() << " µs" << std::endl;
    std::cout << "Cached resolution took: " << duration_cached.count() << " µs" << std::endl;
}

void testNonExistentDomain() {
    DNSResolver resolver;
    std::string domain = "thisdomaindoesnotexist123.com";
    DNSResolver::ResolverOptions options;
    auto result = resolver.resolve(domain, options);

    if (!result.empty()) {
        throw std::runtime_error("Resolution should have failed for non-existent domain");
    }
}

void testTimeout() {
    DNSResolver resolver;
    std::string domain = "google.com";
    DNSResolver::ResolverOptions options;
    options.timeout_seconds = 1;  // Very short timeout

    auto start_time = std::chrono::steady_clock::now();
    auto result = resolver.resolve(domain, options);
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - start_time).count();

    if (duration > options.timeout_seconds + 1) {
        throw std::runtime_error("Resolver did not respect timeout setting");
    }
}

void testMultipleRecords() {
    DNSResolver resolver;
    std::string domain = "example.com";
    DNSResolver::ResolverOptions options;
    auto result = resolver.resolve(domain, options);

    if (result.empty()) {
        throw std::runtime_error("Resolution failed for example.com");
    }

    if (result.size() < 2) {
        throw std::runtime_error("Expected multiple IP addresses for example.com");
    }
}

void testInvalidDomainFormat() {
    DNSResolver resolver;
    std::string domain = "invalid_domain";
    DNSResolver::ResolverOptions options;
    auto result = resolver.resolve(domain, options);

    if (!result.empty()) {
        throw std::runtime_error("Resolution should have failed for invalid domain format");
    }
}

void testLargeDomainResolution() {
    DNSResolver resolver;
    std::vector<std::string> domains = {
        "google.com", "yahoo.com", "example.com", 
        "github.com", "facebook.com"
    };

    std::vector<std::string> failedDomains;

    for (const auto& domain : domains) {
        DNSResolver::ResolverOptions options;
        auto result = resolver.resolve(domain, options);

        if (result.empty()) {
            failedDomains.push_back(domain);
        }
    }

    if (!failedDomains.empty()) {
        std::string errorMsg = "Resolution failed for domains: ";
        for (const auto& domain : failedDomains) {
            errorMsg += domain + " ";
        }
        throw std::runtime_error(errorMsg);
    }
}

void testIDNResolution() {
    DNSResolver resolver;
    std::string domain = "xn--ls8h.xn--zfr164b"; // Example IDN domain
    DNSResolver::ResolverOptions options;
    auto result = resolver.resolve(domain, options);

    if (result.empty()) {
        throw std::runtime_error("Resolution failed for internationalized domain");
    }
}

void testAnotherIDNResolution() {
    DNSResolver resolver;
    std::string domain = "xn--d1acufc.xn--p1ai.com"; // Another IDN domain
    DNSResolver::ResolverOptions options;
    auto result = resolver.resolve(domain, options);

    if (result.empty()) {
        throw std::runtime_error("Resolution failed for another internationalized domain");
    }
}

int main() {
    TestRunner runner;
    runner.startTesting();

    // Run all tests
    runner.runTest("Basic Resolution", testBasicResolution);
    runner.runTest("Recursive Resolution", testRecursiveResolution);
    runner.runTest("Caching", testCaching);
    runner.runTest("Non-Existent Domain", testNonExistentDomain);
    runner.runTest("Timeout", testTimeout);
    runner.runTest("Multiple Records", testMultipleRecords);
    runner.runTest("Invalid Domain Format", testInvalidDomainFormat);
    runner.runTest("Large Domain Resolution", testLargeDomainResolution);
    runner.runTest("IDN Resolution", testIDNResolution);
    runner.runTest("Another IDN Resolution", testAnotherIDNResolution);
    // Print final summary
    runner.printSummary();

    return 0;
}