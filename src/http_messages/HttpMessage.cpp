#include "HttpMessage.hpp"

const MessageStatus status_dictionary[N_STATUS] = {
        {"OK", 200},
        {"CREATED", 201},
        {"NO CONTENT", 204},
        {"Moved Permanently", 301},
        {"Found", 302},
        {"Not Modified", 304},
        {"Bad Request", 400},
        {"Unauthorized", 401},
        {"Forbidden", 403},
        {"Not Found", 404}
};

HttpMessage::HttpMessage()
        : is_chunk_(false), is_complete_(false)
{
}

HttpMessage::~HttpMessage()
{
}

const HttpRequestLine& HttpMessage::get_request_line() const
{
    return request_line_;
}

HttpRequestLine& HttpMessage::get_protected_request_line()
{
    return request_line_;
}

const HttpHeader& HttpMessage::get_message_header() const
{
    return message_header_;
}

HttpHeader& HttpMessage::get_protected_message_header()
{
    return message_header_;
}

const MessageStatus& HttpMessage::get_message_status() const
{
    return message_status_;
}

void HttpMessage::set_message_status(MessageStatus &message_status)
{
    message_status_ = message_status;
}

const std::string& HttpMessage::get_message() const
{
    return message_;
}

void HttpMessage::set_message(const std::string& message)
{
    message_ = message;
}

const std::string& HttpMessage::get_body() const
{
    return body_;
}

void HttpMessage::set_body(const std::string& body)
{
    body_ = body;
}

bool HttpMessage::get_is_chunck() const
{
    return is_chunk_;
}

void HttpMessage::set_is_chunk(bool is_chunk)
{
    is_chunk_ = is_chunk;
}

bool HttpMessage::get_is_complete() const
{
    return is_complete_;
}

void HttpMessage::set_is_complete(bool is_complete)
{
    is_complete_ = is_complete;
}

size_t HttpMessage::FindStatus(const MessageStatus *dictionary, int status)
{
    size_t i;
    for (i = 0; i < N_STATUS; ++i) {
        if (dictionary[i].status == status)
            break;
    }
    return i;
}