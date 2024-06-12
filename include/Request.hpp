#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>

class Request
{
    public:
        /* Class handling events */
        Request(const std::string& request_raw);
        Request(const Request& request);
        ~Request();

        /* Getter for request_raw_ */
        const std::string& GetRequestRaw() const;

        /* Getter and setter for request_type_ attribute */
        const std::string& GetRequestType() const;
        void SetRequestType(const std::string& request_type);

    private:
        /* Private class handling event, should not be used */
        Request();
        Request& operator=(const Request& request);

        /* Private attribute for Request class */
        const std::string request_raw_;
        std::string request_type_;
};

#endif