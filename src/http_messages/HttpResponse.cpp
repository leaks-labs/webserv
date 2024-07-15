#include "HttpResponse.hpp"

#include <algorithm>
#include <cstdio>

#include "CgiHandler.hpp"
#include "Directory.hpp"
#include "InitiationDispatcher.hpp"
#include "PathFinder.hpp"
#include "StreamHandler.hpp"

const std::vector<int>  HttpResponse::code_requiring_close_ = HttpResponse::InitCodeRequiringClose();

HttpResponse::HttpResponse(StreamHandler& stream_handler, const HttpRequest& request) : 
    stream_handler_(stream_handler),
    error_(request.get_status()),
    server_(request.get_server()),
    location_(request.get_location()),
    keep_alive_(request.KeepAlive()),
    method_(request.get_request_line().get_method()),
    target_(request.get_request_line().get_target()),
    request_header_(request.get_header()),
    request_body_(request.get_body().get_body()),
    path_(BuildPath()),
    cgi_path_(GetCgiPath(FindExtension(path_))),
    env_(SetEnv(request)),
    complete_(false)
{   
}

HttpResponse::HttpResponse(const HttpResponse& src)
    : stream_handler_(src.stream_handler_),
      error_(src.error_),
      server_(src.server_),
      location_(src.location_),
      keep_alive_(src.keep_alive_),
      method_(src.method_),
      target_(src.target_),
      request_header_(src.request_header_),
      request_body_(src.request_body_),
      path_(src.path_),
      status_line_(src.status_line_),
      header_(src.header_),
      body_(src.body_),
      cgi_path_(src.cgi_path_),
      env_(src.env_),
      complete_(src.complete_),
      response_(src.response_)
{
}

HttpResponse::~HttpResponse()
{
}

const Location& HttpResponse::get_location() const 
{
    return location_;
}

int HttpResponse::get_error() const 
{
    return error_;
}

std::string&    HttpResponse::get_response()
{
    return response_;
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
    return target_.get_query();
}

std::string&    HttpResponse::get_request_body()
{
    return request_body_;
}

const std::vector<std::string>& HttpResponse::get_env() const
{
    return env_;
}

void    HttpResponse::set_status_line(const std::string& str)
{
    status_line_ = str;
}

void    HttpResponse::set_body(const std::string& str)
{
    body_ = str;
}

void    HttpResponse::Execute()
{
    if (error_ == 200 && location_.HasMethod(method_) == false)
        error_ = 405;
    if (error_ != 200) {
        AddErrorPageToBody(error_);
        FinalizeResponse();
    }
    else if (method_ == "DELETE")
        Delete();
    else if (IsCgiFile(path_))
        LaunchCgiHandler();
    else
        Get();
}

bool HttpResponse::IsComplete() const
{
    return complete_;
}

void HttpResponse::SetComplete()
{
    response_ = status_line_ + "\r\n" + header_ + "\r\n" + body_;
    status_line_.clear();
    header_.clear();
    body_.clear();
    complete_ = true;
}

void HttpResponse::AddHeaderContentLength()
{
    std::ostringstream oss;
    oss << body_.size();
    std::string body_size_str = oss.str();
    header_ += "Content-Length: " + body_size_str + "\r\n";
}

void    HttpResponse::AppendToHeader(const std::string& str)
{
    header_ += str;
}

bool    HttpResponse::AskToCloseConnection() const
{
    return (std::find(code_requiring_close_.begin(), code_requiring_close_.end(), error_) != code_requiring_close_.end() || !keep_alive_);
}

std::vector<int> HttpResponse::InitCodeRequiringClose()
{
    std::vector<int>    res;
    res.push_back(400);
    res.push_back(408);
    res.push_back(413);
    res.push_back(414);
    res.push_back(431);
    res.push_back(500);
    return res;
}

std::string HttpResponse::BuildPath()
{
    if (target_.get_target()[target_.get_target().size() - 1] == '/' && location_.get_listing() == false)
        target_.set_target(target_.get_target() + location_.get_default_file());
    std::string res = PathFinder::CanonicalizePath(location_.get_root() + target_.get_target());
    if (!PathFinder::PathExist(res))
        error_ = 404;
    return res;
}

std::string HttpResponse::FindExtension(std::string const & str) const
{
    size_t pos = str.rfind('.');
    if (pos == std::string::npos)
        return "";
    return str.substr(pos + 1);
}

bool HttpResponse::IsCgiFile(std::string const & path) const
{
    std::string extension = FindExtension(path);
    return (extension == "php");
}

std::string HttpResponse::GetCgiPath(std::string const & ext) const
{
    PathFinder& finder = PathFinder::Instance();
    return ext == "php" ? finder.GetPhp() : "none";
}

void HttpResponse::Get()
{
    if (path_[path_.size() - 1] == '/')
        AddListingPageToBody();
    else
        AddFileToBody();
    FinalizeResponse();
}

void    HttpResponse::Delete()
{
    if (std::remove(path_.c_str()) != 0) {
        error_ = 404; // TODO: change to the right error
        AddErrorPageToBody(error_);
    }
    error_ = 204;
    FinalizeResponse();
}

void HttpResponse::AddFileToBody()
{
    std::ifstream ifs(path_.c_str());
    if (ifs.good()) {
        std::ostringstream buffer;
        buffer << ifs.rdbuf();
        body_ = buffer.str();
    } else {
        error_ = 404; // TODO: check if it's the right error
        AddErrorPageToBody(error_);
    }
}

void HttpResponse::AddListingPageToBody()
{
    Directory dir(path_, target_.get_target(), location_.get_root());
    if (dir.IsOpen()) {
        body_ =  dir.GetHTML();
    } else {
        error_ = 404; // TODO: check if it's the right error
        return AddErrorPageToBody(error_);
    }
}

void HttpResponse::AddErrorPageToBody(const int error)
{
    std::map<int, std::string>::const_iterator it = location_.get_errors().find(error);
    if (it == location_.get_errors().end()) {
        HTMLPage    error_page;
        body_ = error_page.GetErrorPage(error);
    } else {
        path_ = location_.get_errors().find(error)->second;
        std::ifstream ifs(path_.c_str());
        // TODO: check if a infinite loop is possible
        if(ifs.good())
            AddFileToBody();
        else {
            HTMLPage    error_page;
            body_ = error_page.GetErrorPage(error);
        }
    }
}

void    HttpResponse::CreateStatusLine()
{
    typedef std::map<int, std::string>::const_iterator iterator;

    std::ostringstream oss;
    oss << error_;
    std::string code_str = oss.str();

    iterator it = HttpRequest::status_map.find(error_);
    status_line_ = "HTTP/1.1 " + code_str + " " + it->second;
}

void HttpResponse::LaunchCgiHandler()
{
    new CgiHandler(stream_handler_, *this);
    // TODO: handle the new fail
}

void    HttpResponse::FinalizeResponse()
{
    CreateStatusLine();
    if (error_ != 204)
        AddHeaderContentLength();
    SetComplete();
    if (InitiationDispatcher::Instance().AddWriteFilter(stream_handler_) == -1)
        throw std::runtime_error("Failed to add write filter to InitiationDispatcher");
}

std::vector<std::string> HttpResponse::SetEnv(const HttpRequest& request)
{
    if (error_ != 200)
        return std::vector<std::string>();

    std::vector<std::string>    res;
    const std::map<std::string, std::string>&   map = request.get_header().get_header_map();
    res.push_back("REQUEST_METHOD=" + method_);
    res.push_back("SCRIPT_FILENAME=" + path_);
    std::map<std::string, std::string>::const_iterator it = map.find("CONTENT-TYPE");
    std::string content_type = it == map.end() ? "text/html" : it->second;
    it = map.find("CONTENT-LENGTH");
    std::string content_length = it == map.end() ? "0" : it->second;
    res.push_back("CONTENT_TYPE=" + content_type);
    res.push_back("CONTENT_LENGTH=" + content_length);
    res.push_back("REDIRECT_STATUS=200");
    return res;
}
