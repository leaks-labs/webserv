#ifndef WEBSERV_HPP_
# define WEBSERV_HPP_

# include <string>
# include <vector>

# include "Listener.hpp"
# include "ListenerListInfo.hpp"

class WebServ {
    public:
        WebServ();
        WebServ(const std::string &config_file);

        ~WebServ();

        int run() const;

    private:
        WebServ(const WebServ& src);
        WebServ&    operator=(const WebServ& rhs);

        std::vector<Listener*>  listeners_;
        ListenerListInfo        listener_list_info_;
};

#endif  // WEBSERV_HPP_