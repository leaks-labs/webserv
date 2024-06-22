#include "Location.hpp"

#include <iostream>
#include <sstream>

const std::map<const std::string, int(Location::*)(const std::string&)> Location::set_functions_ = Location::InitSetFunctions();

Location::Location()
    : path_("/"),
      root_("/"), // TODO: change to current directory?
      default_file_("/data/default.html"),
      cgi_(kCgiPHP),
      proxy_("false"),
      methods_(kMethodGet | kMethodPost | kMethodDelete),
      listing_(true),
      strict_(false),
      errors_("/data/errors"), // TODO: change to a directory inside current directory?
      bodymax_(0)
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
        strict_ = rhs.get_strict();
        errors_ = rhs.get_errors();
        bodymax_ = rhs.get_bodymax();
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

int Location::get_cgi() const
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

bool    Location::get_strict() const
{
    return strict_;
}

const std::string&  Location::get_errors() const
{
    return errors_;
}

int Location::get_bodymax() const
{
    return bodymax_;
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
    cgi_ = 0;
    std::string::size_type  start = 0;
    std::string::size_type  end;
    do {
        end = value.find(' ', start);
        std::string res = value.substr(start, end - start);
        if (res == "none") {
            cgi_ = kCgiNone;
        } else if (res == "php") {
            cgi_ |= kCgiPHP;
        } else if (res == "python") {
            cgi_ |= kCgiPython;
        } else {
            std::cerr << "cgi is invalid: it should be php, python or none" << std::endl;
            return 1;
        }
        start = end + 1;
    } while (end != std::string::npos);
    return 0;
}

int Location::set_proxy(const std::string& value)
{
    proxy_ = value;
    return 0;
}

int Location::set_methods(const std::string& value)
{
    methods_ = 0;
    std::string::size_type  start = 0;
    std::string::size_type  end;
    do {
        end = value.find(' ', start);
        std::string res = value.substr(start, end - start);
        if (res == "none") {
            methods_ = kMethodNone;
        } else if (res == "GET") {
            methods_ |= kMethodGet;
        } else if (res == "POST") {
            methods_ |= kMethodPost;
        } else if (res == "DELETE") {
            methods_ |= kMethodDelete;
        } else {
            std::cerr << "method is invalid: it should be GET POST DELETE or none" << std::endl;
            return 1;
        }
        start = end + 1;
    } while (end != std::string::npos);
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

void    Location::set_strict(bool value)
{
   strict_ = value;
}

int Location::set_errors(const std::string& value)
{
    errors_ = value;
    return 0;
}

int Location::set_bodymax(const std::string& value)
{
    std::istringstream  iss(value);
    iss >> std::noskipws >> bodymax_;
    if (!iss.fail() && iss.eof() && (value[0] != '0' || bodymax_ == 0))
        return 0;
    std::cerr << "bodymax value shoud be a digit" << std::endl;         
    return 1;
}

int Location::SetValue(const std::string& key, const std::string& value)
{
    typedef std::map<const std::string, int (Location::*)(const std::string&)>::const_iterator it;

    it i = set_functions_.find(key);
    if (i == set_functions_.end()) {
        std::cerr << "setting key does not exist: " << key << std::endl;
        return -1;
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
                << "\tlisting: " << listing_ << std::endl
                << "\tstrict: " << strict_ << std::endl
                << "\terrors: " << errors_ << std::endl
                << "\tbodymax: " << bodymax_ << std::endl;
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
    m["errors"] = &Location::set_errors;
    m["bodymax"] = &Location::set_bodymax;
    return m;
}
