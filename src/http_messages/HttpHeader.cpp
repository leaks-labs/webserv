#include "HttpHeader.hpp"

HttpHeader::HttpHeader(const std::string& dictionary)
        : header_dictionary_(BuildDictionary(dictionary))
{

}

HttpHeader::~HttpHeader()
{
}

const std::map<std::string, std::vector<std::string> >&
        HttpHeader::BuildDictionary(const std::string& dictionary)
{
    //todo build dictionary based on a json
}

const std::map<std::string, std::string>& HttpHeader::get_header_map() const
{
    return header_map_;
}