#ifndef DIRECTORY_HPP_
#define DIRECTORY_HPP_

#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <HTMLPage.hpp>

class Directory
{
private:
    std::string path_;
    DIR* dir_;
    HTMLPage html_;
    Directory();
public:
    Directory(const char *path);
    Directory(const Directory &src);
    Directory& operator=(const Directory& src);
    ~Directory();
    void WriteHTML();
    std::string GetHtml()const;
};


#endif