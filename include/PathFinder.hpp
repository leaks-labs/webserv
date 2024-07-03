#ifndef PATH_FINDER_
#define PATH_FINDER_

#include <iostream>
#include <cstdlib>
#include <fstream>

class PathFinder
{
public:
    static PathFinder& Instance();
    ~PathFinder();
    
    std::string const & GetPhp()const;
    std::string const & GetPython()const;
    
private:
    PathFinder();
    PathFinder(PathFinder const & src);
    PathFinder& operator=(PathFinder const & src);
    bool FileExist(std::string const & path) const;

    std::string php_;
    std::string python_;

};


#endif