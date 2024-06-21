#include "Location.hpp"

#include <iostream>

const std::map<const std::string, int(Location::*)(const std::string&)> Location::set_functions_ = Location::InitSetFunctions();

Location::Location()
    : path_("/"),
      root_("/"),
      default_file_("/data/default.html"),
      cgi_("php"),
      proxy_("false"),
      methods_(7),
      listing_(true)
{
}

Location::Location(const Location& src)
{
    *this = src;
}

Location&   Location::operator=(Location const& rhs)
{
    if (this != &rhs) {
        path_ = rhs.get_path();
        root_ = rhs.get_root();
        default_file_ = rhs.get_default_file();
        cgi_ = rhs.get_cgi();
        proxy_ = rhs.get_proxy();
        methods_ = rhs.get_methods();
        listing_ = rhs.get_listing();
    }
    return *this;
}

Location::~Location()
{
}

const std::string&  Location::get_path() const
{
    return path_;
}

const std::string&  Location::get_root() const
{
    return root_;
}

const std::string&  Location::get_default_file() const
{
    return default_file_;
}

const std::string&  Location::get_cgi() const
{
    return cgi_;
}

const std::string&  Location::get_proxy() const
{
    return proxy_;
}

int Location::get_methods() const
{
    return methods_;
}

bool    Location::get_listing() const
{
    return listing_;
}

int Location::set_path(const std::string& value)
{
    path_ = value;
    return 0;
}

int Location::set_root(const std::string& value)
{
    root_ = value;
    return 0;
}

int Location::set_default_file(const std::string& value)
{
    default_file_ = value;
    return 0;
}

int Location::set_cgi(const std::string& value)
{
    if (value != "php" && value != "python" && value != "none") {
        std::cerr << "cgi value should be php, python or none" << std::endl;
        return 1;
    }
    cgi_ = value;
    return 0;
}

int Location::set_proxy(const std::string& value)
{
    proxy_ = value;
    return 0;
}

int Location::set_methods(const std::string& value)
{
    std::string::size_type  beg = 0;
    std::string::size_type  end = value.find(' ');
    methods_ = 0;
    while (true) {
        std::string str = value.substr(beg, end - beg);
        if (str == "GET" && (methods_ & kGet) == 0) {
            methods_ |= kGet;
        } else if (str == "POST" && (methods_ & kPost) == 0) {
            methods_ |= kPost;
        } else if (str == "DELETE" && (methods_ & kDelete) == 0) {
            methods_ |= kDelete;
        } else {
            std::cerr << "method should be GET POST or DELETE" << std::endl;
            return 1;
        }
        if (end == std::string::npos)
            break;
        beg = end + 1;
        end = value.find(' ', beg);
    }
    return 0;
}

int Location::set_listing(const std::string& value)
{
    if (value == "true") {
        listing_ = true;
    } else if (value =="false") {
        listing_ = false;
    } else {
        std::cerr << "Listing value should be true or false" << std::endl;
        return 1;
    }
    return 0;
}

int Location::SetValue(const std::string& key, const std::string& value)
{
    typedef std::map<const std::string, int (Location::*)(const std::string&)>::const_iterator it;

    it i = set_functions_.find(key);
    if (i == set_functions_.end()) {
        std::cerr << "setting key does not exist: " << key << std::endl;
        return 1;
    }
    return (this->*(i->second))(value);
}

void    Location::Print() const
{
    std::cout   << "\tlocation: " << path_ << std::endl
                << "\troot: " << root_ << std::endl
                << "\tdefault_file: " << default_file_ << std::endl
                << "\tcgi: " << cgi_ << std::endl
                << "\tmethods: " << methods_ << std::endl
                << "\tproxy: " << proxy_ << std::endl
                << "\tlisting: " << listing_ << std::endl;
}

const std::map<const std::string, int(Location::*)(const std::string&)> Location::InitSetFunctions()
{
    std::map<const std::string, int(Location::*)(const std::string&)>   m;
    m["root"] = &Location::set_root;
    m["default_file"] = &Location::set_default_file;
    m["cgi"] = &Location::set_cgi;
    m["methods"] = &Location::set_methods;
    m["proxy"] = &Location::set_proxy;
    m["listing"] = &Location::set_listing;
    return m;
}
