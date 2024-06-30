#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include <iostream>
#include "Directory.hpp"
#include <sstream>

class Response
{
private:
    std::string text_;
public:
    Response();
    ~Response();
    std::string GetText() const;
};

#endif