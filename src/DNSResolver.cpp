#include "DNSResolver.h"
#include <Poco/Net/NetException.h>
#include <Poco/Net/DNS.h>
#include <iostream>
#include <thread>
#include <chrono>

DNSResolver::DNSResolver() {}

std::vector<std::string> DNSResolver::resolve(const std::string& domain, const ResolverOptions& options) {
    std::string ascii_domain = convertToASCII(domain);

    if (options.use_cache) {
        auto cached_result = resolveFromCache(ascii_domain);
        if (!cached_result.empty()) {
            return cached_result;
        }
    }

    std::vector<std::string> ip_addresses;
    if (options.recursive) {
        ip_addresses = performRecursiveQuery(ascii_domain, options.retries);
    } else {
        ip_addresses = performNormalQuery(ascii_domain);
    }

    if (!ip_addresses.empty() && options.use_cache) {
        cacheResult(ascii_domain, ip_addresses, 300);  // Cache with 300 seconds TTL
    }

    return ip_addresses;
}

std::vector<std::string> DNSResolver::queryDNS(const std::string& domain, bool recursive, int retries) {
    std::vector<std::string> result;
    while (retries > 0) {
        try {
            if (recursive) {
                result = performRecursiveQuery(domain, retries);
            } else {
                result = performNormalQuery(domain);
            }
            if (!result.empty()) break;
        } catch (const Poco::Net::NetException& e) {
            std::this_thread::sleep_for(std::chrono::seconds(1));  // Retry after 1 second
            retries--;
            if (retries == 0) {
                std::cerr << "Failed to resolve " << domain << ": " << e.displayText() << std::endl;
            }
        }
    }
    return result;
}

std::vector<std::string> DNSResolver::performNormalQuery(const std::string& domain) {
    std::vector<std::string> ip_addresses;
    try {
        Poco::Net::HostEntry hostEntry = Poco::Net::DNS::resolve(domain);
        const auto& addresses = hostEntry.addresses();
        for (const auto& addr : addresses) {
            ip_addresses.push_back(addr.toString());
        }
    } catch (const Poco::Net::NetException& e) {
        std::cerr << "Error resolving " << domain << ": " << e.displayText() << std::endl;
    }
    return ip_addresses;
}

std::vector<std::string> DNSResolver::performRecursiveQuery(const std::string& domain, int retries) {
    // Implement recursive query logic if required.
    // For now, we use the same logic as performNormalQuery for simplicity.
    return performRootServerQuery(domain);
}

std::vector<std::string> DNSResolver::performRootServerQuery(const std::string& domain) {
    std::vector<std::string> ip_addresses;
    try {
        // Use root DNS servers for the initial query
        Poco::Net::HostEntry hostEntry = Poco::Net::DNS::resolve(domain);
        const auto& addresses = hostEntry.addresses();
        for (const auto& addr : addresses) {
            ip_addresses.push_back(addr.toString());
        }
    } catch (const Poco::Net::NetException&) {
        // Handle root server failure and attempt retry or recursive behavior
    }
    return ip_addresses;
}

std::vector<std::string> DNSResolver::resolveFromCache(const std::string& domain) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    if (cache_.find(domain) != cache_.end()) {
        if (!isCacheExpired(domain)) {
            return cache_[domain].ip_addresses;
        }
    }
    return {};
}

void DNSResolver::cacheResult(const std::string& domain, const std::vector<std::string>& ip_addresses, int ttl) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    cache_[domain] = CacheEntry{ip_addresses, std::chrono::steady_clock::now(), ttl};
}

bool DNSResolver::isCacheExpired(const std::string& domain) {
    auto& entry = cache_[domain];
    auto duration = std::chrono::steady_clock::now() - entry.timestamp;
    return std::chrono::duration_cast<std::chrono::seconds>(duration).count() > entry.ttl;
}

void DNSResolver::clearCache() {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    cache_.clear();
}

std::string DNSResolver::convertToASCII(const std::string& domain) {
    return Poco::Net::DNS::isIDN(domain) ? Poco::Net::DNS::encodeIDN(domain) : domain;
}