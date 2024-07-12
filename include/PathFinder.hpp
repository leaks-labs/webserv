#ifndef PATH_FINDER_
# define PATH_FINDER_

# include <string>

class PathFinder {
    public:
        static PathFinder&  Instance();
        static bool         FileExist(const std::string& path);

        ~PathFinder();
        
        const std::string&  GetPhp() const;
        const std::string&  GetPython() const;
        
    private:
        PathFinder();
        PathFinder(PathFinder const & src);
        PathFinder& operator=(const PathFinder& rhs);

        // TODO: need a refactor into a map
        std::string php_;
        std::string python_;
};

#endif  // PATH_FINDER_
