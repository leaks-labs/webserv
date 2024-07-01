#ifndef WEBSERV_HPP_
# define WEBSERV_HPP_

# include <string>
# include <vector>

<<<<<<< HEAD
# include "ListenerList.hpp"
# include "ServerList.hpp"
# include "EventBroker.hpp"
=======
# include "AcceptorRecords.hpp"
>>>>>>> refactor/EventBroker_to_InitiationDispatcher

class WebServ {
    public:
        static const std::string    kDefaultConfigFile;

        WebServ(const std::string& config_file);

        ~WebServ();

        void    Run() const;

    private:
        WebServ();
        WebServ(const WebServ& src);
<<<<<<< HEAD
        WebServ&        operator=(const WebServ& rhs);
        Server          FindServer(const int client_sfd, const std::string& name) const;
        ListenerList    listener_list_;
        ServerList      server_list_;
=======
        WebServ&    operator=(const WebServ& rhs);

        AcceptorRecords acceptor_records_;
>>>>>>> refactor/EventBroker_to_InitiationDispatcher
};

#endif  // WEBSERV_HPP_
