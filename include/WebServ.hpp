#ifndef WEBSERV_HPP_
# define WEBSERV_HPP_

# include <string>
# include <vector>

# include "AcceptorRecords.hpp"

class WebServ {
    public:
        static const std::string    kDefaultConfigFile;

        WebServ(const std::string& config_file);

        ~WebServ();

        void    Run() const;

    private:
        WebServ();
        WebServ(const WebServ& src);
        WebServ&    operator=(const WebServ& rhs);
        AcceptorRecords acceptor_records_;

};

#endif  // WEBSERV_HPP_
