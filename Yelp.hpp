#include <memory>
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <set>
#include <string>
#include <vector>

#pragma db view
class StarCount{
public:
	int stars;
	int count;
};

#pragma db view query("select top 1 text, last_elapsed_time from sys.dm_exec_query_stats cross apply sys.dm_exec_sql_text(sql_handle) order by last_execution_time desc")
class LastQueryTime{
public:
	std::string text;
	long elapsed_time;
};

// ---------------------------------------------
// No need to change anything above this line
// ---------------------------------------------

class user;
class business;
class hours;
class review;

#pragma db object table("business")
class business {
public:
private:
    friend class odb::access;
    business() {}
#pragma db id auto
	std::string id;
    std::string name;
    odb::lazy_weak_ptr<review> review_;
    odb::lazy_weak_ptr<hours> hours_;


};

#pragma db object table("hours")
class hours {
public:
private:
    friend class odb::access;
    hours() {}
#pragma db id auto
    unsigned long id;
    std::string hours_;
    std::shared_ptr<business> business_id;
    //business foreign key
};

#pragma db object table("review")
class review {
public:
private:
    friend class odb::access;
    review() {}
#pragma db id auto
    std::string id;
    std::shared_ptr<user> user_id;
    std::shared_ptr<business> business_id;
    // user foreign key
};

#pragma db object table("user")
class user {
public:
    std::string get_name() {
        return this->name;
    }
private:
    friend class odb::access;
    user() {}
#pragma db id auto
    std::string id;
    std::string name;
#pragma db inverse(user_id)
    std::vector<odb::lazy_weak_ptr<review>> review_;
};

