#ifndef PATH_FINDER_
# define PATH_FINDER_

# include <string>

class PathFinder {
    public:
        static const std::string    kPhpBinName;

        static PathFinder&  Instance();
        static bool         PathExist(const std::string& path);
        static bool         PathIsExecutable(const std::string& path);
        static std::string  CanonicalizePath(const std::string& path);

        ~PathFinder();
        
        const std::string&  GetPhp() const;
        
    private:
        PathFinder();
        PathFinder(const PathFinder& src);
        PathFinder& operator=(const PathFinder& rhs);

        // TODO: need a refactor into a map
        std::string php_;
};

#endif  // PATH_FINDER_
