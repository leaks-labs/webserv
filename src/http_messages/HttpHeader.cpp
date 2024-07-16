#include "HttpHeader.hpp"

#include <algorithm>
#include <stdexcept>
#include <sstream>

#include "HttpRequest.hpp"

// TODO: remove
#include <iostream>
// TODO: remove

HttpHeader::HttpHeader()
    : is_complete_(false),
      need_body_(false)
{
}

HttpHeader::HttpHeader(const HttpHeader &src)
{
    *this = src;
}

HttpHeader& HttpHeader::operator=(const HttpHeader &rhs)
{
    if (this != &rhs) {
        is_complete_ = rhs.is_complete_;
        need_body_ = rhs.need_body_;
        header_map_ = rhs.header_map_;
        buffer_ = rhs.buffer_;
    }
    return *this;
}

HttpHeader::~HttpHeader()
{
}

const std::map<std::string, std::string>& HttpHeader::get_header_map() const
{
    return header_map_;
}

void    HttpHeader::set_host(const std::string& host)
{
    header_map_["HOST"] = host;
}

void HttpHeader::Parse(std::string& message)
{
    buffer_ = message;
    size_t pos = FindEndOfHeader(buffer_);
    if (pos == kNotFoundEnd) {
        message.clear();
        return;
    }

    buffer_.erase(pos - kTerminatorSize + 2);
    message.erase(0, pos);
    is_complete_ = true;

    std::vector<std::string> tokens;
    HttpRequest::Split(buffer_, "\r\n", tokens);
    buffer_.clear();

    for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
        std::pair<std::string, std::string> one_line = ParseOneLine(*it);
        
        if (one_line.first == "CONTENT-LENGTH") {
            if (one_line.second.find_first_not_of("0123456789") != std::string::npos)
                throw std::runtime_error("400");
        } else if (one_line.first == "TRANSFER-ENCODING") {
            if (one_line.second != "chunked" && one_line.second != "compress" && one_line.second != "deflate" && one_line.second != "gzip")
                throw std::runtime_error("400");
        } else if (one_line.first == "CONNECTION") {
            if (one_line.second != "keep-alive" && one_line.second != "close")
                throw std::runtime_error("400");
        }

        if (header_map_.find(one_line.first) != header_map_.end())
            throw std::runtime_error("400");
        header_map_[one_line.first] = one_line.second;
    }
    if (header_map_.find("HOST") == header_map_.end())
        throw std::runtime_error("400");
    bool    is_content_length = IsContentLength();
    if ((is_content_length && GetContentLength() > 0)
        || (!is_content_length && BodyIsTransferChunked()))
        need_body_ = true;
}

bool    HttpHeader::IsComplete() const
{
    return is_complete_;
}

bool    HttpHeader::NeedBody() const
{
    return need_body_;
}

bool    HttpHeader::IsContentLength() const
{
    return header_map_.find("CONTENT-LENGTH") != header_map_.end();
}

bool    HttpHeader::BodyIsTransferChunked() const
{
    std::map<std::string, std::string>::const_iterator it = header_map_.find("TRANSFER-ENCODING");
    return it == header_map_.end() ? false : it->second == "chunked";
}

size_t  HttpHeader::GetContentLength() const
{
    std::map<std::string, std::string>::const_iterator it = header_map_.find("CONTENT-LENGTH");
    std::istringstream  iss(it->second);
    size_t contentLength;
    iss >> std::noskipws >> contentLength;
    if (iss.fail() || !iss.eof())
        throw std::runtime_error("String to size_t conversion failed");
    // TODO: change error message
    return contentLength;
}

std::string HttpHeader::GetFormatedHeader() const
{
    std::string header;
    typedef std::map<std::string, std::string>::const_iterator iterator;
    for (iterator it = header_map_.begin(); it != header_map_.end(); ++it)
        header += it->first + ": " + it->second + "\r\n";
    header += "\r\n";
    return header;
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

size_t  HttpHeader::FindEndOfHeader(const std::string& buff)
{
    size_t  pos = buff.find("\r\n\r\n");
    return pos != std::string::npos ? pos + kTerminatorSize : kNotFoundEnd;
}

std::pair<std::string, std::string>  HttpHeader::ParseOneLine(const std::string& line)
{
    size_t  sep_pos = line.find(":");
    if (sep_pos == std::string::npos)
        throw std::runtime_error("400");
    std::string key = line.substr(0, sep_pos);
    std::string value = line.substr(sep_pos + 1);

    if (!value.empty() && value[0] == ' ')
        value.erase(0, 1);
    if (!value.empty() && value[value.size() - 1] == ' ')
        value.erase(value.size() - 1, 1);
    if (key.empty() || value.empty())
        throw std::runtime_error("400");
    std::transform(key.begin(), key.end(), key.begin(), ::toupper);
    return std::make_pair(key, value);
}
