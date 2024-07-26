#include "PathFinder.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stack>

#include <sys/stat.h>

const std::string   PathFinder::kPhpBinName = "php-cgi";

PathFinder& PathFinder::Instance()
{
    static PathFinder   instance;
    return instance;
}

bool    PathFinder::PathExist(const std::string& path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool    PathFinder::PathIsExecutable(const std::string& path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && (buffer.st_mode & S_IXUSR) != 0);
}

std::string PathFinder::CanonicalizePath(const std::string& path)
{
    std::stack<std::string> stack;
    std::istringstream      iss(path);
    std::string             token;
    
    while (std::getline(iss, token, '/')) {
        if (token == "" || token == ".") {
            continue;
        } else if (token == "..") {
            if (!stack.empty())
                stack.pop();
        } else {
            stack.push(token);
        }
    }
    std::string result;
    while (!stack.empty()) {
        result = "/" + stack.top() + result;
        stack.pop();
    }
    if (result.empty())
        result = "/";
    else if (path[path.size() - 1] == '/')
        result += "/";

    return result;
}

PathFinder::~PathFinder()
{
}

const std::string&  PathFinder::GetPhp() const
{
    return php_;
}

PathFinder::PathFinder()
{        
    const std::string   value(std::getenv("PATH"));
    std::string folder;
    std::string path;
    size_t start = 0;
    size_t end;
    do {
        end = value.find(':', start);
        folder = value.substr(start, end - start);
        if (folder.empty())
            folder = ".";
        path = folder + "/" + kPhpBinName;
        if (PathIsExecutable(path)) {
            php_ = path;
            break;
        }
        start = end + 1;
    } while (end != std::string::npos);
}
