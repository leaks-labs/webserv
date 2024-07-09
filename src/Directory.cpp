#include "Directory.hpp"

Directory::Directory(std::string const &path, std::string const & request_path,  std::string const & root) :
    path_(path),
    request_path_(request_path),
    root_(root),
    dir_(opendir(path.c_str()))
{
    if(request_path_.at(request_path.size() - 1) == '/')
        request_path_ = request_path.substr(0, request_path.size() - 1);
}

Directory::Directory(const Directory &src)
{
    *this = src;
}

Directory& Directory::operator=(const Directory& src)
{
    if(&src != this)
        dir_ = src.dir_;
    return *this;
}

Directory::~Directory()
{
    if(dir_)
        closedir(dir_);
}

bool Directory::IsOpen() const
{
    if(dir_)
        return true;
    return false;
}

std::string Directory::GetHTML() const
{
    struct dirent *file;
    std::string name;
    std::string href;
    HTMLPage html;

    html.AddHeader();
    if(request_path_ != root_)
    {
        html.OpenTag("p");
        href = request_path_.substr(0, request_path_.rfind("/") + 1);
        html.AddLinkTag(href, "<<");
        html.CloseTag("p");
        html.NewLine();
    }
    while((file = readdir(dir_)))
    {
        if(file->d_name[0] == '.')
            continue;
        name = file->d_name;
        if(file->d_type == DT_DIR)
            name += "/";
        html.OpenTag("p");
        href = request_path_ + "/" + name;
        html.AddLinkTag(href, name);
        html.CloseTag("p");
        html.NewLine();
    }
    html.AddButtom();
    return html.GetPage();
}