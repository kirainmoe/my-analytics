#include <cstdio>
#include <cstring>
#include <map>

#include "config.hpp"
#include "lib/mysql/MySQL.hpp"

#include "lib/cgic/cgic.h"
#include "lib/ipdb/ipdb.h"

struct Region
{
    std::string country, province, city;
    Region(std::string _c, std::string _p, std::string _ct)
        :country(_c), province(_p), city(_ct) {}

    void print()
    {
        std::cout << "Country: " << country << std::endl;
        std::cout << "Province: " << province << std::endl;
        std::cout << "City: " << city << std::endl;
    }
};


std::map<std::string, std::string> parseQueryString(std::string queryString)
{
    std::map<std::string, std::string> result;
    std::string key = "", value = "";
    int length = queryString.length();
    bool flag = 0;
    for (int i = 0; i < length; i++)
    {
        if (queryString[i] == '=' && !flag)
        {
            flag = 1;
            continue;
        }
        if (queryString[i] == '&')
        {
            result[key] = value;
            key = value = "", flag = 0;
            continue;
        }
        if (!flag)
            key += queryString[i];
        else
            value += queryString[i];
    }
    result[key] = value;
    return result;
}

std::string getDomain(std::string url)
{
    int slashCount = 0, length = url.length();
    std::string domain = "";
    for (int i = 0; i < length; i++) {
        if (url[i] == '/')
        {
            if (slashCount < 2)
                slashCount++;
            else
                break;
        }
        else if (slashCount == 2)
            domain += url[i];
    }
    return domain;
}

/**
 * 通过 IP 地址获取地区信息
 * @param std::string ip IP地址
 * @return Region
 */
Region getRegion(std::string ip)
{
    try {
        auto db = std::make_shared<ipdb::City>("./ipipfree.ipdb");

        std::map<std::string, std::string> regionInfo = db->FindMap(ip, "CN");
        return Region(
            regionInfo["country_name"],
            regionInfo["region_name"],
            regionInfo["city_name"]         
        );
    } catch (const char *e) {
        return Region(
            "未知",
            "未知",
            "未知"
        );
    }
}

/**
 * 将访问记录添加到数据库
 */
void addVisitRecord(std::string ip,
                    std::string userAgent,
                    std::string url,
                    std::string language,
                    std::string referer,
                    Region region)
{
    MySQL db(MYSQL_HOST, MYSQL_PORT, MYSQL_USER, MYSQL_PASS, "");
    db.connect();
    db.useDatabase("analytics");
    
    std::string domain = getDomain(url);

    db.prepare()
        ->insert("page")
        ->values({
            Tuple("domain", domain),
            Tuple("page_url", url),
            Tuple("ip_address", ip),
            Tuple("country", region.country),
            Tuple("user_agent", userAgent),
            Tuple("province", region.province),
            Tuple("city", region.city),
            Tuple("language", language),
            Tuple("referer", referer)
        })
        ->exec();
}

int cgiMain()
{
    std::string remoteAddr = cgiRemoteAddr,
            userAgent = cgiUserAgent,
            referer = cgiReferer;

    fprintf(cgiOut, "Access-Control-Allow-Origin: *\n");
	cgiHeaderContentType("text/json");

    Region currentRegion = getRegion(remoteAddr);

    std::map<std::string, std::string> queryString = parseQueryString(cgiQueryString);
    addVisitRecord(remoteAddr,
                   userAgent,
                   referer,
                   queryString["lang"],
                   queryString["referer"],
                   currentRegion);

    fprintf(cgiOut, "{ \"code\": 200, \"status\": \"ok\", \"userAgent\": \"%s\", \"referer\": \"%s\" }", userAgent.c_str(), queryString["referer"].c_str());

    return 0;
}
