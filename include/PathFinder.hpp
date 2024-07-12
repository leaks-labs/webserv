#ifndef PATH_FINDER_
# define PATH_FINDER_

# include <string>

class PathFinder {
    public:
        static PathFinder&  Instance();
        static bool         FileExist(const std::string& path);

        ~PathFinder();
        
        std::string const & GetPhp()const;
        std::string const & GetPython()const;
        
    private:
        PathFinder();
        PathFinder(PathFinder const & src);
        PathFinder& operator=(PathFinder const & src);

        std::string php_;
        std::string python_;
};

#endif  // PATH_FINDER_
