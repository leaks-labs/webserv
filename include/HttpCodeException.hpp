#ifndef HTTP_CODE_EXCEPTION_HPP_
# define HTTP_CODE_EXCEPTION_HPP_

#include <exception>
#include <string>

class HttpCodeException : public std::exception {
    public:
        HttpCodeException() {}
        virtual ~HttpCodeException() throw() {}

        virtual int         Code() const throw() = 0;
        virtual const char* Reason() const throw() = 0;
        virtual const char* CodeStr() const throw() = 0;
};

namespace HttpCodeExceptions
{

class OkException : public HttpCodeException {
    public:
        OkException() {}
        virtual ~OkException() throw() {}

        virtual int Code() const throw() {
            return 200;
        }
        virtual const char* Reason() const throw() {
            return "OK";
        }
        virtual const char* CodeStr() const throw() {
            return "200";
        }
};

class CreatedException : public HttpCodeException {
    public:
        CreatedException() {}
        virtual ~CreatedException() throw() {}

        virtual int Code() const throw() {
            return 201;
        }
        virtual const char* Reason() const throw() {
            return "Created";
        }
        virtual const char* CodeStr() const throw() {
            return "201";
        }
};

class NoContentException : public HttpCodeException {
    public:
        NoContentException() {}
        virtual ~NoContentException() throw() {}

        virtual int Code() const throw() {
            return 204;
        }
        virtual const char* Reason() const throw() {
            return "No Content";
        }
        virtual const char* CodeStr() const throw() {
            return "204";
        }
};

class MovedPermanentlyException : public HttpCodeException {
    public:
        MovedPermanentlyException() {}
        virtual ~MovedPermanentlyException() throw() {}

        virtual int Code() const throw() {
            return 301;
        }
        virtual const char* Reason() const throw() {
            return "Moved Permanently";
        }
        virtual const char* CodeStr() const throw() {
            return "301";
        }
};

class PermanentRedirectException : public HttpCodeException {
    public:
        PermanentRedirectException() {}
        virtual ~PermanentRedirectException() throw() {}

        virtual int Code() const throw() {
            return 308;
        }
        virtual const char* Reason() const throw() {
            return "Permanent Redirect";
        }
        virtual const char* CodeStr() const throw() {
            return "308";
        }
};

class BadRequestException : public HttpCodeException {
    public:
        BadRequestException() {}
        virtual ~BadRequestException() throw() {}

        virtual int Code() const throw() {
            return 400;
        }
        virtual const char* Reason() const throw() {
            return "Bad Request";
        }
        virtual const char* CodeStr() const throw() {
            return "400";
        }
};

class ForbiddenException : public HttpCodeException {
    public:
        ForbiddenException() {}
        virtual ~ForbiddenException() throw() {}

        virtual int Code() const throw() {
            return 403;
        }
        virtual const char* Reason() const throw() {
            return "Forbidden";
        }
        virtual const char* CodeStr() const throw() {
            return "403";
        }
};

class NotFoundException : public HttpCodeException {
    public:
        NotFoundException() {}
        virtual ~NotFoundException() throw() {}

        virtual int Code() const throw() {
            return 404;
        }
        virtual const char* Reason() const throw() {
            return "Not Found";
        }
        virtual const char* CodeStr() const throw() {
            return "404";
        }
};

class MethodNotAllowedException : public HttpCodeException {
    public:
        MethodNotAllowedException() {}
        virtual ~MethodNotAllowedException() throw() {}

        virtual int Code() const throw() {
            return 405;
        }
        virtual const char* Reason() const throw() {
            return "Method Not Allowed";
        }
        virtual const char* CodeStr() const throw() {
            return "405";
        }
};

class RequestTimeoutException : public HttpCodeException {
    public:
        RequestTimeoutException() {}
        virtual ~RequestTimeoutException() throw() {}

        virtual int Code() const throw() {
            return 408;
        }
        virtual const char* Reason() const throw() {
            return "Request Timeout";
        }
        virtual const char* CodeStr() const throw() {
            return "408";
        }
};

class ContentTooLargeException : public HttpCodeException {
    public:
        ContentTooLargeException() {}
        virtual ~ContentTooLargeException() throw() {}

        virtual int Code() const throw() {
            return 413;
        }
        virtual const char* Reason() const throw() {
            return "Content Too Large";
        }
        virtual const char* CodeStr() const throw() {
            return "413";
        }
};

class UrlTooLongException : public HttpCodeException {
    public:
        UrlTooLongException() {}
        virtual ~UrlTooLongException() throw() {}

        virtual int Code() const throw() {
            return 414;
        }
        virtual const char* Reason() const throw() {
            return "URL Too Long";
        }
        virtual const char* CodeStr() const throw() {
            return "414";
        }
};

class RequestHeaderFieldsTooLargeException : public HttpCodeException {
    public:
        RequestHeaderFieldsTooLargeException() {}
        virtual ~RequestHeaderFieldsTooLargeException() throw() {}

        virtual int Code() const throw() {
            return 431;
        }
        virtual const char* Reason() const throw() {
            return "Request Header Fields Too Large";
        }
        virtual const char* CodeStr() const throw() {
            return "431";
        }
};

class InternalServerErrorException : public HttpCodeException {
    public:
        InternalServerErrorException() {}
        virtual ~InternalServerErrorException() throw() {}

        virtual int Code() const throw() {
            return 500;
        }
        virtual const char* Reason() const throw() {
            return "Internal Server Error";
        }
        virtual const char* CodeStr() const throw() {
            return "500";
        }
};

class NotImplementedException : public HttpCodeException {
    public:
        NotImplementedException() {}
        virtual ~NotImplementedException() throw() {}

        virtual int Code() const throw() {
            return 501;
        }
        virtual const char* Reason() const throw() {
            return "Not Implemented";
        }
        virtual const char* CodeStr() const throw() {
            return "501";
        }
};

class BadGatewayException : public HttpCodeException {
    public:
        BadGatewayException() {}
        virtual ~BadGatewayException() throw() {}

        virtual int Code() const throw() {
            return 502;
        }
        virtual const char* Reason() const throw() {
            return "Bad Gateway";
        }
        virtual const char* CodeStr() const throw() {
            return "502";
        }
};

class GatewayTimeoutException : public HttpCodeException {
    public:
        GatewayTimeoutException() {}
        virtual ~GatewayTimeoutException() throw() {}

        virtual int Code() const throw() {
            return 504;
        }
        virtual const char* Reason() const throw() {
            return "Gateway Timeout";
        }
        virtual const char* CodeStr() const throw() {
            return "504";
        }
};

class HttpVersionNotSupportedException : public HttpCodeException {
    public:
        HttpVersionNotSupportedException() {}
        virtual ~HttpVersionNotSupportedException() throw() {}

        virtual int Code() const throw() {
            return 505;
        }
        virtual const char* Reason() const throw() {
            return "HTTP Version Not Supported";
        }
        virtual const char* CodeStr() const throw() {
            return "505";
        }
};

}

#endif // HTTP_CODE_EXCEPTION_HPP_
