#include "Directory.hpp"

Directory::Directory(const std::string& path, const std::string& request_path, const std::string& root)
    : path_(path),
      request_path_(request_path),
      root_(root),
      dir_(opendir(path.c_str()))
{
    while (!request_path_.empty() && request_path_.back() == '/')
        request_path_.pop_back();
}

Directory::~Directory()
{
    if(dir_ != NULL)
        closedir(dir_);
}

bool    Directory::IsOpen() const
{
    return dir_ != NULL;
}

std::string Directory::GetHTML() const
{
    struct dirent*  file;
    std::string     name, href;
    HTMLPage        html;

    html.AddHeader();
    if (request_path_ != root_) {
        html.OpenTag("p");
        href = request_path_.substr(0, request_path_.rfind("/") + 1);
        html.AddLinkTag(href, "<<");
        html.CloseTag("p");
        html.NewLine();
    }
    while((file = readdir(dir_)) != NULL) {
        if (file->d_name[0] == '.')
            continue;
        name = file->d_name;
        if (file->d_type == DT_DIR)
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
