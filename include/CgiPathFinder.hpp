#ifndef CGI_PATH_FINDER_
#define CGI_PATH_FINDER_

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
class CgiPathFinder
{
private:
    CgiPathFinder();
    CgiPathFinder(CgiPathFinder const & src);
    CgiPathFinder& operator=(CgiPathFinder const & src);
    bool FileExist(std::string const & path) const;
    std::string php_;
    std::string python_;
public:
    static CgiPathFinder& Instance();
    ~CgiPathFinder();
    std::string const & GetPhp()const;
    std::string const & GetPython()const;
};


#endif