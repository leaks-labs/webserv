#include "HttpResponse.hpp"

HttpResponse::HttpResponse(StreamHandler & stream_handler, HttpRequest * request, int acceptor_sfd) : 
    stream_handler_(stream_handler),
    request_(request),
    server_(ServerList::Instance().FindServer(acceptor_sfd, request_->get_header().get_header_map().find("HOST")->second)),
    request_path_(request_->get_request_line().get_target().target),
    args_(ExtractArgs()),
    location_(server_.FindLocation(request_path_)),
    path_(BuildPath()),
    cgi_path_(GetCgiPath(FindExtension(path_))),
    error_(200),
    complete_(false)
{
    if(IsCgiFile(path_))
    {
        LaunchCgiHandler();
        return;
    }
    Directory dir(path_, request_path_, location_->get_path());
    if(dir.IsOpen())
        ReadDirectory(dir);
    else
        ReadFile();
    set_complete();
    stream_handler_.Register();
}

HttpResponse::HttpResponse(HttpResponse const & src) :
    stream_handler_(src.get_stream_handler()),
    request_(src.get_request()),
    server_(src.get_server()),
    request_path_(src.get_request_path()),
    args_(src.get_args()),
    location_(src.get_location()),
    path_(src.get_path()),
    body_(src.get_body()),
    cgi_path_(src.get_cgi_path()),
    error_(src.get_error()),
    complete_(src.get_complete())
{
    if (complete_)
        set_complete();
}

HttpResponse::~HttpResponse()
{
}

std::string HttpResponse::ExtractArgs()
{
    size_t pos = request_path_.find("?");
    if (pos == std::string::npos || pos == request_path_.size() - 1)
        return "";
    std::string res = request_path_.substr(pos + 1, request_path_.size() - (pos + 1));
    request_path_ = request_path_.substr(0, pos);
    return res;
}

std::string HttpResponse::BuildPath()
{
    if(!location_)
    {
        error_ = 404;
        return "";
    }
    if (location_->get_strict())
        return location_->get_root() + location_->get_default_file();
    size_t start = location_->get_path().size();
    return location_->get_root() + request_path_.substr(start, request_path_.size() - start);
}

void HttpResponse::LaunchCgiHandler()
{
    try
    {
        new CgiHandler(stream_handler_, *this);
    }
    catch(const std::exception& e)
    {
        throw;
    }
}

void HttpResponse::ReadDirectory(Directory & dir)
{
    std::string path;

    if(location_->get_listing())
    {
        body_ =  dir.GetHTML();
        return;
    }
    path_ = location_->get_root() + "/" + location_->get_default_file();
    ReadFile();
}

void HttpResponse::ReadFile()
{
    std::ifstream ifs (path_.c_str());
    if(!ifs)
        return;
    std::filebuf* pbuf = ifs.rdbuf();
    std::size_t size = pbuf->pubseekoff(0, ifs.end, ifs.in);
    pbuf->pubseekpos (0, ifs.in);
    char* buf = new char[size];
    pbuf->sgetn (buf, size);
    ifs.close();
    body_ = std::string(buf, size);
    delete [] buf;
}

std::string HttpResponse::CreateHeader()
{
    typedef std::map<std::string, std::string>::const_iterator iterator;
    std::map<std::string, std::string> map;
    std::stringstream ss;
    std::string header;

    header += request_->get_request_line().get_http_version() + " ";
    ss << error_;
    map = HttpMessage::status_map;
    for (iterator it= map.begin(); it != map.end(); ++it)
    {
        if (it->second == ss.str())
        {
            header += it->second + " " + it->first + + "\r\n";
            break;
        }
    }
    map = request_->get_header().get_header_map();
    for (iterator it= map.begin(); it != map.end(); ++it)
        header += std::string(it->first) + ": " + std::string(it->second) + "\r\n";
    header += "Content-type: text/html\r\n";
    ss.str("");
    ss << body_.size();
    header += "Content-Length: " + ss.str() + "\r\n\r\n";
    return header;
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
    std::cout << this->get_cgi_path() << std::endl;
    body_ += std::string(str); // remplacer body_ par buffer_
}

void HttpResponse::set_complete()
{
    complete_ = true;
    buffer_ = CreateHeader() + body_;
}

void HttpResponse::RemoveFirstBodyLine() // SHOULD EXTRACT ENCODING TYPE AND CONTENT TYPE HERE
{
    size_t pos = body_.find("\n");
    body_ = body_.substr(pos, body_.size() - pos);
}

StreamHandler & HttpResponse::get_stream_handler() const
{
    return stream_handler_;
}

HttpRequest *HttpResponse::get_request() const
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

std::string const & HttpResponse::get_args() const 
{
    return args_;
}

int HttpResponse::get_error() const 
{
    return error_;
}

std::string  const &HttpResponse::get_body() const
{
    return body_;
}

std::string & HttpResponse::get_buffer()
{
    return buffer_;
}

std::string  const & HttpResponse::get_cgi_path() const
{
    return cgi_path_;
}

bool HttpResponse::get_complete() const
{
    return complete_;
}