#include "HttpHeader.hpp"
#include "HttpMessage.hpp"

#include <stdexcept>
#include <algorithm>
#include <iostream>

const HeaderField HttpHeader::header_dictionary[N_HEADERS] = {
        {"CONTENT-LENGTH", {"1024"}, true},
        {"TRANSFER-ENCODING", {"chunked", "compress", "deflate", "gzip"}, true},
        {"CONNECTION", {"keep-alive"}, false},
        {"CONTENT-TYPE", {}, false},
        {"AUTHORIZATION", {}, false},
        {"USER-AGENT", {}, false},
        {"CACHE-CONTROL", {}, false},
        {"HOST", {}, false},
        {"ACCEPT", {}, false},
        {"ACCEPT-ENCODING", {}, false},
        {"ACCEPT-LANGUAGE", {}, false},
        {"COOKIE", {}, false}
};

HttpHeader::HttpHeader()
{
}

HttpHeader::~HttpHeader()
{
}

const std::map<std::string, std::string>& HttpHeader::get_header_map() const
{
    return header_map_;
}

void HttpHeader::Parse(const std::string &header)
{
    std::vector<std::string> headers, values;
    HttpMessage::Split(header, ": ", headers);
    if (headers.size() != 2)
        throw std::runtime_error("Bad Request");
    HttpMessage::Split(headers.at(1), ",", values);
    std::transform(headers.at(0).begin(), headers.at(0).end(), headers.at(0).begin(), ::toupper);
    size_t i = FindHeader(header_dictionary, headers.at(0));

    if (i == N_HEADERS)
        throw std::runtime_error("Bad Request");
    if (header_dictionary[i].is_used) {
        if (header_dictionary[i].name == "CONTENT-LENGTH") {
            if (values.at(0) != " 1024"/*header_dictionary[i].args[0]*/) //trim space in arg
                throw std::runtime_error("OK");
        }
        else if (header_dictionary[i].name == "TRANSFER-ENCODING") {
            if (values.at(0) != header_dictionary[i].args[0])
                throw std::runtime_error("Bad Request");
        }
    }
    header_map_.insert(std::make_pair(headers.at(0), headers.at(1)));
}

size_t HttpHeader::FindHeader(const HeaderField *header_field, const std::string &header)
{
    size_t i;
    for (i = 0; i < N_HEADERS; ++i) {
        if (header_field[i].name == header)
            break;
    }
    return i;
}

void HttpHeader::Print() const
{
    std::cout << "\tHttpHeader properties: " << std::endl;
    typedef std::map<std::string, std::string>::const_iterator iterator;
    if(header_map_.empty())
        std::cout << "\t\tthe header map is empty" << std::endl;
    for (iterator it = header_map_.begin(); it != header_map_.end(); it++)
        std::cout << "\t\t" << it->first << ": " << it->second << std::endl;
}