#ifndef DIRECTORY_HPP_
# define DIRECTORY_HPP_

# include <dirent.h>

# include "HTMLPage.hpp"

class Directory
{
public:
    Directory(const std::string& path, const std::string& request_path, const std::string& root);

    ~Directory();

    bool        IsOpen() const;
    std::string GetHTML() const;

private:
    Directory();

    std::string path_;
    std::string request_path_;
    std::string root_;
    DIR*        dir_;
};


#endif  // DIRECTORY_HPP_
