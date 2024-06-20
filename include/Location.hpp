#ifndef LOCATION_HPP_
# define LOCATION_HPP_

# include <fstream>
# include <iostream>
# include <map>
# include <sstream>
# include <vector>

# include <unistd.h>

class Location {
    public:
        static const int    kGet = 1;
        static const int    kPost = 2;
        static const int    kDelete = 4;

        Location();
        Location(const Location& src);
        Location&   operator=(const Location& rhs);

        ~Location();

        const std::string&  get_path() const;
        const std::string&  get_root() const;
        const std::string&  get_default_file() const;
        const std::string&  get_cgi() const;
        const std::string&  get_proxy() const;
        int                 get_methods() const;
        bool                get_listing() const;

        int set_path(const std::string& value);
        int set_root(const std::string& value);
        int set_default_file(const std::string& value);
        int set_cgi(const std::string& value);
        int set_proxy(const std::string& value);
        int set_methods(const std::string& value);
        int set_listing(const std::string& value);

        int     SetValue(const std::string& key, const std::string& value);
        void    Print() const;

    private:
        void    InitSetFunctions();

        std::string     path_;
        std::string     root_;
        std::string     default_file_;
        std::string     cgi_;
        std::string     proxy_;
        unsigned int    methods_;
        bool            listing_;
        std::map<const std::string, int(Location::*)(const std::string&)> set_functions_;
};

#endif  // LOCATION_HPP_
