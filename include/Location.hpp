#ifndef LOCATION_HPP_
# define LOCATION_HPP_

# include <map>
# include <string>

class Location {
    public:
        static const int    kInvalidKey = -1;
        static const int    kValidKey = 0;

        static const int    kMethodNone = 0;
        static const int    kMethodGet = 1 << 0;
        static const int    kMethodPost = 1 << 1;
        static const int    kMethodDelete = 1 << 2;

        static const int    kCgiNone = 0;
        static const int    kCgiPHP = 1 << 0;

        Location();
        Location(const Location& src);
        Location&   operator=(const Location& rhs);

        ~Location();

        const std::string&                  get_path() const;
        const std::string&                  get_root() const;
        const std::string&                  get_alias() const;
        const std::string&                  get_default_file() const;
        const std::string&                  get_redirect() const;
        const std::map<int, std::string>&   get_errors() const;
        int                                 get_cgi() const;
        int                                 get_methods() const;
        size_t                              get_bodymax() const;
        bool                                get_listing() const;
        bool                                get_strict() const;
        const std::string&                  get_path_info() const;

        void    set_path(const std::string& value);
        void    set_root(const std::string& value);
        void    set_alias(const std::string& value);
        void    set_default_file(const std::string& value);
        void    set_redirect(const std::string& value);
        void    set_errors(const std::string& value);
        void    set_cgi(const std::string& value);
        void    set_methods(const std::string& value);
        void    set_bodymax(const std::string& value);
        void    set_listing(const std::string& value);
        void    set_strict(bool value);
        void    set_path_info(const std::string& value);

        bool    HasMethod(const std::string& value) const;
        int     SetValue(const std::string& key, const std::string& value);
        size_t  Compare(const std::string& path) const;
        bool    StrictCompare(const std::string& path) const;
        void    Print() const;

    private:
        static const std::map<std::string, void (Location::*)(const std::string&)>  set_functions_;
        static const std::map<std::string, int>                                     methods_ref_;
        static const std::map<std::string, int>                                     cgi_ref_;

        static const std::map<std::string, void (Location::*)(const std::string&)>  InitSetFunctions();
        static const std::map<std::string, int>                                     InitMethodsRef();
        static const std::map<std::string, int>                                     InitCgiRef();

        bool    IsAbsolutePath(const std::string&) const;

        std::string                 path_;
        std::string                 root_;
        std::string                 alias_;
        std::string                 default_file_;
        std::string                 redirect_;
        std::map<int, std::string>  errors_;
        int                         cgi_;
        int                         methods_;
        size_t                      bodymax_;
        bool                        listing_;
        bool                        strict_;
        std::string                 path_info_;
};

#endif  // LOCATION_HPP_
