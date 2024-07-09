#include "HttpRequest.hpp"

#include <sstream>
#include <iostream>

HttpRequest::HttpRequest()
{
}

HttpRequest::~HttpRequest()
{
}

size_t HttpRequest::FindRequest(const std::string &buff, size_t pos)
{
    return (buff.find("\r\n\r\n", pos));
}

void HttpRequest::Parse()
{
    try {
        std::vector<std::string> tokens;
        HttpMessage::Split(get_message(), "\r\n", tokens);
        if (tokens.empty())
            throw std::runtime_error("Bad Request");
        /*std::cout << "2\n";
        std::cout << tokens.at(0) << std::endl;
        std::cout << "Size :"<< tokens.size() << std::endl;*/
        get_protected_request_line().Parse(tokens.at(0));
        //std::cout << "8\n";
        for (size_t i = 1; i < tokens.size(); i++) {
            get_protected_header().Parse(tokens.at(i));
        }

        const std::map<std::string, std::string>& map = get_header().get_header_map();
        std::map<std::string, std::string>::const_iterator it = map.begin();
        while (it != map.end()) {
            //std::cout << it->first << "|" << it->second << std::endl;
            ++it;
        }
        //get_protected_body().SearchBody(*this);
//        bool body = SearchBody(*this);
//        std::cout << body << std::endl;
        get_protected_body().set_is_body(SearchBody(*this));// temporary fix
        //std::cout << get_protected_body().get_is_body() << std::endl;
    } catch (const std::exception &e) {
        std::string key = e.what();
        const std::string& value = HttpMessage::status_map.at(key);
        //std::cout << key << value << std::endl;
        set_status(std::make_pair(key, value));
    }
}

// temporary fix
bool HttpRequest::SearchBody(const HttpRequest& request)
{
    std::cout << "search body\n";
    int factor = 0;
    if (request.get_request_line().get_method().first == "POST" ||
        request.get_request_line().get_method().first == "PUT" ||
        request.get_request_line().get_method().first == "DELETE")
    {
        //std::cout << request.get_request_line().get_method().first << std::endl;
        factor++;
    }
    std::map<std::string, std::string>::const_iterator it1 = request.get_header().get_header_map().find("CONTENT-LENGTH");
    if (it1 != request.get_header().get_header_map().end() && it1->first == "CONTENT-LENGTH")
    {
        std::cout << "Content-Length\n";
        get_protected_body().set_transfer_type(*it1);
        factor++;
    }
    else
    {
        std::map<std::string, std::string>::const_iterator it2 = request.get_header().get_header_map().find("TRANSFER-ENCODING");
        if (it2 != request.get_header().get_header_map().end() && it2->first == "TRANSFER-ENCODING")
        {
            std::cout << "Transfer-Encoding\n";
            get_protected_body().set_transfer_type(*it2);
            factor++;
        }
    }
    std::cout << "End of search body\n";
    return factor == 2;
}