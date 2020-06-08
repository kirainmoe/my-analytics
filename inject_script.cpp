#include <cstdio>
#include <cstring>

#include <iostream>
#include <fstream>
#include <string>

#include "lib/cgic/cgic.h"

int cgiMain()
{
    cgiHeaderContentType("text/plain");

    std::string serverName = (std::string)cgiServerName + ":" + (std::string)cgiServerPort;
    std::string scriptContent = "", tmp;

    std::ifstream infile;
    infile.open("./example.js");

    while (std::getline(infile, tmp))
        scriptContent += tmp;

    scriptContent = scriptContent.replace(scriptContent.find("example.com"), 11, serverName);
    std::cout << scriptContent;

    return 0;
}