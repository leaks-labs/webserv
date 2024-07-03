#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include <iostream>
#include "Directory.hpp"
# include "AcceptorRecords.hpp"
#include "PathFinder.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>

class Response
{
public:
    Response(int client_sfd, int client_pfd[2], std::string const & server_name, std::string const & path);
    ~Response();

    void Get(std::string const &path, std::string const &args) const;
    
private:
    Response();
    Response &operator=(const Response &);

    std::string BuildPath(std::string const &path) const;
    std::string FindExtension(std::string const & str) const;
    bool IsCgiFile(std::string const & path) const;
    std::string GetCgiPath(std::string const & ext) const;

    void GetError(const int Error) const;
    void GetCgi(std::string const & path, std::string const & args) const;
    void GetDirectory(Directory & dir, std::string const & args) const;
    void GetFile(std::string const & path) const;
    void Write(std::string const & str) const;

    const Server & server_;
    Location const * location_;
    int pfd_[2];

};

#endif