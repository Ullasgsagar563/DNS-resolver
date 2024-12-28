#ifndef DNS_CACHE_H
#define DNS_CACHE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>

class DNSCache {
private:
    struct CacheEntry {
        std::vector<std::string> ip_addresses;
        std::chrono::system_clock::time_point expiry;
    };

    std::unordered_map<std::string, CacheEntry> cache_;
    mutable std::mutex cache_mutex_;

public:
    void addEntry(const std::string& domain,
                 const std::vector<std::string>& ip_addresses,
                 std::chrono::seconds ttl);

    bool getEntry(const std::string& domain,
                 std::vector<std::string>& ip_addresses);

    void cleanup();
};

#endif // DNS_CACHE_H