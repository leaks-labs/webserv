#ifndef CGI_HPP_
# define CGI_HPP_

# include <cstddef>
# include <string>
# include <vector>
# include <string>

class Cgi {
    public:

        Cgi();

        Cgi(int sfd[2]);

        Cgi(const Cgi& src);

        Cgi& operator=(const Cgi& rhs);

        ~Cgi();

        int        *get_sfds() const;
        int         get_sfd() const;

        int        Send(std::string& data);
        std::string Read();

    private:
        static const int    kBufSize = 1024;
        int                 sfds_[2];
        std::vector<char>   buffer_;
        bool                reading_;
};

#endif  // CGI_HPP_
