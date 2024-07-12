#ifndef HTML_PAGE_HPP_
# define HTML_PAGE_HPP_

# include <string>

class HTMLPage {
    public:
        static std::string  GetErrorPage();

        HTMLPage();

        ~HTMLPage();

        std::string get_page() const;

        void    NewLine();
        void    Write(const std::string& str);
        void    OpenTag(const std::string& tag);
        void    CloseTag(const std::string& tag);
        void    AddLinkTag(const std::string& href, const std::string& text);
        void    AddHeader();
        void    AddButtom();
        void    Print() const;

    private:
        HTMLPage(const HTMLPage& src);
        HTMLPage&   operator=(const HTMLPage& rhs);

        std::string page_;
};

#endif  // HTML_PAGE_HPP_
