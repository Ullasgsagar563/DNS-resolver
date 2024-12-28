#include <cppunit/extensions/HelperMacros.h>
#include "DNSResolver.h"
#include <vector>
#include <string>
#include <iostream>

class DNSResolverTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(DNSResolverTest);
    CPPUNIT_TEST(testResolveWithCache);
    CPPUNIT_TEST(testResolveWithoutCache);
    CPPUNIT_TEST(testCacheExpiration);
    CPPUNIT_TEST(testEmptyCache);
    CPPUNIT_TEST(testCacheStaleAfterTTL);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override {
        // Initialize any necessary resources
    }

    void tearDown() override {
        // Clean up resources
    }

    void testResolveWithCache() {
        DNSResolver resolver;
        DNSResolver::ResolverOptions options;
        options.use_cache = true;

        // Assume "example.com" resolves to a specific IP address
        std::vector<std::string> result = resolver.resolve("example.com", options);

        CPPUNIT_ASSERT_EQUAL(size_t(1), result.size()); // Assuming it resolves to one IP
        CPPUNIT_ASSERT(result[0] == "93.184.216.34"); // Replace with actual resolved IP
    }

    void testResolveWithoutCache() {
        DNSResolver resolver;
        DNSResolver::ResolverOptions options;
        options.use_cache = false;

        std::vector<std::string> result = resolver.resolve("example.com", options);

        CPPUNIT_ASSERT_EQUAL(size_t(1), result.size());
        CPPUNIT_ASSERT(result[0] == "93.184.216.34"); // Replace with actual resolved IP
    }

    void testCacheExpiration() {
        DNSResolver resolver;
        DNSResolver::ResolverOptions options;
        options.use_cache = true;

        // First resolution
        resolver.resolve("example.com", options);

        // Simulate cache expiration (you can adjust the TTL for testing)
        // After TTL expires, the cache should be cleared
        // Here, you would add code to manipulate cache TTL for testing
        // Assuming the cache expires after a few seconds
        
        std::vector<std::string> result = resolver.resolve("example.com", options);

        CPPUNIT_ASSERT_EQUAL(size_t(1), result.size());
        CPPUNIT_ASSERT(result[0] == "93.184.216.34"); // Replace with actual resolved IP
    }

    void testEmptyCache() {
        DNSResolver resolver;
        DNSResolver::ResolverOptions options;
        options.use_cache = true;

        // Testing when the cache is empty
        std::vector<std::string> result = resolver.resolve("nonexistent-domain.com", options);
        CPPUNIT_ASSERT(result.empty()); // Should return empty because no resolution is available
    }

    void testCacheStaleAfterTTL() {
        DNSResolver resolver;
        DNSResolver::ResolverOptions options;
        options.use_cache = true;

        // Simulate a resolution with a TTL of 1 second
        std::vector<std::string> result = resolver.resolve("example.com", options);
        CPPUNIT_ASSERT_EQUAL(size_t(1), result.size());

        // Wait for the cache to expire (TTL is set to 1 second in this case)
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // Now the cache should be considered expired, and it should re-query
        std::vector<std::string> resultAfterTTL = resolver.resolve("example.com", options);
        CPPUNIT_ASSERT_EQUAL(size_t(1), resultAfterTTL.size());
        CPPUNIT_ASSERT(resultAfterTTL[0] == "93.184.216.34");
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(DNSResolverTest);
