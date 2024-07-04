#ifndef STREAM_HPP_
# define STREAM_HPP_

# include <cstddef>
# include <vector>
# include <deque>

# include "HttpMessage.hpp"
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

class Stream {
    public:
        Stream(int sfd);

        ~Stream();

        int         get_sfd() const;
        std::size_t get_request_count() const;

        void    Read();
        void    Decode(const std::vector<char>& buffer);
        void    Execute();
        void    Encode(const HttpRequest& request);
        void    Send();

    private:
        static const int    kBufSize = 1024;

        Stream();
        Stream(const Stream& src);
        Stream& operator=(const Stream& rhs);

        const int           sfd_;
        std::size_t         request_count_;
        std::vector<char>   buffer_;
        std::deque<HttpRequest> request_queue_;
        std::deque<HttpResponse> response_queue_;
};

#endif  // STREAM_HPP_
