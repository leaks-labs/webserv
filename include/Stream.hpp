#ifndef STREAM_HPP_
# define STREAM_HPP_

# include <cstddef>
# include <string>
# include <vector>
# include <string>
# include <deque>


class Stream {
    public:
        Stream(int sfd);
        Stream(const Stream& src);
        Stream& operator=(const Stream& rhs);
        ~Stream();

        int         get_sfd() const;

        void        Send(std::string& data);
        std::string Read();

    private:
        static const int    kBufSize = 1024;

        Stream();
    
        int           sfd_;
        std::vector<char>   buffer_;
};

#endif  // STREAM_HPP_
