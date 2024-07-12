#include "PathFinder.hpp"

#include <cstdlib>
#include <fstream>

PathFinder::PathFinder()
{        
    const std::string value = std::string(std::getenv("PATH"));
    std::string folder;
    std::string path;
    size_t start = 0;
    size_t end;
    do {
        end = value.find(":", start);
        folder = value.substr(start, end - start);
        path = folder + "/php-cgi";
        if (FileExist(path))
            php_ = path;
        path = folder + "/python3";
        if (FileExist(path))
            python_ = path;
        start = end + 1;
    } while (end != std::string::npos);
}

PathFinder::~PathFinder()
{
}

PathFinder &PathFinder::Instance()
{
    static PathFinder instance;
    return instance;
}

bool PathFinder::FileExist(std::string const & path)
{
    std::ifstream file(path.c_str());
    return file.good();
}

std::string const & PathFinder::GetPhp()const
{
    return php_;
}

std::string const & PathFinder::GetPython()const
{
    return python_;
}
