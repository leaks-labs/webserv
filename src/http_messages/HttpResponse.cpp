#include "HttpResponse.hpp"

#include <algorithm>
#include <cstdio>

#include <sys/stat.h>
#include <unistd.h>

#include "CgiHandler.hpp"
#include "Directory.hpp"
#include "HttpCodeException.hpp"
#include "InitiationDispatcher.hpp"
#include "PathFinder.hpp"
#include "StreamHandler.hpp"

HttpResponse::HttpResponse(StreamHandler& stream_handler, HttpRequest& request)
    : complete_(false),
      request_(request),
      stream_handler_(stream_handler),
      server_(&request_.get_server()),
      location_(&request_.get_location()),
      status_line_(request.get_status_code()),
      keep_alive_(request.KeepAlive()),
      target_(request_.get_request_line().get_target()),
      path_(BuildPath()),
      redirect_count_(0)
{
    if (status_line_.get_status_code() != 200) {
        target_.ClearTarget();
        path_.clear();
        std::string tmp_request_path = ErrorFileIsSet();
        if (!tmp_request_path.empty())
            UpdatePathAndTarget(tmp_request_path);
    }
}

HttpResponse::HttpResponse(const HttpResponse& src)
    : complete_(src.complete_),
      request_(src.request_),
      stream_handler_(src.stream_handler_),
      server_(src.server_),
      location_(src.location_),
      status_line_(src.status_line_),
      keep_alive_(src.keep_alive_),
      target_(src.target_),
      path_(src.path_),
      cgi_path_(src.cgi_path_),
      env_(src.env_),
      header_(src.header_),
      body_(src.body_),
      response_(src.response_),
      redirect_count_(src.redirect_count_)
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

std::string&    HttpResponse::get_request_body_buffer()
{
    return request_.get_body();
}

std::string&    HttpResponse::get_response_buffer()
{
    return response_;
}

const HttpHeader& HttpResponse::get_header() const
{
    return header_;
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
    if (target_.get_target().empty())
        return ApplyGeneratedPage();
    location_ = &server_->FindLocation(target_.get_target());
    if (!request_.get_location().HasMethod(request_.get_request_line().get_method())) {
        return RedirectToNewTarget(405);
    } else if (location_->get_redirect() != "false") {
        return MovedPermanentely(location_->get_redirect());
    } else if (!PathFinder::PathExist(path_)) {
        return RedirectToNewTarget(404);
    } else if (target_.get_target()[target_.get_target().size() - 1] != '/' && IsDir(path_)) {
        return MovedPermanentely(target_.get_target() + "/");
    } else if (target_.get_target()[target_.get_target().size() - 1] == '/') {
        std::string index_file_path = target_.get_target() + location_->get_default_file();
        std::string complete_index_file_path = PathFinder::CanonicalizePath(BuildPath() + location_->get_default_file());
        if (PathFinder::PathExist(complete_index_file_path) && IsDir(complete_index_file_path)) {
            return MovedPermanentely(index_file_path + "/");
        } else if (!PathFinder::PathExist(complete_index_file_path)) {
            if (location_->get_listing() && request_.get_request_line().get_method() == "GET")
                set_status_line(200);
            else
                return RedirectToNewTarget(403);
        } else {
            UpdatePathAndTarget(index_file_path);
        }
    }
    Apply();
}

std::string     HttpResponse::GetCompleteRequet() const
{
    return request_.GetCompleteRequest();
}

// void    HttpResponse::AppendToResponse(std::string& message)
// {
//     if (!status_line_.IsComplete())
//         status_line_.Parse(message);
//     if (!message.empty() && !header_.IsComplete()) {
//         header_.Parse(message, HttpHeader::kParseResponse);
//         if (header_.IsComplete()) {
//             if (!header_.NeedBody())
//                 body_.set_is_complete(true);
//             else if (header_.IsContentLength())
//                 body_.SetMode(HttpBody::kModeContentLength, 0, header_.GetContentLength());
//             else
//                 body_.SetMode(HttpBody::kModeTransferEncodingChunked, 0);
//         }
//     }
//     if (!message.empty() && !body_.IsComplete())
//         body_.Parse(message);
//     if (body_.IsComplete())
//         SetComplete();
// }

void    HttpResponse::ParseHeader(std::string& str)
{
    header_.Parse(str, HttpHeader::kParseResponse);
}

bool    HttpResponse::HeaderIsComplete() const
{
    return header_.IsComplete();
}

void    HttpResponse::ClearHeader()
{
    header_.Clear();
}

bool HttpResponse::IsComplete() const
{
    return complete_;
}

void HttpResponse::SetComplete()
{
    response_ = status_line_.GetFormatedStatusLine() + header_.GetFormatedHeader() + body_.get_body();
    complete_ = true;
}

void HttpResponse::AddHeaderContentLength()
{
    if (body_.Size() == 0)
        return;
    std::ostringstream oss;
    oss << body_.Size();
    std::string body_size_str = oss.str();
    header_.AddOneHeader("CONTENT-LENGTH", body_size_str);
}

bool    HttpResponse::IsAskingToCloseConnection() const
{
    std::map<std::string, std::string> map = request_.get_header().get_header_map();
    if (map.find("CONTENT-LENGTH") != map.end() && map.find("TRANSFER-ENCODING") != map.end())
        return true;
    const std::vector<int>& vec = status_line_.get_codes_requiring_close();
    return (std::find(vec.begin(), vec.end(), status_line_.get_status_code()) != vec.end() || !keep_alive_);
}

void    HttpResponse::RedirectToNewTarget(int code)
{
        target_.ClearTarget();
        path_.clear();
        header_.Clear();
        body_.Clear();
        set_status_line(code);
        std::string tmp_request_path = ErrorFileIsSet();
        if (!tmp_request_path.empty() && ++redirect_count_ < kMaxRedirectCount) {
            request_.get_request_line().set_method("GET");
            UpdatePathAndTarget(tmp_request_path);
        }
        if (redirect_count_ >= kMaxRedirectCount)
            return RedirectToEmptyTarget(500);
        return Execute();
}

void    HttpResponse::FinalizeResponse()
{
    UpdateReason();
    AddHeaderContentLength();
    AddHeaderCloseConnection();
    SetComplete();
}

void    HttpResponse::Clear()
{
    complete_ = false;
    status_line_.Clear();
    header_.Clear();
    body_.Clear();
    response_.clear();
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

bool    HttpResponse::IsDir(const std::string& path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

bool    HttpResponse::HasRightToModify(const std::string& path)
{
    std::string parent_dir = path.substr(0, path.find_last_of('/'));
    if (parent_dir.empty())
        parent_dir = "/";
    struct stat statbuf;
    return (access(path.c_str(), W_OK) == 0 && stat(parent_dir.c_str(), &statbuf) == 0 && access(parent_dir.c_str(), W_OK) == 0);
}

void    HttpResponse::Apply()
{
    if (IsHandledExternaly())
        return;

    if (target_.get_target()[target_.get_target().size() - 1] == '/' && location_->get_listing()) {
        Directory   dir(path_, target_.get_target(), location_->get_path().size());
        if (dir.IsOpen())
            body_.set_body(dir.GetHTML());
        else
            return RedirectToNewTarget(403);
    } else if (request_.get_request_line().get_method() == "DELETE") {
        return DeleteResource();
    } else {
        struct stat buffer;
        if ((stat(path_.c_str(), &buffer) == 0 && !S_ISREG(buffer.st_mode)) || access(path_.c_str(), R_OK) == -1)
            return RedirectToNewTarget(403);
        std::ifstream ifs(path_.c_str());
        if (ifs.good()) {
            std::ostringstream buffer;
            buffer << ifs.rdbuf();
            body_.set_body(buffer.str());
        } else {
            return RedirectToNewTarget(500);
        }
    }

    FinalizeResponse();
    if (InitiationDispatcher::Instance().AddWriteFilter(stream_handler_) == -1)
        throw std::runtime_error("Failed to add write filter to InitiationDispatcher");

}

void    HttpResponse::ApplyGeneratedPage()
{
    body_.set_body(HTMLPage::GetErrorPage(status_line_.get_status_code()));
    FinalizeResponse();
    if (InitiationDispatcher::Instance().AddWriteFilter(stream_handler_) == -1)
        throw std::runtime_error("Failed to add write filter to InitiationDispatcher");
}

void    HttpResponse::LaunchCgiHandler()
{
    try
    {
        cgi_path_ = GetCgiPath(FindExtension(path_));
        env_ = SetEnv();
        new CgiHandler(stream_handler_, *this);
    }
    catch(const HttpCodeException& e)
    {
        RedirectToNewTarget(e.Code());
    }
    catch(const std::exception& e)
    {
        RedirectToNewTarget(500);
    }
}

/*

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
    }
}

*/

bool    HttpResponse::IsHandledExternaly()
{
    /*

    if (request_.get_location().get_proxy() != "false") {
        LaunchProxyHandler();
        return true;
    }

    */

    if (IsCgiFile(path_)) {
        LaunchCgiHandler();
        return true;
    }
    return false;
}

void    HttpResponse::DeleteResource()
{
    if (!HasRightToModify(path_))
        return RedirectToNewTarget(403);
    else if (std::remove(path_.c_str()) != 0)
        return RedirectToNewTarget(500);
    else
        return RedirectToEmptyTarget(204);
}

void    HttpResponse::MovedPermanentely(const std::string& new_target)
{
        AddHeaderLocation(new_target);
        if (request_.get_request_line().get_method() == "GET")
            return RedirectToEmptyTarget(301);
        else
            return RedirectToEmptyTarget(308);
}

void    HttpResponse::AddHeaderLocation(const std::string& location)
{
    header_.AddOneHeader("LOCATION", location);
}

void    HttpResponse::AddHeaderCloseConnection()
{
    if (IsAskingToCloseConnection())
        header_.AddOneHeader("CONNECTION", "close");
}

void    HttpResponse::RedirectToEmptyTarget(int code)
{
        target_.ClearTarget();
        path_.clear();
        header_.Clear();
        body_.Clear();
        set_status_line(code);
        return Execute();
}

std::string HttpResponse::BuildPath()
{
    if (!location_->get_root().empty())
        return PathFinder::CanonicalizePath(location_->get_root() + target_.get_target());
    else
        return location_->get_alias() + target_.get_target().substr(location_->get_path().size());
}

void    HttpResponse::UpdatePathAndTarget(const std::string& new_target)
{
    target_.set_target(new_target);
    path_ = BuildPath();
}

void    HttpResponse::UpdateReason()
{
    set_status_line(status_line_.get_status_code());
}

std::string HttpResponse::ErrorFileIsSet() const
{
    const std::map<int, std::string>    errors_files = request_.get_location().get_errors();
    std::map<int, std::string>::const_iterator  it = errors_files.find(status_line_.get_status_code());
    return it == errors_files.end() ? "" : it->second;
}

std::vector<std::string> HttpResponse::SetEnv()
{
    if (status_line_.get_status_code() != 200)
        return std::vector<std::string>();

    std::vector<std::string>    res;
    const std::map<std::string, std::string>&   map = request_.get_header().get_header_map();
    res.push_back("SERVER_PROTOCOL=" + request_.get_request_line().get_http_version());
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
