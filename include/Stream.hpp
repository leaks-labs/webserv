#ifndef STREAM_HPP_
# define STREAM_HPP_

# include <cstddef>
# include <vector>
# include <string>

class Stream {
    public:
        Stream(int acceptor_sfd, int sfd);

        ~Stream();

        int         get_sfd() const;
        std::size_t get_request_count() const;

        void    Send();
        void    Read();
        std::string AddHeader(std::string const & str) const;

    private:
        static const int    kBufSize = 1024;

        Stream();
        Stream(const Stream& src);
        Stream& operator=(const Stream& rhs);
    
        const int           acceptor_sfd_;
        const int           sfd_;
        int                 pfd_[2];
        std::size_t         request_count;
        std::vector<char>   buffer_;
};

#endif  // STREAM_HPP_
