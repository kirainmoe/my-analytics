#include <cstdio>
#include <cstring>

#include "lib/cgic/cgic.h"
#include "lib/mysql/MySQL.hpp"

#include "config.hpp"

int cgiMain()
{
    cgiHeaderContentType("text/html");

    // connect to mysql in docker
    MySQL db(MYSQL_HOST, MYSQL_PORT, MYSQL_USER, MYSQL_PASS, "");
    bool connect_result = db.connect();
    // create database
    db.setDatabase("analytics");
    bool create_result = db.createDatabase();
    // use database
    db.useDatabase("analytics");
    // create table
    bool create_table_Result = db.createTable("page", {
        Tuple("id", "INT NOT NULL AUTO_INCREMENT"),
        Tuple("domain", "TEXT"),
        Tuple("page_url", "TEXT"),
        Tuple("ip_address", "VARCHAR(32)"),
        Tuple("country", "TEXT"),
        Tuple("province", "TEXT"),
        Tuple("city", "TEXT"),
        Tuple("language", "TEXT"),
        Tuple("user_agent", "TEXT"),
        Tuple("referer", "TEXT"),
        Tuple("timestamp", "TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP")
    }, "id");

	fprintf(cgiOut, "<html>\n");
    fprintf(cgiOut, "<head>\n");
	fprintf(cgiOut, "<title>Database Initialization</title>\n");
    fprintf(cgiOut, "</head>\n");
	fprintf(cgiOut, "<body>\n");
    fprintf(cgiOut, "<h1>Database connection: %d</h1>", connect_result);
    fprintf(cgiOut, "<h1>Database create: %d</h1>", create_result);
    fprintf(cgiOut, "<h1>Table create: %d</h1>", create_table_Result);
	fprintf(cgiOut, "</body>\n");
    fprintf(cgiOut, "</html>");

    return 0;
}