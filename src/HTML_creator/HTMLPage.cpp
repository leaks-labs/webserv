#include "HTMLPage.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "HttpStatusLine.hpp"

std::string HTMLPage::GetErrorPage(int code)
{
     HTMLPage page;
    const std::map<int, std::string>::const_iterator it = HttpStatusLine::get_status_code_map().find(code);

    const std::string error_message = (it == HttpStatusLine::get_status_code_map().end())
    ? "Sorry, something went wrong."
    : it->second;

    const std::string place_holders[2] = {
        "{{ERROR_CODE}}",
        "{{ERROR_MESSAGE}}"
    };
    const std::ifstream ifs("data/error.html");
    if (!ifs.good())
        std::cout << "Failled to open the file" << std::endl;
    std::ostringstream oss, to_str;
    oss << ifs.rdbuf();
    to_str << code;
    const std::string logs[2] = {
        to_str.str(),
        error_message
    };
    std::string error_html = oss.str();
    for (size_t i = 0; i < 2; ++i) {
        size_t pos = error_html.find(place_holders[i]);
        error_html.replace(pos, place_holders[i].length(), logs[i]);
    }
    page.Write(error_html);
    return (page.get_page());
}

HTMLPage::HTMLPage()
    : page_("<!DOCTYPE html>\n")
{
}

HTMLPage::~HTMLPage()
{
}

std::string HTMLPage::get_page() const
{
    return page_;
}

void    HTMLPage::NewLine()
{
    page_ += "\n";
}

void    HTMLPage::Write(const std::string& str)
{
    page_ += str;
}

void    HTMLPage::OpenTag(const std::string& tag)
{
    page_ += "<" + tag + ">";
}

void    HTMLPage::CloseTag(const std::string& tag)
{
    page_ += "</" + tag + ">";
}

void    HTMLPage::AddLinkTag(const std::string& href, const std::string& text)
{
    page_ += "<a href=\"" + href + "\">" + text + "</a>";
}

void    HTMLPage::AddHeader()
{
    OpenTag("head");
    Write("<meta charset=\"UTF-8\">");
    CloseTag("head");
    NewLine();
    OpenTag("html");
    NewLine();
    OpenTag("body");
    NewLine();
}

void    HTMLPage::AddButtom()
{
    CloseTag("body");
    NewLine();
    CloseTag("html");
    NewLine();
}


void    HTMLPage::Print () const
{
    std::cout << page_;
}
