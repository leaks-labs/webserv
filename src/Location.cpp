#include "Location.hpp"

Location::Location() :
    path("/"),
    root("/"),
    default_file("/data/default.html"),
    cgi("php"),
    methods(7),
    proxy("false"),
    listing(true)
{
    initSetFunctions();
};

Location::Location(const Location& src) :
    path(src.getPath()),
    root(src.getRoot()),
    default_file(src.getDefaulFile()),
    cgi(src.getCgi()),
    methods(src.getMethods()),
    proxy(src.getProxy()),
    listing(src.getListing())
{
    initSetFunctions();
}

Location::~Location(){};

Location&   Location::operator=(Location const & rhs)
{
    if (this != &rhs)
        *this = rhs;
    return *this;
}

void Location::initSetFunctions()
{
    try
    {
        set_functions.insert(std::make_pair("root", &Location::setRoot));
        set_functions.insert(std::make_pair("default_file", &Location::setDefaultFile));
        set_functions.insert(std::make_pair("cgi", &Location::setCgi));
        set_functions.insert(std::make_pair("methods", &Location::setMethods));
        set_functions.insert(std::make_pair("proxy", &Location::setProxy));
        set_functions.insert(std::make_pair("listing", &Location::setListing));
    }
    catch(const std::exception& e)
    {
        throw(e.what());
    }
    
}


int Location::setValue(std::string key, std::string value)
{
    typedef std::map<std::string, int (Location::*)(std::string)>::iterator it;

    it i = set_functions.find(key);
    if(i == set_functions.end())
    {
        std::cerr << "setting key does not exist: " << key << std::endl;
        return(1);
    }
    return(this->*(i->second))(value);
}

int Location::setPath(std::string value)
{
    path = value;
    return(0);
}

std::string const & Location::getPath() const
{
    return (path);
}

int Location::setRoot(std::string value)
{
    root = value;
    return(0);
}

std::string const & Location::getRoot() const
{
    return (root);
}

int Location::setDefaultFile(std::string value)
{
    default_file = value;
    return(0);
}

std::string const & Location::getDefaulFile() const
{
    return (default_file);
}

int Location::setCgi(std::string value)
{
    if(value.compare("php") && value.compare("python") && value.compare("none"))
    {
        std::cerr << "cgi value should be php, python or none" << std::endl;
        return (1);
    }
    cgi = value;
    return(0);
}

std::string const & Location::getCgi() const
{
    return (cgi);
}

int Location::setMethods(std::string value)
{
    std::string::size_type beg;
    std::string::size_type end;
    std::string str;

    beg = 0;
    end = value.find(' ');
    methods = 0;
    while (1)
    {
        str = value.substr(beg, end - beg);
        if(str == "GET" && (methods & GET) == 0)
            methods += 1;
        else if(str == "POST" && (methods & POST) == 0)
            methods += 2;
        else if(str == "DELETE" && (methods & DELETE) == 0)
            methods += 4;
        else
        {
            std::cerr << "method should be GET POST or DELETE" << std::endl;
            return(1);
        }
        if(end == std::string::npos)
            break;
        beg = end + 1;
        end = value.find(' ', beg);
    }
    return(0);
}

int Location::getMethods() const
{
    return (methods);
}


int Location::setProxy(std::string value)
{
    proxy = value;
    return(0);
}

std::string const & Location::getProxy() const
{
    return (proxy);
}

int Location::setListing(std::string value)
{
    if(!value.compare("true"))
        listing = true;
    else if(!value.compare("false"))
        listing = false;
    else
    {
        std::cerr << "Listing value should be true or false" << std::endl;
        return(1);
    }
    return(0);
}

bool Location::getListing() const
{
    return (listing);
}


void Location::print()const
{
    std::cout   << "\tlocation: " << path << std::endl
                << "\troot: " << root << std::endl
                << "\tdefault_file: " << default_file << std::endl
                << "\tcgi: " << cgi << std::endl
                << "\tmethods: " << methods << std::endl
                << "\tproxy: " << proxy << std::endl
                << "\tlisting: " << listing << std::endl;
}