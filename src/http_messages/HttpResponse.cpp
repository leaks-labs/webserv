#include "HttpResponse.hpp"

HttpResponse::HttpResponse(StreamHandler & stream_handler, HttpRequest & request, int acceptor_sfd) : 
    stream_handler_(stream_handler),
    request_(request),
    server_(ServerList::Instance().FindServer(acceptor_sfd, request_.get_header().get_header_map().find("HOST")->second)),
    request_path_(request_.get_request_line().get_target().target),
    location_(server_.FindLocation(request_path_)),
    path_(BuildPath()),
    cgi_path_(GetCgiPath(FindExtension(path_))),
    error_(200),
    complete_(false)
{
    //request_.get_header().Print();
    //request_.get_request_line().Print();
    CreateBody();
}

HttpResponse::HttpResponse(HttpResponse const & src) :
    stream_handler_(src.get_stream_handler()),
    request_(src.get_request()),
    server_(src.get_server()),
    request_path_(src.get_request_path()),
    location_(src.get_location()),
    path_(src.get_path()),
    body_(src.get_body()),
    cgi_path_(src.get_cgi_path()),
    error_(src.get_error()),
    complete_(src.get_complete())
{
}

HttpResponse::~HttpResponse()
{
}

std::string HttpResponse::BuildPath()
{
    if(!location_ || (location_->get_strict() && location_->get_path() != request_path_))
    {
        error_ = 404;
        return "";
    }
    size_t start = location_->get_path().size();
    //std::cout << "request_path: " << request_path_ << std::endl;
    return location_->get_root() + request_path_.substr(start, request_path_.size() - start);
}


void HttpResponse::CreateBody()
{
    //std::cout << "path: " << path_ << std::endl;
    if(IsCgiFile(path_))
    {
        CgiHandler cgi(stream_handler_, *this);
        return;
    }
    Directory dir(path_, request_path_, location_->get_path());
    if(dir.IsOpen())
        body_ = ReadDirectory(dir);
    else
        body_ = ReadFile();
    complete_ = true;
}

std::string HttpResponse::ReadDirectory(Directory & dir)
{
    std::string path;

    if(location_->get_listing())
        return dir.GetHTML();
    path_ = location_->get_root() + "/" + location_->get_default_file();
    return(ReadFile());
}

std::string HttpResponse::ReadFile()
{
    std::ifstream ifs (path_.c_str());
    if(!ifs)
        return "";
    std::filebuf* pbuf = ifs.rdbuf();
    std::size_t size = pbuf->pubseekoff(0, ifs.end, ifs.in);
    pbuf->pubseekpos (0, ifs.in);
    char* buf = new char[size];
    pbuf->sgetn (buf, size);
    ifs.close();
    std::string res = std::string(buf, size);
    delete [] buf;
    return res;
}

std::string HttpResponse::CreateHeader()
{
    std::stringstream ss;
    ss << body_.size(); // A REMPLACER PAR body_.size();
    return "HTTP/1.1 200 OK\r\nContent-Length: " + ss.str() + "\r\n\r\n";
}

std::string HttpResponse::FindExtension(std::string const & str) const
{
    size_t pos = str.rfind(".");
    if(pos == str.size() - 1 || pos == std::string::npos)
        return "";
    return str.substr(pos + 1, str.size() - pos);
}

bool HttpResponse::IsCgiFile(std::string const & path) const
{
    std::string extension = FindExtension(path);
    if (extension == "php" || extension == "py")
        return true;
    return false;
}

std::string HttpResponse::GetCgiPath(std::string const & ext) const
{
    PathFinder& finder = PathFinder::Instance();
    if (ext == "php")
        return finder.GetPhp();
    else if (ext == "py")
        return finder.GetPython();
    return "none";
}

void HttpResponse::addToBuffer(std::string const & str)
{
    body_ += str; // remplacer body_ par buffer_
}

void HttpResponse::set_complete()
{
    complete_ = true;
}

StreamHandler & HttpResponse::get_stream_handler() const
{
    return stream_handler_;
}

HttpRequest &HttpResponse::get_request() const
{
    return request_;
}

Server const &HttpResponse::get_server() const
{
    return server_;
}

Location    const *HttpResponse::get_location() const
{
    return location_;
}

std::string const & HttpResponse::get_request_path() const 
{
    return request_path_;
}

std::string const & HttpResponse::get_path() const 
{
    return path_;
}

int HttpResponse::get_error() const 
{
    return error_;
}

std::string  const &HttpResponse::get_body() const
{
    return body_;
}

std::string HttpResponse::get_content()
{
    return CreateHeader() + body_;
}

std::string  const & HttpResponse::get_cgi_path() const
{
    return cgi_path_;
}

bool HttpResponse::get_complete() const
{
    return complete_;
}