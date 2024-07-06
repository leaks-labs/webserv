#ifndef CGI_HPP_
# define CGI_HPP_

# include <cstddef>
# include <string>
# include <vector>
# include <string>

class Cgi {
    public:
        Cgi(int sfd);

        ~Cgi();

        int         get_sfd() const;

        void        Send(std::string& data);
        std::string Read();

    private:
        static const int    kBufSize = 1024;

        Cgi();
        Cgi(const Cgi& src);
        Cgi& operator=(const Cgi& rhs);
    
        const int           sfd_;
        std::vector<char>   buffer_;
};

#endif  // CGI_HPP_
