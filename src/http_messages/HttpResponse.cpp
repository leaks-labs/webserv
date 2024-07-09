#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest & request, int acceptor_sfd) : 
    request_(request),
    server_(ServerList::Instance().FindServer(acceptor_sfd, request_.get_header().get_header_map().find("HOST")->second)),
    request_path_(request_.get_request_line().get_target().target),
    location_(server_.FindLocation(request_path_)),
    path_(BuildPath()),
    body_(CreateBody()),
    header_(CreateHeader()),
    error_(0)
{
    //request_.get_header().Print();
    //request_.get_request_line().Print();
}

HttpResponse::HttpResponse(HttpResponse const & src) :
    request_(src.get_request()),
    server_(src.get_server()),
    request_path_(src.get_request_path()),
    location_(src.get_location()),
    path_(src.get_path()),
    body_(src.get_body()),
    header_(src.get_header()),
    error_(src.get_error())
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
    std::cout << "request_path: " << request_path_ << std::endl;
    return location_->get_root() + request_path_.substr(start, request_path_.size() - start);
}


std::string HttpResponse::CreateBody()
{
    std::cout << "path: " << path_ << std::endl;
    Directory dir(path_, request_path_);
    if(dir.IsOpen())
        return ReadDirectory(dir);
    return ReadFile();  
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
    std::size_t size = pbuf->pubseekoff(0,ifs.end,ifs.in);
    pbuf->pubseekpos (0,ifs.in);
    char* buf = new char[size];
    pbuf->sgetn (buf, size);
    ifs.close();
    std::string res = std::string(buf);
    delete [] buf;
    return res;
}

std::string HttpResponse::CreateHeader()
{
    std::stringstream ss;
    ss << body_.size();
    return "HTTP/1.1 200 OK\r\nContent-Length: " + ss.str() + "\r\n\r\n";
}

HttpResponse::~HttpResponse()
{
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

std::string  const &HttpResponse::get_header() const
{
    return header_;
}


std::string  HttpResponse::get_content() const
{
    return std::string(header_ + body_);
}