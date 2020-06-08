#ifndef __request
#define __request

#include <string>

/* libcurl */
#include <curl/curl.h>

class Request
{
public:
    static std::string get(std::string url);
};

#endif
