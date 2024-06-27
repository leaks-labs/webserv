#include "Location.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

const std::map<const std::string, void (Location::*)(const std::string&)>   Location::set_functions_ = Location::InitSetFunctions();
const std::map<const std::string, int>                                      Location::methods_ref_ = Location::InitMethodsRef();
const std::map<const std::string, int>                                      Location::cgi_ref_ = Location::InitCgiRef();
int    errorList[] = {400, 404};

Location::Location()
    : path_("/"),
      root_("/"), // TODO: change to current directory?
      default_file_("/data/default.html"),
      proxy_("false"),
      cgi_(kCgiPHP),
      methods_(kMethodGet | kMethodPost | kMethodDelete),
      bodymax_(0),
      listing_(true),
      strict_(false)
{
    for (unsigned long int i = 0; i < sizeof(errorList) / sizeof(errorList[0]); i++)
        errors_[errorList[i]] = "index.html";
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

const std::string&  Location::get_proxy() const
{
    return proxy_;
}

const std::map<int, std::string>&  Location::get_errors() const
{
    return errors_;
}

int Location::get_cgi() const
{
    return cgi_;
}

int Location::get_methods() const
{
    return methods_;
}

int Location::get_bodymax() const
{
    return bodymax_;
}

bool    Location::get_listing() const
{
    return listing_;
}

bool    Location::get_strict() const
{
    return strict_;
}

void    Location::set_path(const std::string& value)
{
    if (value.size() == 0 || value[0] != '/')
        throw std::runtime_error("path should start with /");
    path_ = value;

}

void    Location::set_root(const std::string& value)
{
    if (value.size() == 0 || value[0] != '/')
        throw std::runtime_error("root should start with /");
    root_ = value;
}

void    Location::set_default_file(const std::string& value)
{
    if (value.find("/") != value.npos)
        throw std::runtime_error("default file should not contain a /");
    default_file_ = value;
}

void    Location::set_proxy(const std::string& value)
{
    proxy_ = value;
}

void    Location::set_errors(const std::string& value)
{
    std::string::size_type      start = 0;
    std::string::size_type      end;
    std::string                 res;
    std::vector<std::string>    lst;
    int                         code;

    do {
        end = value.find(' ', start);
        res = value.substr(start, end - start);
        if (res.empty())
            break;
        lst.push_back(res);
        start = end + 1;
    } while (end != std::string::npos);
    if (end != std::string::npos || res.empty() || lst.size() < 2)
        throw std::runtime_error("errors are invalid");
    for (long unsigned int i = 0; i < lst.size() - 1; i++)
    {
        std::istringstream  iss(lst[i]);
        iss >> std::noskipws >> code;
        if (iss.fail() || !iss.eof() || code < 0)
            throw std::runtime_error("error code should be a positive integer");
        errors_[code] = lst.back();
    }
}

void    Location::set_cgi(const std::string& value)
{
    cgi_ = 0;
    std::string::size_type  start = 0;
    std::string::size_type  end;
    do {
        end = value.find(' ', start);
        std::string res = value.substr(start, end - start);
        std::map<const std::string, int>::const_iterator    i = cgi_ref_.find(res);
        if (i == cgi_ref_.end())
            throw std::runtime_error("cgi is invalid: it should be php, python or none");
        else if (i->second == kCgiNone)
            cgi_ = kCgiNone;
        else
            cgi_ |= i->second;
        start = end + 1;
    } while (end != std::string::npos);
}

void    Location::set_methods(const std::string& value)
{
    methods_ = 0;
    std::string::size_type  start = 0;
    std::string::size_type  end;
    do {
        end = value.find(' ', start);
        std::string res = value.substr(start, end - start);
        std::map<const std::string, int>::const_iterator    i = methods_ref_.find(res);
        if (i == methods_ref_.end())
            throw std::runtime_error("method is invalid: it should be GET POST DELETE or none");
        else if (i->second == kMethodNone)
            methods_ = kMethodNone;
        else
            methods_ |= i->second;
        start = end + 1;
    } while (end != std::string::npos);
}

void    Location::set_bodymax(const std::string& value)
{
    std::istringstream  iss(value);
    iss >> std::noskipws >> bodymax_;
    if (iss.fail() || !iss.eof() || (value[0] == '0' && bodymax_ != 0))
        throw std::runtime_error("bodymax value shoud be a digit");
}

void    Location::set_listing(const std::string& value)
{
    if (value == "true")
        listing_ = true;
    else if (value =="false")
        listing_ = false;
    else
        throw std::runtime_error("listing value should be true or false");
}

void    Location::set_strict(bool value)
{
   strict_ = value;
}

int Location::SetValue(const std::string& key, const std::string& value)
{
    typedef std::map<const std::string, void (Location::*)(const std::string&)>::const_iterator it;

    it i = set_functions_.find(key);
    if (i == set_functions_.end())
        return kInvalidKey;
    (this->*(i->second))(value);
    return kValidKey;
}

void    Location::Print() const
{
    typedef std::map<int, std::string>::const_iterator iterator;

    std::cout   << "\tlocation: " << path_ << std::endl
                << "\troot: " << root_ << std::endl
                << "\tdefault_file: " << default_file_ << std::endl
                << "\tcgi: " << cgi_ << std::endl
                << "\tmethods: " << methods_ << std::endl
                << "\tproxy: " << proxy_ << std::endl
                << "\tlisting: " << listing_ << std::endl
                << "\tstrict: " << strict_ << std::endl
                << "\tbodymax: " << bodymax_ << std::endl
                << "\terrors: " << std::endl;

    for (iterator i = errors_.begin(); i != errors_.end(); i++)
    {
        std::cout << "\t\t" << i->first << ": " << i->second << std::endl;
    }
}

const std::map<const std::string, void (Location::*)(const std::string&)>   Location::InitSetFunctions()
{
    std::map<const std::string, void (Location::*)(const std::string&)> m;
    m["root"] = &Location::set_root;
    m["default_file"] = &Location::set_default_file;
    m["proxy"] = &Location::set_proxy;
    m["errors"] = &Location::set_errors;
    m["cgi"] = &Location::set_cgi;
    m["methods"] = &Location::set_methods;
    m["bodymax"] = &Location::set_bodymax;
    m["listing"] = &Location::set_listing;
    return m;
}

const std::map<const std::string, int>  Location::InitMethodsRef()
{
    std::map<const std::string, int>    m;
    m["none"] = kMethodNone;
    m["GET"] = kMethodGet;
    m["POST"] = kMethodPost;
    m["DELETE"] = kMethodDelete;
    return m;
}

const std::map<const std::string, int>  Location::InitCgiRef()
{
    std::map<const std::string, int>    m;
    m["none"] = kCgiNone;
    m["php"] = kCgiPHP;
    m["python"] = kCgiPython;
    return m;
}
