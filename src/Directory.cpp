#include "Directory.hpp"

Directory::Directory(std::string const &path, std::string const & root) :
    path_(path),
    root_(root),
    dir_(opendir(path.c_str()))
{
    if(root_.at(root.size() - 1) == '/')
        root_ = root.substr(0, root.size() - 1);
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
    while((file = readdir(dir_)))
    {
        if(file->d_name[0] == '.')
            continue;
        name = file->d_name;
        if(file->d_type == DT_DIR)
            name += "/";
        html.OpenTag("p");
        href = root_ + "/" + name;
        html.AddLinkTag(href, name);
        html.CloseTag("p");
        html.NewLine();
    }
    html.AddButtom();
    return html.GetPage();
}