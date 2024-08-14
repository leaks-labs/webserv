#include "HttpHeader.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <sstream>

#include "HttpCodeException.hpp"
#include "HttpRequest.hpp"

const std::map<std::string, HttpHeader::MemberFunctionPtr> HttpHeader::specific_header_handling_functions_ = HttpHeader::InitSpecificHeaderHandlingFuctions();

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

void    HttpHeader::Parse(std::string& message, int mode)
{
    std::vector<std::string> tokens;
    if (DivideIntoTokens(message, tokens, mode) == kHeaderNotParsed)
        return;
    HandleTokens(tokens, mode);
    AdditionalCheck(mode);
    need_body_ = (BodyIsTransferChunked() || (IsContentLength() && GetContentLength() > 0));
}

bool    HttpHeader::ParseTrailer(std::string& message, int mode)
{
    std::vector<std::string> tokens;
    if (DivideIntoTokens(message, tokens, mode) == kHeaderNotParsed)
        return kHeaderNotParsed;
    HandleTrailerTokens(tokens, mode);
    AdditionalTrailerCheck(mode);
    return kHeaderParsed;
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

bool    HttpHeader::IsTrailer() const
{
    return header_map_.find("TRAILER") != header_map_.end();
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

void    HttpHeader::DelOneHeader(const std::string& key)
{
    header_map_.erase(key);
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

const std::map<std::string, HttpHeader::MemberFunctionPtr>  HttpHeader::InitSpecificHeaderHandlingFuctions()
{
    std::map<std::string, MemberFunctionPtr>    functions;
    functions["CONTENT-LENGTH"] = &HttpHeader::HandleContentLength;
    functions["TRANSFER-ENCODING"] = &HttpHeader::HandleTransferEncoding;
    functions["CONNECTION"] = &HttpHeader::HandleConnection;
    functions["TRAILER"] = &HttpHeader::HandleTrailer;
    functions["HOST"] = &HttpHeader::HandleHost;
    return functions;
}

std::pair<std::string, std::string>  HttpHeader::ParseOneLine(const std::string& line, int mode)
{
    size_t  sep_pos = line.find(':');
    if (sep_pos == std::string::npos)
        throw HttpCodeExceptions::BadRequestException();
    std::string key = line.substr(0, sep_pos);
    std::string value = line.substr(sep_pos + 1);
    for (std::string::iterator it = key.begin(); mode == kParseRequest && it != key.end(); ++it) {
        if (std::isspace(*it) != 0)
            throw HttpCodeExceptions::BadRequestException();
    }
    if (mode == kParseResponse && !key.empty())
        key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
    value = TrimOWS(value);
    if (key.empty() || value.empty())
        mode == kParseRequest ? throw HttpCodeExceptions::BadRequestException() : throw HttpCodeExceptions::BadGatewayException();
    ToUpper(key);
    return std::make_pair(key, value);
}

void    HttpHeader::ToLower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void    HttpHeader::ToUpper(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

std::string HttpHeader::TrimOWS(const std::string& str) {
    if (str.empty())
        return str;
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, last - first + 1);
}

bool    HttpHeader::DivideIntoTokens(std::string& message, std::vector<std::string>& tokens, int mode)
{
    if (mode != kParseRequest && mode != kParseResponse)
        throw HttpCodeExceptions::InternalServerErrorException();
    size_t initial_buffer_size = buffer_.length();
    buffer_ += message;
    if (mode == kParseRequest && buffer_.size() > kMaxHeaderSize)
        throw HttpCodeExceptions::RequestHeaderFieldsTooLargeException();
    size_t pos = buffer_.find("\r\n\r\n");
    if (pos == std::string::npos) {
        message.clear();
        return kHeaderNotParsed;
    }
    buffer_.erase(pos);
    size_t  bytes_to_trim_in_message = buffer_.length() - initial_buffer_size + 4;
    message.erase(0, bytes_to_trim_in_message);
    is_complete_ = true;
    int err = HttpRequest::Split(buffer_, "\r\n", tokens);
    if (err == -1)
        mode == kParseRequest ? throw HttpCodeExceptions::BadRequestException() : throw HttpCodeExceptions::BadGatewayException();
    buffer_.clear();
    return kHeaderParsed;
}

void    HttpHeader::HandleTokens(std::vector<std::string>& tokens, int mode)
{
    for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
        if (mode == kParseRequest && it->find('\r') != std::string::npos)
            throw HttpCodeExceptions::BadRequestException();
        for (std::string::iterator char_it = it->begin(); mode == kParseResponse && char_it != it->end(); ++char_it) {
            if (*char_it == '\r')
                *char_it = ' ';
        }
        std::pair<std::string, std::string> one_line = ParseOneLine(*it, mode);
        if (header_map_.find(one_line.first) != header_map_.end())
            mode == kParseRequest ? throw HttpCodeExceptions::BadRequestException() : throw HttpCodeExceptions::BadGatewayException();

        std::map<std::string, MemberFunctionPtr>::const_iterator    func_it = specific_header_handling_functions_.find(one_line.first);
        if (func_it != specific_header_handling_functions_.end())
            (this->*(func_it->second))(one_line.second, mode);
        header_map_[one_line.first] = one_line.second;
    }
}

void    HttpHeader::HandleTrailerTokens(std::vector<std::string>& tokens, int mode)
{
    for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
        if (mode == kParseRequest && it->find('\r') != std::string::npos)
            throw HttpCodeExceptions::BadRequestException();
        for (std::string::iterator char_it = it->begin(); mode == kParseResponse && char_it != it->end(); ++char_it) {
            if (*char_it == '\r')
                *char_it = ' ';
        }
        std::pair<std::string, std::string> one_line = HttpHeader::ParseOneLine(*it, mode);
        
        std::map<std::string, std::string>::iterator    header_it = header_map_.find(one_line.first);
        if (header_it != header_map_.end()) {
            if (!header_it->second.empty())
                mode == kParseRequest ? throw HttpCodeExceptions::BadRequestException() : throw HttpCodeExceptions::BadGatewayException();
            
            std::map<std::string, MemberFunctionPtr>::const_iterator    func_it = specific_header_handling_functions_.find(one_line.first);
            if (func_it != specific_header_handling_functions_.end())
                (this->*(func_it->second))(one_line.second, mode);

            header_map_[one_line.first] = one_line.second;
        }
    }
}

void    HttpHeader::HandleContentLength(std::string& value, int mode)
{

    if (value.find_first_not_of("0123456789") != std::string::npos)
        mode == kParseRequest ? throw HttpCodeExceptions::BadRequestException() : throw HttpCodeExceptions::BadGatewayException();
}

void    HttpHeader::HandleTransferEncoding(std::string& value, int mode)
{

    ToLower(value);
    if (value != "chunked" && value != "compress" && value != "deflate" && value != "gzip")
        mode == kParseRequest ? throw HttpCodeExceptions::BadRequestException() : throw HttpCodeExceptions::BadGatewayException();
    else if (value != "chunked")
        throw HttpCodeExceptions::NotImplementedException();
}

void    HttpHeader::HandleConnection(std::string& value, int mode)
{
    ToLower(value);
    if (value != "keep-alive" && value != "close")
        mode == kParseRequest ? throw HttpCodeExceptions::BadRequestException() : throw HttpCodeExceptions::BadGatewayException();
}

void    HttpHeader::HandleTrailer(std::string& value, int mode)
{

    ToLower(value);
    value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
    value.erase(std::remove(value.begin(), value.end(), '\t'), value.end());
    std::vector<std::string>    trailer_fields;
    if (HttpRequest::Split(value, ",", trailer_fields) == -1)
        mode == kParseRequest ? throw HttpCodeExceptions::BadRequestException() : throw HttpCodeExceptions::BadGatewayException();
    for (std::vector<std::string>::iterator it = trailer_fields.begin(); it != trailer_fields.end(); ++it) {
        ToUpper(*it);
        if (header_map_.find(*it) != header_map_.end() || (*it == "TRANSFER-ENCODING" || *it == "CONTENT-LENGTH" || *it == "HOST" || *it == "TRAILER"))
            mode == kParseRequest ? throw HttpCodeExceptions::BadRequestException() : throw HttpCodeExceptions::BadGatewayException();
        header_map_[*it] = "";
    }
}

void    HttpHeader::HandleHost(std::string& value, int mode)
{
    (void)mode;
    ToLower(value);
}

void    HttpHeader::AdditionalCheck(int mode) const
{
    if (mode == kParseRequest && header_map_.find("HOST") == header_map_.end())
        throw HttpCodeExceptions::BadRequestException();
    if (header_map_.find("TRAILER") != header_map_.end() && header_map_.find("TRANSFER-ENCODING") == header_map_.end())
        mode == kParseRequest ? throw HttpCodeExceptions::BadRequestException() : throw HttpCodeExceptions::BadGatewayException();
}

void    HttpHeader::AdditionalTrailerCheck(int mode) const
{
    for (std::map<std::string, std::string>::const_iterator it = header_map_.begin(); it != header_map_.end(); ++it)
        if (it->second.empty())
            mode == kParseRequest ? throw HttpCodeExceptions::BadRequestException() : throw HttpCodeExceptions::BadGatewayException();
}
