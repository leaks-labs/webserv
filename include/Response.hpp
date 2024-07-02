#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include <iostream>
#include "Directory.hpp"
# include "AcceptorRecords.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class Response
{
private:
    Response();
    std::string text_;
    Response &operator=(const Response &);
    const Server & server_;
    Location const * location_;
public:
    Response(int client_sfd, std::string const & server_name, std::string const & path);
    ~Response();
    std::string GetText() const;
    const Server& FindServer(const int acceptor_sfd, const std::string& name) const;
    std::string Build(std::string path) const;
    std::string BuildPath(std::string) const;
    std::string ReadFile(std::string path) const;
};

#endif