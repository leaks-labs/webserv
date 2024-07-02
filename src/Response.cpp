#include "Response.hpp"

Response::Response(int client_sfd, std::string const & server_name, std::string const & path) :
server_(FindServer(client_sfd, server_name)),
location_(server_.FindLocation(path))
{
    std::stringstream ss;
    std::string content;

    if(!location_->has_method("GET"))
        content = HTMLPage::GetErrorPage();
    else
        content = Build(path);
    ss << content.size();
    text_ = "HTTP/1.1 200 OK\r\nContent-Length: " + ss.str() + "\r\n\r\n";
    text_ += content;
}

Response::~Response()
{
}

std::string Response::GetText() const
{
    return text_;
}

const Server& Response::FindServer(const int acceptor_sfd, const std::string& name) const
{
    typedef std::vector<const Server *>::iterator Iterator;
    std::vector<const Server *> matched;
    ServerList& server_list = ServerList::Instance();
    std::cout << "client_sfd: " << acceptor_sfd << matched.size() << std::endl;
    for (size_t i = 0; i < server_list.Size(); i++)
        if(AcceptorRecords::IsSameAddr(acceptor_sfd, server_list[i].get_addr()))
            matched.push_back(&server_list[i]);
    for (Iterator it = matched.begin(); it != matched.end(); it++)
    {
        if((*it)->HasServerName(name))
            return **it;
    }
    return *matched[0];
}

std::string Response::Build(std::string path) const
{
    std::string realpath;
    std::string res;

    try
    {
        if(location_->get_strict())
            realpath =  location_->get_root();
        else
            realpath = BuildPath(path);
        if(realpath[realpath.size() - 1] != '/')
            res = ReadFile(realpath);
        else
        {
            if(location_->get_listing())
            {
                Directory dir(realpath.c_str());
                res = dir.GetHtml();
            }
            else
                res = ReadFile(location_->get_root() + "/" + location_->get_default_file());
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        res = HTMLPage::GetErrorPage();
    }
    return res;
}


std::string Response::BuildPath(std::string path) const
{
    if(!location_)
        return "";
    return location_->get_root() + path.substr(location_->get_path().size(), path.size());
}

std::string Response::ReadFile(std::string path) const
{
    std::ifstream ifs (path.c_str());
    if(!ifs)
        return HTMLPage::GetErrorPage();
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
