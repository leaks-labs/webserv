#ifndef DIRECTORY_HPP_
# define DIRECTORY_HPP_

# include <dirent.h>

# include "HTMLPage.hpp"

class Directory {
    public:
        Directory(const std::string& path, const std::string& request_path, size_t min_request_size);

        ~Directory();

        bool        IsOpen() const;
        std::string GetHTML() const;

    private:
        Directory();
        Directory(const Directory& src);
        Directory&  operator=(const Directory& rhs);

        std::string path_;
        std::string request_path_;
        size_t      min_request_size_;
        DIR*        dir_;
};

#endif  // DIRECTORY_HPP_
