#include "PathFinder.hpp"

#include <cstdlib>
#include <fstream>

PathFinder &PathFinder::Instance()
{
    static PathFinder   instance;
    return instance;
}

bool PathFinder::FileExist(const std::string& path)
{
    return std::ifstream(path.c_str()).good();

    //TODO: or ?
    // struct stat buffer;
    // return (stat(path.c_str(), &buffer) == 0 && buffer.st_mode & S_IXUSR);
}

PathFinder::~PathFinder()
{
}

const std::string&  PathFinder::GetPhp() const
{
    return php_;
}

const std::string&  PathFinder::GetPython() const
{
    return python_;
}

PathFinder::PathFinder()
{        
    // TODO: need a refactor
    const std::string   value(std::getenv("PATH"));
    std::string folder, path;
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
