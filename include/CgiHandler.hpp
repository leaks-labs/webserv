#ifndef CGI_HANDLER_HPP_
# define CGI_HANDLER_HPP_

# include <utility>

# include "EventHandler.hpp"
# include "EventTypes.hpp"
# include "HttpResponse.hpp"
# include "InitiationDispatcher.hpp"
# include "StreamHandler.hpp"

class StreamHandler;
class HttpResponse;

class CgiHandler : public EventHandler {
    public:
        CgiHandler(StreamHandler& stream_handler, HttpResponse& response);

        virtual ~CgiHandler();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    HandleTimeout();

    private:
        static const int    kContinueCgi = 0;
        static const int    kReturnToStreamHandler = 1;

        CgiHandler();
        CgiHandler(const CgiHandler &src);
        CgiHandler&   operator=(const CgiHandler &rhs);

        std::pair<int, int> InitSocketPair();
        void                ExecCGI();
        void                KillChild();
        int                 ReadFromCGI();
        void                WriteToCGI();
        void                ReturnToStreamHandler();
        int                 CheckBuffer();

        bool                                    error_occured_while_handle_event_;
        StreamHandler&                          stream_handler_;
        HttpResponse&                           response_;
        std::string                             data_to_send_to_cgi_;
        std::pair<int, int>                     sfd_pair_;
        Stream                                  stream_main_;
        Stream                                  stream_child_;
        int                                     should_return_to_stream_handler_;
        pid_t                                   pid_child_;
        std::string                             cgi_buffer;
        InitiationDispatcher::TimeoutIterator   timeout_it_;
};

#endif  // CGI_HANDLER_HPP_
