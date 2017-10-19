#include "Yelp-odb.hxx"
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

std::vector<std::string> findHours(odb::database& db, std::string username) {
	std::vector<std::string> result;
	transaction t(db.begin());
	// Your implementation goes here:
	// Find the hours
	t.commit();
	return result;
}

std::vector<StarCount> countStars(odb::database& db, float latMin, float latMax, float longMin,
																	float longMax) {
	std::vector<StarCount> result;
	transaction t(db.begin());
	// Your implementation goes here:
	// Count the stars
	t.commit();
	return result;
}

void createIndex(odb::database& db){
	// Your implementation goes here:
	// create a columnstore index to accelerate your query
}

void dropIndex(odb::database& db){
	// Your implementation goes here:
	// drop the columnstore index you've created
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

int main(int argc, char** argv) {

	using namespace std;
	database db("SA", "AdvancedDB17", "yelp", "localhost");

	findHours(db, "kn");

	for(size_t i = 0; i < 5; i++) {
		countStars(db, 30.0, 45.7, -100.0, -1.0);
		cout << "Run number " << i << " time before indexing: " << getLastQueryRuntime(db).elapsed_time << endl;
	}

	createIndex(db);
	
	for(size_t i = 0; i < 5; i++) {
		countStars(db, 30.0, 45.7, -100.0, -1.0);
		cout << "Run number " << i << " time before indexing: " << getLastQueryRuntime(db).elapsed_time << endl;
	}
	cout << endl;

	dropIndex(db);

	return 0;
}
