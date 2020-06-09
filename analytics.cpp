#include <cstdio>
#include <cstring>
#include <algorithm>
#include <string>
#include <map>

#include "config.hpp"
#include "lib/cgic/cgic.h"
#include "lib/mysql/MySQL.hpp"

#include "lib/ipdb/rapidjson/document.h"
#include "lib/ipdb/rapidjson/writer.h"
#include "lib/ipdb/rapidjson/stringbuffer.h"

using namespace rapidjson;

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

int cgiMain()
{
    fprintf(cgiOut, "Access-Control-Allow-Origin: *\n");
    cgiHeaderContentType("text/json");

    std::map<std::string, std::string> queryString = parseQueryString((std::string)cgiQueryString);

    if (queryString.count("domain") == 0)
    {
        StringBuffer strBuf;
        Writer<StringBuffer> writer(strBuf);

        writer.StartObject();
        writer.Key("code");
        writer.Int(400);
        writer.Key("status");
        writer.String("failed");
        writer.Key("info");
        writer.String("domain field is required.");
        writer.EndObject();

        std::string output = strBuf.GetString();
        fprintf(cgiOut, "%s", output.c_str());

        return 0;
    }

    std::string domain = queryString["domain"];
    
    MySQL db(MYSQL_HOST, MYSQL_PORT, MYSQL_USER, MYSQL_PASS, "");
    db.connect();
    db.useDatabase("analytics");

    MySQLResult res;
    res = db.prepare()
            ->select({"*"})
            ->from({"page"})
            ->where({ Tuple("domain", domain) })
            ->exec();
    
    std::map<std::string, int> browser;
    std::map<std::string, int> system;
    std::map<std::string, int> region;
    std::map<std::string, int> language;
    std::map<std::string, int> url;
    std::map<std::string, int> ip;

    StringBuffer strBuf;
    Writer<StringBuffer> writer(strBuf);
    writer.StartObject();
    writer.Key("rawData");
    writer.StartArray();
    for (auto row : res.result)
    {
        writer.StartObject();
        if (ip.count(row["ip_address"]) == 0)
        {
            ip[row["ip_address"]]++;
            std::string currentRegion = row["country"];
            if (row["province"] != row["country"])
                currentRegion += row["province"];
            if (row["city"] != row["province"])
                currentRegion += row["city"];

            region[currentRegion]++;
        }

        for (auto item : row)
        {
            writer.Key(item.first.c_str());
            writer.String(item.second.c_str());

            if (item.first == "user_agent")
            {
                std::string agent = item.second;
                std::transform(agent.begin(), agent.end(), agent.begin(), ::tolower);
                if (agent.find("chrome") != std::string::npos)
                    browser["Chrome"]++;
                else if (agent.find("safari") != std::string::npos)
                    browser["Safari"]++;
                else if (agent.find("firefox") != std::string::npos)
                    browser["Firefox"]++;
                else if (agent.find("msie") != std::string::npos)
                    browser["IE"]++;
                else if (agent.find("edge") != std::string::npos)
                    browser["Edge"]++;
                else
                    browser["Other"]++;

                if (agent.find("windows") != std::string::npos)
                    system["Windows"]++;
                else if (agent.find("macintosh") != std::string::npos)
                    system["macOS"]++;
                else if (agent.find("linux") != std::string::npos)
                    system["Linux"]++;
                else if (agent.find("ios") != std::string::npos)
                    system["iOS"]++;
                else if (agent.find("android") != std::string::npos)
                    system["Android"]++;
                else
                    system["Other"]++;
            }

            if (item.first == "language") {
                std::string lang = item.second;
                std::transform(lang.begin(), lang.end(), lang.begin(), ::tolower);
                language[lang]++;
            }

            if (item.first == "page_url") {
                std::string currentUrl = item.second;
                std::transform(currentUrl.begin(), currentUrl.end(), currentUrl.begin(), ::tolower);
                url[currentUrl]++;
            }
        }
        writer.EndObject();
    }
    writer.EndArray();

    writer.Key("browser");
    writer.StartObject();
    for (auto brw : browser)
    {
        writer.Key(brw.first.c_str());
        writer.Int(brw.second);
    }
    writer.EndObject();

    writer.Key("system");
    writer.StartObject();
    for (auto item : system)
    {
        writer.Key(item.first.c_str());
        writer.Int(item.second);
    }
    writer.EndObject();

    writer.Key("region");
    writer.StartObject();
    for (auto item : region)
    {
        writer.Key(item.first.c_str());
        writer.Int(item.second);
    }
    writer.EndObject();    

    writer.Key("language");
    writer.StartObject();
    for (auto item : language)
    {
        writer.Key(item.first.c_str());
        writer.Int(item.second);
    }
    writer.EndObject();    

    writer.Key("pages");
    writer.StartObject();
    for (auto item : url)
    {
        writer.Key(item.first.c_str());
        writer.Int(item.second);
    }
    writer.EndObject();

    writer.EndObject();

    std::string output = strBuf.GetString();
    fprintf(cgiOut, "%s", output.c_str());

    return 0;
}
