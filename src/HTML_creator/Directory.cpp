#include "Directory.hpp"

Directory::Directory(const std::string& path, const std::string& request_path, size_t min_request_size)
    : path_(path),
      request_path_(request_path),
      min_request_size_(min_request_size),
      dir_(opendir(path.c_str()))
{
    while (!request_path_.empty() && request_path_[request_path_.length() - 1] == '/')
        request_path_.erase(request_path_.length() - 1);
}

Directory::~Directory()
{
    if (dir_ != NULL)
        closedir(dir_);
}

bool    Directory::IsOpen() const
{
    return dir_ != NULL;
}

std::string Directory::GetHTML() const
{
    struct dirent*  file = NULL;
    std::string     name;
    std::string     href;
    HTMLPage        html;

    std::string parent;
    if (!request_path_.empty())
        parent = request_path_.substr(0, request_path_.rfind('/') + 1);
    html.AddHeader();
    if (parent.length() >= min_request_size_) {
        html.OpenTag("p");
        href = parent;
        html.AddLinkTag(href, "<<");
        html.CloseTag("p");
        html.NewLine();
    }
    while ((file = readdir(dir_)) != NULL) {
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
    return html.get_page();
}
