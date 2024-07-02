#include "Directory.hpp"

Directory::Directory(const char *path) :
    path_(path),
    dir_(opendir(path))
{
    if(!dir_)
        WriteError();
    else
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
        href = path_ + name;
        html_.AddLinkTag(href, name);
        html_.CloseTag("p");
        html_.NewLine();
    }
    html_.AddButtom();
}

void Directory::WriteError()
{
    html_.AddHeader();
    html_.OpenTag("h1");
    html_.Write("Error 404");
    html_.CloseTag("h1");
    html_.AddButtom();
    html_.NewLine();
}

std::string Directory::GetHtml() const
{
    return html_.GetPage();
}