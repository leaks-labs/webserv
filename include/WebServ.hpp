#ifndef WEBSERV_HPP_
# define WEBSERV_HPP_

# include <string>
# include <vector>

# include "ListenerList.hpp"
# include "ServerList.hpp"

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

        ListenerList    listener_list_;
        ServerList      server_list_;
};

#endif  // WEBSERV_HPP_
