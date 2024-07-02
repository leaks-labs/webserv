#include "Response.hpp"

Response::Response(int client_sfd, std::string const & server_name, std::string const & path) :
server_(FindServer(client_sfd, server_name)),
location_(server_.FindLocation(path))
{
    std::stringstream ss;
    std::string content;
    try
    {
        Directory dir(location_->get_root().c_str());
        content = dir.GetHtml();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        content = HTMLPage::GetErrorPage();
    }
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

const Server& Response::FindServer(const int client_sfd, const std::string& name) const
{
    typedef std::vector<const Server *>::iterator Iterator;
    std::vector<const Server *> matched;
    ServerList& server_list = ServerList::Instance();
    std::cout << "client_sfd: " << client_sfd << matched.size() << std::endl;
    for (size_t i = 0; i < server_list.Size(); i++)
        //if(AcceptorRecords::IsSameAddr(client_sfd, server_list[i].get_addr()))
            matched.push_back(&server_list[i]);
    for (Iterator it = matched.begin(); it != matched.end(); it++)
    {
        if((*it)->HasServerName(name))
            return **it;
    }
    return *matched[0];
}