#include "HttpResponse.hpp"

#include <algorithm>
#include <cstdio>

#include <sys/stat.h>
#include <unistd.h>

#include "CgiHandler.hpp"
#include "Directory.hpp"
#include "InitiationDispatcher.hpp"
#include "PathFinder.hpp"
#include "ProxyHandler.hpp"
#include "StreamHandler.hpp"

HttpResponse::HttpResponse(StreamHandler& stream_handler, HttpRequest& request)
    : complete_(false),
      request_(request),
      stream_handler_(stream_handler),
      status_line_(request.get_status_code()),
      keep_alive_(request.KeepAlive())
{
    if (status_line_.get_status_code() == 200 && request.get_location().get_proxy().empty())
        path_ = BuildPath();
    if (status_line_.get_status_code() == 200 && IsCgiFile(path_)) {
        cgi_path_ = GetCgiPath(FindExtension(path_));
        env_ = SetEnv();
    }
}

HttpResponse::HttpResponse(const HttpResponse& src)
    : complete_(src.complete_),
      request_(src.request_),
      stream_handler_(src.stream_handler_),
      status_line_(src.status_line_),
      keep_alive_(src.keep_alive_),
      path_(src.path_),
      cgi_path_(src.cgi_path_),
      env_(src.env_),
      header_(src.header_),
      body_(src.body_),
      response_(src.response_)
{
}

HttpResponse::~HttpResponse()
{
}

const std::string&  HttpResponse::get_cgi_path() const
{
    return cgi_path_;
}

const std::string&  HttpResponse::get_path() const
{
    return path_;
}

const std::string&  HttpResponse::get_query() const
{
    return request_.get_request_line().get_target().get_query();
}

const std::vector<std::string>& HttpResponse::get_env() const
{
    return env_;
}

std::string&    HttpResponse::get_request_body()
{
    return request_.get_body();
}

std::string     HttpResponse::get_complete_request() const
{
    return request_.GetCompleteRequest();
}

std::string&    HttpResponse::get_response()
{
    return response_;
}

const HttpHeader&   HttpResponse::get_header()
{
    return header_;
}

HttpBody&   HttpResponse::get_body()
{
    return body_;
}

void    HttpResponse::set_status_line(int code)
{
    status_line_.SetCodeAndPhrase(code);
}

void    HttpResponse::set_body(const std::string& str)
{
    body_.set_body(str);
}

void    HttpResponse::set_request_host(const std::string& host)
{
    request_.set_host(host);
}

void    HttpResponse::Execute()
{
    if (status_line_.get_status_code() == 200 && request_.get_location().HasMethod(request_.get_request_line().get_method()) == false)
        set_status_line(405);
    if (status_line_.get_status_code() != 200) {
        AddErrorPageToBody(status_line_.get_status_code());
    } else if (!request_.get_location().get_proxy().empty()) {
        LaunchProxyHandler();
        return;
    } else if (request_.get_request_line().get_method() == "DELETE") {
        Delete();
    } else if (IsCgiFile(path_)) {
        LaunchCgiHandler();
        return;
    } else {
        Get();
    }
    FinalizeResponse();
    if (InitiationDispatcher::Instance().AddWriteFilter(stream_handler_) == -1)
        throw std::runtime_error("Failed to add write filter to InitiationDispatcher");
}

void    HttpResponse::ParseStatusLine(std::string& str)
{
    status_line_.Parse(str);
}

bool    HttpResponse::StatusLineIsComplete() const
{
    return status_line_.IsComplete();
}

void    HttpResponse::ParseHeader(std::string& str)
{
    header_.Parse(str, HttpHeader::kParseResponse);
}

bool    HttpResponse::HeaderIsComplete() const
{
    return header_.IsComplete();
}

void    HttpResponse::ParseBody(std::string& str)
{
    body_.Parse(str);
}

bool    HttpResponse::BodyIsComplete() const
{
    return body_.IsComplete();
}

bool HttpResponse::IsComplete() const
{
    return complete_;
}

void HttpResponse::SetComplete()
{
    response_ = status_line_.GetFormatedStatusLine() + header_.GetFormatedHeader() + body_.get_body();
    status_line_.Clear();
    header_.Clear();
    body_.Clear();
    complete_ = true;
}

void HttpResponse::AddHeaderContentLength()
{
    if (status_line_.get_status_code() == 204)
        return;
    std::ostringstream oss;
    oss << body_.Size();
    std::string body_size_str = oss.str();
    header_.AddOneHeader("CONTENT-LENGTH", body_size_str);
}

bool    HttpResponse::IsAskingToCloseConnection() const
{
    const std::vector<int>& vec = status_line_.get_codes_requiring_close();
    return (std::find(vec.begin(), vec.end(), status_line_.get_status_code()) != vec.end() || !keep_alive_);
}

void    HttpResponse::ClearStatusLine()
{
    status_line_.Clear();
}

void    HttpResponse::ClearHeader()
{
    header_.Clear();
}

void    HttpResponse::ClearBody()
{
    body_.Clear();
}

void    HttpResponse::UpdateReason()
{
    set_status_line(status_line_.get_status_code());
}

void    HttpResponse::SetResponseToErrorPage(const int error)
{
    set_status_line(error);
    AddErrorPageToBody(error);
    ClearHeader();
    FinalizeResponse();
}

std::string HttpResponse::FindExtension(const std::string& str)
{
    size_t pos = str.rfind('.');
    return (pos == std::string::npos) ? "" : str.substr(pos + 1);
}

bool HttpResponse::IsCgiFile(const std::string& path)
{
    std::string extension = FindExtension(path);
    return (extension == "php");
}

std::string HttpResponse::GetCgiPath(const std::string& ext)
{
    PathFinder& finder = PathFinder::Instance();
    return ext == "php" ? finder.GetPhp() : "none";
}

std::string HttpResponse::BuildPath()
{
    HttpRequestLine::Target&    target_ = request_.get_request_line().get_target();
    const Location&             location_ = request_.get_location();
    if (target_.get_target()[target_.get_target().size() - 1] == '/' && location_.get_listing() == false)
        target_.set_target(target_.get_target() + location_.get_default_file());
    std::string res = PathFinder::CanonicalizePath(location_.get_root() + target_.get_target());
    if (!PathFinder::PathExist(res))
        set_status_line(404);
    return res;
}

void HttpResponse::Get()
{
    if (path_[path_.size() - 1] == '/')
        AddListingPageToBody();
    else
        AddFileToBody();
}

void    HttpResponse::Delete()
{
    if (!HasRightToModify(path_)) {
        set_status_line(403);
        AddErrorPageToBody(status_line_.get_status_code());
    } else if (std::remove(path_.c_str()) != 0) {
        set_status_line(500);
        AddErrorPageToBody(status_line_.get_status_code());
    } else {
        set_status_line(204);
    }
}

void HttpResponse::AddFileToBody()
{
    struct stat buffer;
    if ((stat(path_.c_str(), &buffer) == 0 && !S_ISREG(buffer.st_mode)) || access(path_.c_str(), R_OK) == -1) {
        set_status_line(403);
        AddErrorPageToBody(status_line_.get_status_code());
        return;
    }
    std::ifstream ifs(path_.c_str());
    if (ifs.good()) {
        std::ostringstream buffer;
        buffer << ifs.rdbuf();
        body_.set_body(buffer.str());
    } else {
        set_status_line(500);
        AddErrorPageToBody(status_line_.get_status_code());
    }
}

void HttpResponse::AddListingPageToBody()
{
    Directory dir(path_, request_.get_request_line().get_target().get_target(), request_.get_location().get_root());
    if (dir.IsOpen()) {
        body_.set_body(dir.GetHTML());
    } else {
        set_status_line(404);
        return AddErrorPageToBody(status_line_.get_status_code());
    }
}

void HttpResponse::AddErrorPageToBody(const int error)
{
    const std::map<int, std::string>&   errors_files = request_.get_location().get_errors();
    std::map<int, std::string>::const_iterator it = errors_files.find(error);
    if (it == errors_files.end()) {
        HTMLPage    error_page;
        body_.set_body(error_page.GetErrorPage(error));
    } else {
        path_ = errors_files.find(error)->second;
        std::ifstream ifs(path_.c_str());
        // TODO: check if a infinite loop is possible
        if(ifs.good())
            AddFileToBody();
        else {
            HTMLPage    error_page;
            body_.set_body(error_page.GetErrorPage(error));
        }
    }
}

void HttpResponse::LaunchCgiHandler()
{
    try
    {
        new CgiHandler(stream_handler_, *this);
    }
    catch(const std::exception& e)
    {
        std::istringstream iss(e.what());
        int code;
        iss >> std::noskipws >> code;
        if (iss.fail() || !iss.eof() || code < 100 || code > 599)
            code = 500;
        SetResponseToErrorPage(code);
        if (InitiationDispatcher::Instance().AddWriteFilter(stream_handler_) == -1)
            throw std::runtime_error("Failed to add write filter to InitiationDispatcher");
    }
}

void    HttpResponse::LaunchProxyHandler()
{
    struct addrinfo* addr = NULL;
    try
    {
        std::string host = request_.get_location().get_proxy();
        size_t  pos = host.find(':');
        std::string host_without_port = host.substr(0, pos);
        addr = ProxyHandler::ConvertToAddrInfo(host_without_port);
        new ProxyHandler(stream_handler_, *addr, host_without_port, *this);
        freeaddrinfo(addr);
    }
    catch(const std::exception& e)
    {
        if (addr != NULL)
            freeaddrinfo(addr);
        std::istringstream iss(e.what());
        int code;
        iss >> std::noskipws >> code;
        if (iss.fail() || !iss.eof() || code < 100 || code > 599)
            code = 500;
        SetResponseToErrorPage(code);
        if (InitiationDispatcher::Instance().AddWriteFilter(stream_handler_) == -1)
            throw std::runtime_error("Failed to add write filter to InitiationDispatcher");
    }
}

void    HttpResponse::FinalizeResponse()
{
    UpdateReason();
    AddHeaderContentLength();
    SetComplete();
}

std::vector<std::string> HttpResponse::SetEnv()
{
    if (status_line_.get_status_code() != 200)
        return std::vector<std::string>();

    std::vector<std::string>    res;
    const std::map<std::string, std::string>&   map = request_.get_header().get_header_map();
    res.push_back("REQUEST_METHOD=" + request_.get_request_line().get_method());
    std::string path_info = request_.get_location().get_path_info();
    if (path_info != "none")
        res.push_back("PATH_INFO=" + path_info);
    res.push_back("SCRIPT_FILENAME=" + path_);
    std::map<std::string, std::string>::const_iterator it = map.find("CONTENT-TYPE");
    std::string content_type = it == map.end() ? "text/html" : it->second;
    it = map.find("CONTENT-LENGTH");
    std::string content_length = it == map.end() ? "0" : it->second;
    res.push_back("CONTENT_TYPE=" + content_type);
    res.push_back("CONTENT_LENGTH=" + content_length);
    if(!get_query().empty())
        res.push_back("QUERY_STRING=" + get_query());
    res.push_back("REDIRECT_STATUS=200");
    return res;
}

bool    HttpResponse::HasRightToModify(const std::string& path)
{
    std::string parent_dir = path.substr(0, path.find_last_of('/'));
    if (parent_dir.empty())
        parent_dir = "/";
    struct stat statbuf;
    return (access(path.c_str(), W_OK) == 0 && stat(parent_dir.c_str(), &statbuf) == 0 && access(parent_dir.c_str(), W_OK) == 0);
}
