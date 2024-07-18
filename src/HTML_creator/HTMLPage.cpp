#include "HTMLPage.hpp"

#include <sstream>

// #include "HttpRequest.hpp"
#include "HttpStatusLine.hpp"

// TODO: remove this
#include <iostream>
// TODO: remove this

std::string HTMLPage::GetErrorPage(int code)
{
    HTMLPage page;

    page.AddHeader();
    page.OpenTag("h1");
    std::map<int, std::string>::const_iterator it = HttpStatusLine::get_status_code_map().find(code);
    std::ostringstream iss;
    iss << code;
    std::string code_str = iss.str();
    std::string str;
    if (it == HttpStatusLine::get_status_code_map().end())
        str = "Error " + code_str + " : unkown problem";
    else
        str = "Error " + code_str + " : " + it->second;
    page.Write(str);
    page.CloseTag("h1");
    page.NewLine();
    page.AddButtom();
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
