#include "DNSQuery.h"
#include <Poco/Net/NetException.h>
#include <Poco/Net/IPAddress.h>
#include<bits/stdc++.h>

const std::vector<std::string> DNSQuery::ROOT_SERVERS = {
    "198.41.0.4",    // a.root-servers.net
    "199.9.14.201",  // b.root-servers.net
    "192.33.4.12"    // c.root-servers.net
};

DNSQuery::QueryResult DNSQuery::performQuery(const std::string& domain) {
    QueryResult result;
    result.success = false;

    try {
        std::cout << "Attempting to resolve domain: " << domain << std::endl;
        Poco::Net::HostEntry hostEntry = Poco::Net::DNS::resolve(domain);

        const auto& addresses = hostEntry.addresses();
        for (const auto& addr : addresses) {
            result.ip_addresses.push_back(addr.toString());
        }
        result.success = true;
        std::cout << "Successfully resolved domain: " << domain << std::endl;
    }
    catch (const Poco::Exception& e) {
        result.error_message = e.displayText();
        std::cerr << "Error resolving " << domain << ": " << e.displayText() << std::endl;
    }

    return result;
}

DNSQuery::QueryResult DNSQuery::performRecursiveQuery(const std::string& domain) {
    auto result = performQuery(domain);
    if (result.success) {
        return result;
    }

    for (const auto& root_server : ROOT_SERVERS) {
        try {
            std::cout << "Attempting to resolve domain using root server: " << root_server << std::endl;
            Poco::Net::HostEntry hostEntry = Poco::Net::DNS::resolve(domain);

            const auto& addresses = hostEntry.addresses();
            if (!addresses.empty()) {
                result.ip_addresses.clear();
                for (const auto& addr : addresses) {
                    result.ip_addresses.push_back(addr.toString());
                }
                result.success = true;
                std::cout << "Successfully resolved domain using root server: " << root_server << std::endl;
                return result;
            }
        }
        catch (const Poco::Exception& e) {
            std::cerr << "Error resolving using root server " << root_server << ": " << e.displayText() << std::endl;
            continue;
        }
    }

    result.success = false;
    result.error_message = "Unable to resolve domain recursively";
    return result;
}