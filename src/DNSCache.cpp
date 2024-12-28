#include "DNSCache.h"

void DNSCache::addEntry(const std::string& domain,
                       const std::vector<std::string>& ip_addresses,
                       std::chrono::seconds ttl) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    CacheEntry entry{
        ip_addresses,
        std::chrono::system_clock::now() + ttl
    };
    cache_[domain] = entry;
}

bool DNSCache::getEntry(const std::string& domain,
                       std::vector<std::string>& ip_addresses) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto it = cache_.find(domain);
    if (it != cache_.end() && 
        it->second.expiry > std::chrono::system_clock::now()) {
        ip_addresses = it->second.ip_addresses;
        return true;
    }
    return false;
}

void DNSCache::cleanup() {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto now = std::chrono::system_clock::now();
    for (auto it = cache_.begin(); it != cache_.end();) {
        if (it->second.expiry <= now) {
            it = cache_.erase(it);
        } else {
            ++it;
        }
    }
}