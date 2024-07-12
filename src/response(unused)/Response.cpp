#include "Response.hpp"

Response::Response(int client_sfd, int pfd[2], std::string const & server_name, std::string const & path) :
    server_(ServerList::Instance().FindServer(client_sfd, server_name)),
    location_(server_.FindLocation(path))
{
    std::memcpy(pfd_, pfd, sizeof(int) * 2);
    std::string args = "arg1=toto";
    if(!location_->HasMethod("GET"))
        GetError(400);
    else
        Get(path, args);
}

Response::~Response()
{
}

std::string Response::BuildPath(std::string const &path) const
{
    if(!location_)
        return "";
    return location_->get_root() + path.substr(location_->get_path().size(), path.size());
}

std::string Response::FindExtension(std::string const & str) const
{
    size_t pos = str.rfind(".");
    if(pos == str.size() - 1 || pos == std::string::npos)
        return "";
    return str.substr(pos + 1, str.size() - pos);
}

bool Response::IsCgiFile(std::string const & path) const
{
    std::string extension = FindExtension(path);
    if (extension == "php" || extension == "py")
        return true;
    return false;
}

std::string Response::GetCgiPath(std::string const & ext) const
{
    PathFinder& finder = PathFinder::Instance();
    if (ext == "php")
        return finder.GetPhp();
    else if (ext == "py")
        return finder.GetPython();
    return "none";
}

void Response::Get(std::string const & path, std::string const & args) const
{
    std::string realpath;
    std::string res;

    if(location_->get_strict())
    {
        realpath =  location_->get_root();
        if(path != realpath)
        {
            GetError(404);
            return;
        }
    }
    else
        realpath = BuildPath(path);
    if (IsCgiFile(path))
    {
        GetCgi(realpath, args);
        return;
    }
    Directory dir(realpath, location_->get_path());
    if(dir.IsOpen())
        GetDirectory(dir, args);
    else
        GetFile(realpath);
}


void Response::GetError(const int error) const
{
    std::string const & path = location_->get_errors().find(error)->second;
    std::ifstream ifs (path.c_str());
    if(ifs)
        GetFile(path);
}

void Response::GetCgi(std::string const & path, std::string const & args) const
{
    std::string cgi_path;
    std::vector<char *> cmd;
    char **c_cmd;
    int pid;

    cgi_path = GetCgiPath(FindExtension(path));
    cmd.push_back(const_cast<char*>(cgi_path.c_str()));
    cmd.push_back(const_cast<char*>(path.c_str()));
    cmd.push_back(const_cast<char*>(args.c_str()));
    cmd.push_back(NULL);
    c_cmd = &cmd[0];
    pid = fork();
    if(pid == -1)
        throw std::runtime_error("failed fork for cgi");
    else if(pid == 0)
    {
        dup2(pfd_[1], 1);
        close(pfd_[0]);
        close(pfd_[1]);
        execve(c_cmd[0], c_cmd, NULL);
    }
}

void Response::GetDirectory(Directory & dir, std::string const & args) const
{
    std::string path;

    if(location_->get_listing())
    {
        Write(dir.GetHTML());
        return;
    }
    path = location_->get_root() + "/" + location_->get_default_file();
    if(IsCgiFile(path))
        GetCgi(path, args);
    else
        GetFile(path);
}

void Response::GetFile(std::string const & path) const
{
    std::ifstream ifs (path.c_str());
    if(!ifs)
    {
        GetError(404);
        return;
    }
    std::filebuf* pbuf = ifs.rdbuf();
    std::size_t size = pbuf->pubseekoff(0,ifs.end,ifs.in);
    pbuf->pubseekpos (0,ifs.in);
    char* buf = new char[size];
    pbuf->sgetn (buf, size);
    ifs.close();
    std::string res = std::string(buf);
    delete [] buf;
    Write(res);
}

void Response::Write(std::string const & body) const
{
    write(pfd_[1], body.c_str(), body.size());
}