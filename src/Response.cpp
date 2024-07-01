#include "Response.hpp"

Response::Response()
{
    std::stringstream ss;
    std::string content;
    Directory dir("/");
    dir.WriteHTML();
    content = dir.GetHtml();
    ss << content.size();
    text_ = "HTTP/1.1 200 OK\r\nContent-Length: " + ss.str() + "\r\n\r\n";
    text_ += content;
}

Response::~Response()
{
}

std::string Response::GetText() const
{
    return text_;
}

