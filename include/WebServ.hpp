#ifndef WEBSERV_HPP_
# define WEBSERV_HPP_

# include <string>
# include <vector>
# include "ServerList.hpp"
# include "Listener.hpp"
# include "ListenerList.hpp"

class WebServ {
    public:
        WebServ();
        WebServ(const std::string &config_file);
        ~WebServ();
        int run() const;

    private:
        WebServ(const WebServ& src);
        WebServ&    operator=(const WebServ& rhs);

        ListenerList    listener_list_;
        ServerList      server_list_;
        std::vector<Server*>    servers;
};

#endif  // WEBSERV_HPP_
