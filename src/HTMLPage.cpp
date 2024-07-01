#include "HTMLPage.hpp"


HTMLPage::HTMLPage() : page_("<!DOCTYPE html>\n")
{
}

void HTMLPage::NewLine()
{
    page_ += "\n";
}

void HTMLPage::Write(std::string str)
{
    page_ += str;
}

void HTMLPage::OpenTag(const std::string & tag)
{
    page_ += "<" + tag + ">";
}

void HTMLPage::CloseTag(const std::string & tag)
{
    page_ += "</" + tag + ">";
}

void HTMLPage::AddLinkTag(const std::string & href, const::std::string & text)
{
    page_ += "<a href=\"" + href + "\">" + text + "</a>";
}

void HTMLPage::AddHeader()
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

void HTMLPage::AddButtom()
{
    CloseTag("body");
    NewLine();
    CloseTag("html");
    NewLine();
}


void HTMLPage::Print () const
{
    std::cout << page_;
}

std::string HTMLPage::GetPage() const
{
    return page_;
}

HTMLPage::~HTMLPage()
{
}
