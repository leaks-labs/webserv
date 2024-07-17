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

void HttpHeader::Parse(std::string& message, int mode)
{
    if (mode != kParseRequest && mode != kParseResponse)
        throw std::runtime_error("500");
    size_t initial_buffer_size = buffer_.length();
    buffer_ += message;
    if (mode == kParseRequest && buffer_.size() > kMaxHeaderSize) // TODO: maybe add a limit for response too?
        throw std::runtime_error("431");
    size_t pos = buffer_.find("\r\n\r\n");
    if (pos == std::string::npos) {
        message.clear();
        return;
    }
    buffer_.erase(pos);
    size_t  bytes_to_trim_in_message = buffer_.length() - initial_buffer_size + 4;
    message.erase(0, bytes_to_trim_in_message);
    is_complete_ = true;
    std::vector<std::string> tokens;
    int err = HttpRequest::Split(buffer_, "\r\n", tokens);
    if (err == -1)
        mode == kParseRequest ? throw std::runtime_error("400") : throw std::runtime_error("502");
    buffer_.clear();

    for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
        std::pair<std::string, std::string> one_line = ParseOneLine(*it);
        
        if (one_line.first == "CONTENT-LENGTH") {
            if (one_line.second.find_first_not_of("0123456789") != std::string::npos)
                mode == kParseRequest ? throw std::runtime_error("400") : throw std::runtime_error("502");
        } else if (one_line.first == "TRANSFER-ENCODING") {
            if (one_line.second != "chunked" && one_line.second != "compress" && one_line.second != "deflate" && one_line.second != "gzip")
                mode == kParseRequest ? throw std::runtime_error("400") : throw std::runtime_error("502");
            else if (one_line.second != "chunked")
                throw std::runtime_error("501");
        } else if (one_line.first == "CONNECTION") {
            if (one_line.second != "keep-alive" && one_line.second != "close")
                mode == kParseRequest ? throw std::runtime_error("400") : throw std::runtime_error("502");
        }

        if (header_map_.find(one_line.first) != header_map_.end())
            mode == kParseRequest ? throw std::runtime_error("400") : throw std::runtime_error("502");
        header_map_[one_line.first] = one_line.second;
    }
    if (mode == kParseRequest && header_map_.find("HOST") == header_map_.end())
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

void    HttpHeader::AddOneHeader(const std::string& key, const std::string& value)
{
    header_map_[key] = value;
}

void    HttpHeader::Clear()
{
    is_complete_ = false;
    need_body_ = false;
    header_map_.clear();
    buffer_.clear();
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
