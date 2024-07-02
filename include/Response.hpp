#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include <iostream>
#include "Directory.hpp"
#include <sstream>
# include "AcceptorRecords.hpp"

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
    const Server& FindServer(const int listener_sfd, const std::string& name) const;
};

#endif