#ifndef DIRECTORY_HPP_
#define DIRECTORY_HPP_

#include <sys/types.h>
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
    void WriteHTML();
    std::string GetHtml()const;
private:
    std::string path_;
    std::string root_;
    DIR* dir_;
    HTMLPage html_;
    Directory();
};


#endif