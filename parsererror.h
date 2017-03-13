#ifndef PARSERERROR_H
#define PARSERERROR_H

#include <string>
#include <exception>

class ParserError : public std::exception
{
private:
    std::string msg;

public:
    ParserError(std::string msg);

    ~ParserError() throw() {}
    virtual const char* what() const throw()
    {
        return msg.c_str();
    }
};
#endif // PARSERERROR_H
