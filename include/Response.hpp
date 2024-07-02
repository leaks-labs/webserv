#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include <iostream>
#include "Directory.hpp"
# include "AcceptorRecords.hpp"
#include "CgiPathFinder.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>

class Response
{
private:
    Response();
    std::string text_;
    Response &operator=(const Response &);
    const Server & server_;
    Location const * location_;
public:
    Response(int client_sfd, int client_pfd[2], std::string const & server_name, std::string const & path);
    ~Response();
    std::string GetText() const;
    std::string GetHeader(std::string const & str) const;
    const Server& FindServer(const int acceptor_sfd, const std::string& name) const;
    void Write(const int client_pfd[2], std::string const &path, std::string const &args) const;
    void WriteFile(const int client_pfd, std::string const & body) const;
    void WriteError(const int client_sfd) const;
    std::string BuildPath(std::string const &path) const;
    void CreateFile(const int client_pfd[2], std::string const & path, std::string const &args)const;
    std::string ReadFile(std::string const & path) const;
    std::string FindExtension(std::string const & str) const;
    void ExecCgi(const int client_pfd[2], std::string const & path, std::string const & arg, std::string const & cgi_path) const;
};

#endif