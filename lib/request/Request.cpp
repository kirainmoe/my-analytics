#ifndef __request_cpp
#define __request_cpp

#include "Request.h"

/**
 * 向指定地址发送 HTTP GET 请求
 * @param std::string url 请求地址
 * @param std::string 返回请求结果
 */
std::string Request::get(std::string url)
{
    std::string body;
    CURL *handle = curl_easy_init();

    if (handle)
    {
        curl_easy_setopt(handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
        curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);

        auto receive = [](char *buf, size_t size, size_t count, void *data) {
            (*static_cast<std::string *>(data)) += std::string(buf, count);
            return size * count;
        };
        
        typedef size_t (*WriteFunction)(char *, size_t, size_t, void *);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, static_cast<WriteFunction>(receive));
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &body);

        curl_easy_perform(handle);
        curl_easy_cleanup(handle);
    }

    return body;
}

#endif