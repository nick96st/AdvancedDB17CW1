#include "Yelp-odb.hxx"
#include <algorithm>
#include <iostream>
#include <odb/database.hxx>
#include <odb/mssql/database.hxx>
#include <odb/transaction.hxx>

using std::cout;
using std::endl;
using odb::mssql::database;
using odb::transaction;
using odb::query;
using odb::result;
using std::to_string;

typedef odb::query<user> u_query_t;
typedef odb::query<review> r_query_t;
typedef odb::query<business> b_query_t;
typedef odb::query<hours> h_query_t;
typedef odb::result<user> u_result_t;
typedef odb::result<review> r_result_t;
typedef odb::result<hours> h_result_t;

std::vector<std::string> findHours(odb::database& db, std::string username) {
	std::vector<std::string> result;
	transaction t(db.begin());
//    u_query_t user_q(u_query_t::name == username);
//    u_result_t user_objs(db.query(user_q));
//    for (user& i:user_objs)
//    {
//         std::cout<<i.get_name()<<endl;
//    }

	r_query_t get_reviews_with_username(r_query_t::user_id->name == username);
	r_result_t reviews_objs(db.query(get_reviews_with_username));
	for (auto obj:reviews_objs) {
		h_query_t get_hours_with_matching_business_id(h_query_t::business_id->id == obj.get_business_id()->get_id());
		h_result_t hours_objs(db.query(get_hours_with_matching_business_id));
		for (auto obj:hours_objs) {
			result.push_back(obj.get_hours());
		}
	}
	// Your implementation goes here:
	// Find the hours

	t.commit();
	return result;
}

typedef  odb::result<StarCount> sc_result_t;

std::vector<StarCount> countStars(odb::database& db, float latMin, float latMax, float longMin,
																	float longMax) {
	std::vector<StarCount> result;
	transaction t(db.begin());
	// Your implementation goes here:
	sc_result_t results = db.query<StarCount>(std::string("SELECT aceptables.stars, COUNT(*) FROM (SELECT review.stars FROM review,business WHERE review.business_id = business.id AND ") +
													  "business.longitude BETWEEN " + to_string(longMin) + " AND " + to_string(longMax) +
													  "AND business.latitude BETWEEN " +  to_string(latMin) + " AND " + to_string(latMax) + " ) as aceptables " +
													  " GROUP BY aceptables.stars");
	for(auto item : results) {
		result.push_back(item);
	}
	// Count the stars
	t.commit();
	return result;
}

void createIndex(odb::database& db){
	// Your implementation goes here:
	// don't forget to wrap it in a transaction
	// create a columnstore index to accelerate your query
    transaction t(db.begin());
    db.execute("CREATE COLUMNSTORE INDEX countingstars ON review(stars,business_id)");
    db.execute("CREATE COLUMNSTORE INDEX longlat ON business(longitude,latitude) ");
    t.commit();
}

void dropIndex(odb::database& db){
	// Your implementation goes here:
	// don't forget to wrap it in a transaction
	// drop the columnstore index you've created
    transaction t(db.begin());
    db.execute("DROP INDEX review.countingstars");
    db.execute("DROP INDEX business.longlat");
    t.commit();
}

// ---------------------------------------------
// No need to change anything below this line
// ---------------------------------------------

LastQueryTime getLastQueryRuntime(odb::database& db) {
	transaction t(db.begin());
	auto time = db.query_one<LastQueryTime>();
	t.commit();
	return *time;
}

std::vector<std::string> hoursFixtureKn{
		"Friday|0:00-0:00",			 "Friday|0:00-0:00",			"Friday|11:00-1:00",
		"Friday|17:00-1:00",		 "Friday|17:00-1:00",			"Friday|17:30-23:00",
		"Friday|18:00-23:00",		 "Friday|8:00-2:00",			"Friday|9:00-2:00",
		"Monday|0:00-0:00",			 "Monday|0:00-0:00",			"Monday|11:00-23:00",
		"Monday|17:30-23:00",		 "Monday|18:00-23:00",		"Monday|8:00-2:00",
		"Monday|9:00-2:00",			 "Saturday|0:00-0:00",		"Saturday|0:00-0:00",
		"Saturday|11:00-1:00",	 "Saturday|11:30-23:00",	"Saturday|17:00-1:00",
		"Saturday|17:00-1:00",	 "Saturday|18:00-23:00",	"Saturday|8:00-4:00",
		"Saturday|9:00-2:00",		 "Sunday|0:00-0:00",			"Sunday|0:00-0:00",
		"Sunday|11:00-23:00",		 "Sunday|17:00-0:00",			"Sunday|17:00-0:00",
		"Sunday|17:30-23:00",		 "Sunday|18:00-23:00",		"Sunday|8:00-2:00",
		"Sunday|9:00-2:00",			 "Thursday|0:00-0:00",		"Thursday|0:00-0:00",
		"Thursday|11:00-23:00",	"Thursday|17:00-0:00",		"Thursday|17:00-0:00",
		"Thursday|17:30-23:00",	"Thursday|18:00-23:00",	"Thursday|8:00-2:00",
		"Thursday|9:00-2:00",		 "Tuesday|0:00-0:00",			"Tuesday|0:00-0:00",
		"Tuesday|11:00-23:00",	 "Tuesday|17:00-0:00",		"Tuesday|17:00-0:00",
		"Tuesday|17:30-23:00",	 "Tuesday|18:00-23:00",		"Tuesday|8:00-2:00",
		"Tuesday|9:00-2:00",		 "Wednesday|0:00-0:00",		"Wednesday|0:00-0:00",
		"Wednesday|11:00-23:00", "Wednesday|17:00-0:00",	"Wednesday|17:00-0:00",
		"Wednesday|17:30-23:00", "Wednesday|18:00-23:00", "Wednesday|8:00-2:00",
		"Wednesday|9:00-2:00"};

std::vector<std::string> hoursFixtureNeu{
		"Friday|11:30-2:00",		 "Friday|6:00-20:00",		 "Friday|9:00-19:00",		 "Monday|11:30-23:00",
		"Monday|6:00-20:00",		 "Monday|9:00-19:00",		 "Saturday|12:00-20:00", "Saturday|12:00-2:00",
		"Saturday|8:00-16:00",	 "Sunday|12:00-23:00",	 "Thursday|11:30-23:00", "Thursday|6:00-20:00",
		"Thursday|9:00-19:00",	 "Tuesday|11:30-23:00",	"Tuesday|6:00-20:00",	 "Tuesday|9:00-19:00",
		"Wednesday|11:30-23:00", "Wednesday|6:00-20:00", "Wednesday|9:00-19:00"};

std::vector<std::string> hoursFixtureEwr{"Friday|20:30-22:00", "Saturday|20:30-22:00",
																				 "Sunday|20:30-22:00", "Thursday|20:30-22:00"};

std::vector<StarCount> starFixture1{
		{1, 137039}, {2, 111817}, {3, 174317}, {4, 337639}, {5, 410518}};

std::vector<StarCount> starFixture2{{1, 28781}, {2, 19532}, {3, 27541}, {4, 56435}, {5, 83655}};

bool operator==(StarCount const& left, StarCount const& right) {
	return left.stars == right.stars && left.count == right.count;
};

int main(int argc, char** argv) {

	using namespace std;
	database db("SA", "AdvancedDB17", "yelp", "localhost");
//
//	{ // testing find hours
//		auto hours = findHours(db, "kn");
//		std::sort(hours.begin(), hours.end());
//		cout << "findHours Test 1: " << (hours == hoursFixtureKn ? "passed" : "failed") << endl;
//
//		hours = findHours(db, "neu");
//		std::sort(hours.begin(), hours.end());
//		cout << "findHours Test 2: " << (hours == hoursFixtureNeu ? "passed" : "failed") << endl;
//
//		hours = findHours(db, "Ewr");
//		std::sort(hours.begin(), hours.end());
//		cout << "findHours Test 3: " << (hours == hoursFixtureEwr ? "passed" : "failed") << endl;
//	}
//
//	{ // testing countStars
//		auto stars = countStars(db, 30.0, 45.7, -100.0, -1.0);
//		std::sort(stars.begin(), stars.end(),
//							[](auto left, auto right) { return left.stars < right.stars; });
//		cout << "countStars Test 1: " << (stars == starFixture1 ? "passed" : "failed") << endl;
//
//		stars = countStars(db, 4.0, 40., -90.0, -40.0);
//		std::sort(stars.begin(), stars.end(),
//							[](auto left, auto right) { return left.stars < right.stars; });
//		cout << "countStars Test 2: " << (stars == starFixture2 ? "passed" : "failed") << endl;
//	}

	{ // performance runs
		
		// warmup run
//		countStars(db, 30.0, 45.7, -100.0, -1.0);
		for(size_t i = 0; i < 5; i++) {
			countStars(db, 30.0, 45.7, -100.0, -1.0);
			cout << "Run number " << i
					 << " time before indexing: " << getLastQueryRuntime(db).elapsed_time << endl;
		}

		createIndex(db);

		// warmup run
//		countStars(db, 30.0, 45.7, -100.0, -1.0);
		for(size_t i = 0; i < 5; i++) {
			countStars(db, 30.0, 45.7, -100.0, -1.0);
			cout << " Run number " << i << " time after indexing: " << getLastQueryRuntime(db).elapsed_time
					 << endl;
		}
		cout << endl;

		dropIndex(db);
	}
	return 0;
}
