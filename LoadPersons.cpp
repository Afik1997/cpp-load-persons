struct DbInfo
{
    string_view db_name;
    int db_connection_timeout;
    bool db_allow_exceptions;
    DBLogLevel db_log_level;

    DbInfo& SetDBName(string_view name)
    {
        this->db_name = name;
        return *this;
    }

    DbInfo& SetDBConnectionTimeout(int connection_timeout)
    {
        this->db_connection_timeout = connection_timeout;
        return *this;
    }
    
    DbInfo& SetDBAllowExceptions(bool allow_exceptions)
    {
        this->db_allow_exceptions = allow_exceptions;
        return *this;
    }

    DbInfo& SetDBLogLevel(DBLogLevel log_level)
    {
        this->db_log_level = log_level;
        return *this;
    }
};

struct PersonFilter
{
    int min_age;
    int max_age;
    string_view name_filter;

    PersonFilter& SetMinAge(int min)
    {
        this->min_age = min;
        return *this;
    }

    PersonFilter& SetMaxAge(int max)
    {
        this->max_age = max;
        return *this;
    }

    PersonFilter& SetNameFilter(string_view name)
    {
        this->name_filter = name;
        return *this;
    }
};

vector<Person> LoadPersons(DbInfo db_info, PersonFilter person_filter) 
 {
    DBConnector connector(db_info.db_allow_exceptions, db_info.db_log_level);
    DBHandler db;
    if (db_info.db_name.starts_with("tmp."s)) 
    {
        db = connector.ConnectTmp(db_info.db_name, db_info.db_connection_timeout);
    }
    else {
        db = connector.Connect(db_info.db_name, db_info.db_connection_timeout);
    }
    if (!db_info.db_allow_exceptions && !db.IsOK()) {
        return {};
    }

    ostringstream query_str;
    query_str << "from Persons "s
        << "select Name, Age "s
        << "where Age between "s << person_filter.min_age << " and "s << person_filter.max_age << " "s
        << "and Name like '%"s << db.Quote(person_filter.name_filter) << "%'"s;
    DBQuery query(query_str.str());

    vector<Person> persons;
    for (auto [name, age] : db.LoadRows<string, int>(query)) {
        persons.push_back({ move(name), age });
    }
    return persons;
}
