#include "Directory.hpp"

Directory::Directory(std::string const &path, std::string const & root) :
    path_(path),
    root_(root),
    dir_(opendir(path.c_str()))
{
    if(!dir_)
        throw std::runtime_error("Can't open directory");
    WriteHTML();
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
    closedir(dir_);
}

void Directory::WriteHTML()
{
    struct dirent *file;
    std::string name;
    std::string href;

    html_.AddHeader();
    while((file = readdir(dir_)))
    {
        if(file->d_name[0] == '.')
            continue;
        name = file->d_name;
        if(file->d_type == DT_DIR)
            name += "/";
        html_.OpenTag("p");
        href = root_ + "/" + name;
        html_.AddLinkTag(href, name);
        html_.CloseTag("p");
        html_.NewLine();
    }
    html_.AddButtom();
}

std::string Directory::GetHtml() const
{
    return html_.GetPage();
}