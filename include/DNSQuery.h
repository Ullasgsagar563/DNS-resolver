#pragma once
#include <string>
#include <vector>
#include <Poco/Net/DNS.h>
#include <Poco/Net/HostEntry.h>

class DNSQuery {
public:
    struct QueryResult {
        std::vector<std::string> ip_addresses;
        bool success;
        std::string error_message;
    };

    static QueryResult performQuery(const std::string& domain);
    static QueryResult performRecursiveQuery(const std::string& domain);

private:
    static const std::vector<std::string> ROOT_SERVERS;
};
