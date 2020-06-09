#include "MySQL.hpp"

#define QUOTE (std::string)"`"
#define SPACE (std::string)" "
#define COMMA (std::string)","
#define EQUAL (std::string)"="
#define DQUOTE (std::string) "\""
#define LBRACKET (std::string) "("
#define RBRACKET (std::string) ")"
#define LIMIT (std::string)"LIMIT"
#define AND   (std::string)"AND"
#define OR    (std::string)"OR"

bool MySQL::connect()
{
    mysql_init(&this->conn);
    if (mysql_real_connect(&this->conn, this->host.c_str(), this->user.c_str(), 
                           this->pass.c_str(), this->db_name.c_str(), this->port, NULL, 0))
        return true;
    fprintf(stderr, "%s", mysql_error(&this->conn));
    return false;
}

bool MySQL::createDatabase()
{
    char* buffer = new char[this->db_name.length() + 10];
    mysql_real_escape_string(&this->conn, buffer, this->db_name.c_str(), this->db_name.length());

    std::string query = "CREATE DATABASE IF NOT EXISTS ";
    query += buffer;
    query += " CHARACTER SET 'utf8' COLLATE 'utf8_general_ci'";

    if (mysql_query(&this->conn, query.c_str()))
        return false;
    return true;
}

void MySQL::setDatabase(std::string name)
{
    this->db_name = name;
}

void MySQL::useDatabase(std::string name)
{
    this->db_name = name;
    std::string query = "USE " + name;
    mysql_query(&this->conn, query.c_str());
    mysql_query(&this->conn, "set names utf8;");
}

std::string MySQL::escape(std::string original)
{
    char* buffer = new char[original.length() + 10];
    mysql_real_escape_string(&this->conn, buffer, original.c_str(), original.length());
    std::string result = buffer;
    return result;
}

MySQL* MySQL::prepare()
{
    operation = "";
    columns = "";
    table = "";
    filter = "";
    limitation = "";

    fields.clear();

    return this;
}

MySQL* MySQL::select(std::vector<std::string> columns)
{
    this->operation = "SELECT";
    unsigned int row_count = columns.size();
    for (unsigned int i = 0; i < row_count; i++)
        if (i == 0)
            this->columns = escape(columns[i]);
        else
            this->columns += COMMA + escape(columns[i]);
    return this;
}

MySQL* MySQL::from(std::vector<std::string> froms)
{
    this->table = "";
    unsigned int froms_count = froms.size();
    for (unsigned int i = 0; i < froms_count; i++)
        if (i == 0)
            this->table = escape(froms[i]);
        else
            this->table += COMMA + escape(froms[i]);
    return this;
}

MySQL* MySQL::where(std::vector<Tuple> where)
{
    this->filter = "";
    unsigned int filter_count = where.size();
    for (unsigned int i = 0; i < filter_count; i++)
        if (i == 0)
            this->filter = SPACE + QUOTE + escape(where[i].key) + QUOTE 
                         + SPACE + EQUAL + SPACE + DQUOTE + escape(where[i].value) + DQUOTE;
        else
            this->filter += SPACE + AND + QUOTE + escape(where[i].key) + QUOTE 
                         + SPACE + EQUAL + SPACE + DQUOTE + escape(where[i].value) + DQUOTE;
    return this;    
}

MySQL* MySQL::orWhere(std::vector<Tuple> where)
{
    unsigned int filter_count = where.size();
    for (unsigned int i = 0; i < filter_count; i++)
        this->filter += SPACE + OR + QUOTE + escape(where[i].key) + QUOTE 
                     + SPACE + EQUAL + SPACE + DQUOTE + escape(where[i].value) + DQUOTE;
    return this;
}

MySQL* MySQL::limit(std::string lim)
{
    this->limitation = escape(lim);
    return this;
}

MySQL* MySQL::insert(std::string table)
{
    this->operation = "INSERT";
    this->table = escape(table);
    return this;
}

MySQL* MySQL::values(std::vector<Tuple> vals)
{
    for (auto i : vals)
    {
        this->fields.push_back(i);
    }
    return this;
}

MySQL* MySQL::deletes(std::string table)
{
    this->operation = "DELETE";
    this->table = escape(table);
    return this;
}

MySQL* MySQL::update(std::string table)
{
    this->operation = "UPDATE";
    this->table = escape(table);
    return this;
}

MySQL* MySQL::set(std::vector<Tuple> vals)
{
    this->values(vals);
    return this;
}

MySQLResult MySQL::exec()
{
    std::string query;

    if (this->operation == "SELECT")
    {
        query = "SELECT" + SPACE + this->columns + SPACE
              + "FROM" + SPACE + this->table + SPACE;
        if (!this->filter.empty())
            query += "WHERE" + SPACE + this->filter + SPACE;
        if (!this->limitation.empty())
            query += "LIMIT" + SPACE + this->limitation;
    }
    if (this->operation == "INSERT")
    {
        query = "INSERT INTO" + SPACE + this->table + SPACE + LBRACKET;

        std::string keys = "";
        std::string values = "";
        
        for (auto i : this->fields)
        {
            if (keys.empty())
                keys += QUOTE + i.key + QUOTE;
            else
                keys += COMMA + QUOTE + i.key + QUOTE;

            if (values.empty())
                values += DQUOTE + escape(i.value) + DQUOTE;
            else
                values += COMMA + DQUOTE + escape(i.value) + DQUOTE;
        }

        query += keys + RBRACKET + SPACE + "VALUES" + SPACE + LBRACKET + values + RBRACKET;
    }
    if (this->operation == "DELETE")
    {
        query = "DELETE" + SPACE + "FROM" +SPACE + this->table + SPACE;
        if (!this->filter.empty())
            query += "WHERE" + SPACE + this->filter + SPACE;
    }
    if (this->operation == "UPDATE")
    {
        query = "UPDATE" + SPACE + this->table + SPACE + "SET" + SPACE;
        std::string setField = "";
        for (auto item : this->fields)
        {
            if (setField.empty())
                setField += escape(item.key) + SPACE + EQUAL + SPACE + escape(item.value);
            else
                setField += COMMA + escape(item.key) + SPACE + EQUAL + SPACE + escape(item.value);
        }
        query += setField + SPACE;
        if (!this->filter.empty())
            query += "WHERE" + SPACE + this->filter + SPACE;
    }
    MySQLResult res;
    res.flag = mysql_query(&this->conn, query.c_str());

    if (this->operation == "SELECT")
    {
        res.result.clear();

        MYSQL_RES *r;
        MYSQL_FIELD *fields;
        MYSQL_ROW row;

        r = mysql_use_result(&this->conn);

        unsigned int fieldCount = 0;
        fieldCount = mysql_num_fields(r);
        fields = mysql_fetch_fields(r);

        std::vector<std::string> fieldName;
        for (int i = 0; i < fieldCount; i++)
        {
            fieldName.push_back((std::string)fields[i].name);
        }

        while ((row = mysql_fetch_row(r)))
        {
            std::map<std::string, std::string> tmp;
            for (int i = 0; i < fieldCount; i++)
                tmp.insert(make_pair(fieldName[i], (std::string)row[i]));
            res.result.push_back(tmp);
        }
    }

    return res;
}

bool MySQL::createTable(std::string tableName, std::vector<Tuple> columns, std::string primaryKey)
{
    std::string query, coltype = "";

    query = "CREATE TABLE IF NOT EXISTS ";
    query += tableName + " (";

    for (auto col : columns)
    {
        if (coltype.empty())
            coltype += QUOTE + col.key + QUOTE + SPACE + col.value;
        else
            coltype += COMMA + QUOTE + col.key + QUOTE + SPACE + col.value;
    }

    if (primaryKey.length())
    {
        coltype += COMMA + "PRIMARY KEY (" + QUOTE + primaryKey + QUOTE + ")";
    }
    query += coltype + ")";

    if (mysql_query(&this->conn, query.c_str()))
        return false;
    return true;
}

void MySQL::query(std::string target)
{
    mysql_query(&this->conn, target.c_str());
}
