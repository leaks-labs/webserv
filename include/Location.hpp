#ifndef LOCATION_HPP_
# define LOCATION_HPP_

# include <map>
# include <string>

# include <unistd.h>

class Location {
    public:
        static const int    kMethodNone = 0;
        static const int    kMethodGet = 1 << 0;
        static const int    kMethodPost = 1 << 1;
        static const int    kMethodDelete = 1 << 2;

        static const int    kCgiNone = 0;
        static const int    kCgiPHP = 1 << 0;
        static const int    kCgiPython = 1 << 1;

        Location();
        Location(const Location& src);
        Location&   operator=(const Location& rhs);

        ~Location();

        const std::string&  get_path() const;
        const std::string&  get_root() const;
        const std::string&  get_default_file() const;
        const std::string&  get_proxy() const;
        const std::string&  get_errors() const;
        int                 get_cgi() const;
        int                 get_methods() const;
        int                 get_bodymax() const;
        bool                get_listing() const;
        bool                get_strict() const;

        int     set_path(const std::string& value);
        int     set_root(const std::string& value);
        int     set_default_file(const std::string& value);
        int     set_proxy(const std::string& value);
        int     set_errors(const std::string& value);
        int     set_cgi(const std::string& value);
        int     set_methods(const std::string& value);
        int     set_bodymax(const std::string& value);
        int     set_listing(const std::string& value);
        void    set_strict(bool value);

        int     SetValue(const std::string& key, const std::string& value);
        void    Print() const;

    private:
        static const std::map<const std::string, int(Location::*)(const std::string&)>  set_functions_;

        static const std::map<const std::string, int(Location::*)(const std::string&)>  InitSetFunctions();

        std::string     path_;
        std::string     root_;
        std::string     default_file_;
        std::string     proxy_;
        std::string     errors_;
        int             cgi_;
        int             methods_;
        int             bodymax_;
        bool            listing_;
        bool            strict_;
};

#endif  // LOCATION_HPP_
