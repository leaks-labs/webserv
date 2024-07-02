#include "Response.hpp"

Response::Response(int client_sfd, int client_pfd[2], std::string const & server_name, std::string const & path) :
server_(FindServer(client_sfd, server_name)),
location_(server_.FindLocation(path))
{
    std::string content;
    std::string args = "arg1=toto";
    if(!location_->has_method("GET"))
        WriteError(client_pfd[1]);
    else
        Write(client_pfd, path, args);
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

void Response::Write(const int client_pfd[2], std::string const & path, std::string const & args) const
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
            CreateFile(client_pfd, realpath, args);
        else
        {
            if(location_->get_listing())
            {
                Directory dir(realpath, location_->get_path());
                res = dir.GetHtml();
            }
            else
                res = ReadFile(location_->get_root() + "/" + location_->get_default_file());
            WriteFile(client_pfd[1], res);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Response error: " << e.what() << '\n';
        WriteError(client_pfd[1]);
    }
}


std::string Response::BuildPath(std::string const &path) const
{
    if(!location_)
        return "";
    return location_->get_root() + "/" +path.substr(location_->get_path().size(), path.size());
}

std::string Response::FindExtension(std::string const & str) const
{
    size_t pos = str.rfind(".");
    if(pos == str.size() - 1 || pos == std::string::npos)
        return "";
    return str.substr(pos + 1, str.size() - pos);
}

void Response::CreateFile(const int client_pfd[2], std::string const & path, std::string const & arg) const
{
    //size_t size  = path.size();
    std::string cgi_path;
    std::string extension = FindExtension(path);
    CgiPathFinder& finder = CgiPathFinder::Instance();
    std::string res;
    if (extension == "php")
        cgi_path = finder.GetPhp();
    else if (extension == "py")
        cgi_path = finder.GetPython();
    else
    {
        WriteFile(client_pfd[1], ReadFile(path));
        return;
    }
    try
    {
        ExecCgi(client_pfd, path, arg, cgi_path);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        WriteError(client_pfd[1]);
    }
}

void Response::ExecCgi(const int client_pfd[2], std::string const & path, std::string const & arg, std::string const & cgi_path) const
{
    int pid;
    std::vector<char *> cmd;
    char **c_cmd;
    cmd.push_back(const_cast<char*>(cgi_path.c_str()));
    cmd.push_back(const_cast<char*>(path.c_str()));
    cmd.push_back(const_cast<char*>(arg.c_str()));
    cmd.push_back(NULL);
    c_cmd = &cmd[0];
    pid = fork();
    if(pid == -1)
        throw std::runtime_error("failed fork for cgi");
    else if(pid == 0)
    {
        dup2(client_pfd[1], 1);
        close(client_pfd[0]);
        close(client_pfd[1]);
        execve(c_cmd[0], c_cmd, NULL);
    }
}

std::string Response::ReadFile(std::string const & path) const
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

void Response::WriteError(const int client_pfd) const
{
    std::string body = HTMLPage::GetErrorPage();
    write(client_pfd, body.c_str(), body.size());
}

void Response::WriteFile(const int client_pfd, std::string const & body) const
{
    write(client_pfd, body.c_str(), body.size());
}