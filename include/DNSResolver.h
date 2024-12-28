#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <Poco/Net/HostEntry.h>
#include <Poco/Net/DNS.h>
#include <Poco/Net/IPAddress.h>
#include <chrono>
#include <mutex>

class DNSResolver {
public:
    struct ResolverOptions {
        bool use_cache = true;      // Option to use cache
        bool recursive = false;     // Option to use recursive resolution
        int retries = 3;            // Number of retries in case of failure
        int timeout_seconds = 5;    // Timeout for DNS queries in seconds
    };

    struct CacheEntry {
        std::vector<std::string> ip_addresses;
        std::chrono::time_point<std::chrono::steady_clock> timestamp;
        int ttl;  // TTL (Time to live) in seconds
    };

    DNSResolver();

    std::vector<std::string> resolve(const std::string& domain, const ResolverOptions& options);
    void clearCache();  // Declare the clearCache function

private:
    std::unordered_map<std::string, CacheEntry> cache_;
    std::mutex cache_mutex_;  // Mutex for thread safety in cache access

    std::vector<std::string> queryDNS(const std::string& domain, bool recursive, int retries);
    std::vector<std::string> resolveFromCache(const std::string& domain);
    void cacheResult(const std::string& domain, const std::vector<std::string>& ip_addresses, int ttl);
    bool isCacheExpired(const std::string& domain);
    std::vector<std::string> performRecursiveQuery(const std::string& domain, int retries);
    std::vector<std::string> performNormalQuery(const std::string& domain);
    std::vector<std::string> performRootServerQuery(const std::string& domain);
    std::string convertToASCII(const std::string& domain);
};