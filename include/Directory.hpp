#ifndef DIRECTORY_HPP_
#define DIRECTORY_HPP_

#include <dirent.h>
#include <iostream>
#include <HTMLPage.hpp>

class Directory
{
public:
    Directory(std::string const & path, std::string const & root);
    Directory(const Directory &src);
    Directory& operator=(const Directory& src);
    ~Directory();

    std::string GetHTML() const;
    bool IsOpen() const;

private:
    Directory();

    std::string path_;
    std::string root_;
    DIR* dir_;
};


#endif