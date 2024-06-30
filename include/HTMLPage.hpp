#ifndef HTML_PAGE_HPP_
#define HTML_PAGE_HPP_

#include <iostream>

class HTMLPage
{
private:
    std::string page_;
public:
    HTMLPage();
    ~HTMLPage();
    void NewLine();
    void OpenTag(const std::string & tag);
    void CloseTag(const std::string & tag);
    void AddLinkTag(const std::string & href, const::std::string & text);
    void Print() const;
    std::string GetPage() const;
    void Write(std::string);
};

#endif