#include "Directory.hpp"

Directory::Directory(const char *path) :
    path_(path),
    dir_(opendir(path))
{
    if(!dir_)
        throw std::runtime_error("can't open directory");
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

    html_.OpenTag("head");
    html_.Write("<meta charset=\"UTF-8\">");
    html_.CloseTag("head");
    html_.NewLine();
    html_.OpenTag("html");
    html_.NewLine();
    html_.OpenTag("body");
    html_.NewLine();
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
    html_.CloseTag("body");
    html_.NewLine();
    html_.CloseTag("html");
    html_.NewLine();
}

std::string Directory::GetHtml() const
{
    return html_.GetPage();
}