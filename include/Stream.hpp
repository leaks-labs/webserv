#ifndef STREAM_HPP_
# define STREAM_HPP_

# include <cstddef>
# include <vector>

class Stream {
    public:
        Stream(int sfd);

        ~Stream();

        int         get_sfd() const;
        std::size_t get_request_count() const;

        void    Send();
        void    Read();

    private:
        static const int    kBufSize = 1024;

        Stream();
        Stream(const Stream& src);
        Stream& operator=(const Stream& rhs);
    
    const int           sfd_;
    std::size_t         request_count;
    std::vector<char>   buffer_;
};

#endif  // STREAM_HPP_
