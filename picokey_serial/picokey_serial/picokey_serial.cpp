// Verification url:
// https://codebeautify.org/hmac-generator
// ASIO
// https://sourceforge.net/projects/asio/files/asio/1.22.1%20%28Stable%29/asio-1.22.1.zip/download
// Place ASIO in sdk folder, add asio/include to VC++ include directories

#include <iostream>
#include <vector>
#include <exception>
#include "SHA1.h"
#include "HMAC.h"
#include "SimpleSerial.h"
#include "sqlite3.h"
using namespace std;

#define COM_PORT "COM17"
const string VERSION = "2022-04-29";

void printHeader();
void printFooter();
void printMiddle();
string getSerialInput();
void printLineEnd(int spaces);
string calculateOTP(string count);
void attemptOTP(string inputOTP);
void setDatabaseCount(string count);
void dbRetrieveDeviceData(string& inputDeviceId);
int dbCallback(void* NotUsed, int argc, char** argv, char** azColName);

struct {
	string deviceId;
	string secretKey;
	string currentCount;
} dbDeviceInfo;

int main(int argc, char* argv[]) {

	printHeader();
	try {
		string inputOTP = getSerialInput();

		cout << ":: [*] Input OTP: " << inputOTP;
		printLineEnd(11);


		string inputDeviceId = inputOTP.substr(0, 6);

		cout << ":: [*] Device ID: " << inputDeviceId;
		printLineEnd(25);

		dbRetrieveDeviceData(inputDeviceId);

		cout << ":: [+] Device ID found!";
		printLineEnd(26);
		printMiddle();

		attemptOTP(inputOTP);
	}
	catch (exception& e) {
		cout << ":: [-] Error: " << e.what();
		printLineEnd(3);
		printFooter();
		return 1;
	}

	printFooter();
	return 0;
}

string getSerialInput() {
	try {
		string input;
		cout << ":: [*] Input an OTP code: ";
		cin >> input;

		//SimpleSerial serial(COM_PORT, 115200);
		//string input = serial.readLine();
		
		return input;
	}
	catch (exception e) {
		throw e;
	}
}

string calculateOTP(string count) {
	cout << ":: [*] Calculating SHA1 HMAC...";
	printLineEnd(18);

	string sha1hmac = hmac<SHA1>(count, dbDeviceInfo.secretKey);
	string sha1hmacShort = sha1hmac.substr(26, 14);

	cout << ":: [*] SHA1 HMAC Result: ..." << sha1hmacShort;
	printLineEnd(7);

	string otp = dbDeviceInfo.deviceId + sha1hmacShort;

	cout << ":: [*] Calculated OTP: " << otp;
	printLineEnd(6);

	return otp;
}

void attemptOTP(string inputOTP) {
	int attempt = 1;
	int currentCount = stoi(dbDeviceInfo.currentCount);
	while (attempt <= 10) {
		cout << ":: [*] Attempt " << attempt;
		printLineEnd(33);

		string calculatedOTP = calculateOTP(to_string(currentCount));
		currentCount++;
		if (inputOTP == calculatedOTP) {

			cout << ":: [+] Input OTP matches Calculated OTP!";
			printLineEnd(9);

			cout << ":: [+] Found matching count at: "
				<< to_string(currentCount - 1);
			printLineEnd(15);

			setDatabaseCount(to_string(currentCount));

			cout << ":: [+] Successfully authenticated.";
			printLineEnd(15);

			break;
		}
		attempt++;
	}

	if (attempt > 10) {
		cout << ":: [-] Input OTP does not match Calculated OTP!";
		printLineEnd(2);
		cout << ":: [-] Unable to authenticate.";
		printLineEnd(19);
		// Respond invalid
	}
}

void setDatabaseCount(string count) {
	sqlite3* db;
	char* errMsg = 0;
	const string sql = "UPDATE keys SET count=" + count + 
						" WHERE deviceId=" + dbDeviceInfo.deviceId + ";";

	if (sqlite3_open("../data/picokey_db.sqlite3", &db) != SQLITE_OK) {
		throw exception("Unable to open database.");
	}
	cout << ":: [*] Saving new count: ";
	cout << count;
	cout << " to database..";
	printLineEnd(8);

	sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg);

	if (errMsg) {
		//cout << errMsg << endl;
		sqlite3_free(errMsg);
		throw exception("Unable to save record in database");
	}

	sqlite3_free(errMsg);
	sqlite3_close(db);
}

void dbRetrieveDeviceData(string& inputDeviceId) {
	sqlite3* db;
	const string sql = "SELECT * FROM keys WHERE deviceId=" + inputDeviceId + ";";

	if (sqlite3_open("../data/picokey_db.sqlite3", &db) != SQLITE_OK) {
		throw exception("Unable to open database.");
	}

	cout << ":: [*] Searching database for Device ID..";
	printLineEnd(8);

	sqlite3_exec(db, sql.c_str(), dbCallback, 0, NULL);

	if (dbDeviceInfo.deviceId.size() == 0 ||
		dbDeviceInfo.secretKey.size() == 0 ||
		dbDeviceInfo.currentCount.size() == 0) {
		sqlite3_close(db);
		throw exception("Device ID not found in database.");
	}

	sqlite3_close(db);
}

/*
* int argc: holds the number of results
* (array) azColName: holds each column returned
* (array) argv: holds each value
*/
int dbCallback(void* NotUsed, int argc, char** argv, char** azColName) {

	// Is there a name for this process?
	vector<string*> temp = {
		&dbDeviceInfo.deviceId,
		&dbDeviceInfo.secretKey,
		&dbDeviceInfo.currentCount
	};

	for (int i = 0; i < argc; i++) {
		*temp[i] = argv[i];
	}

	return 0;
}

void printHeader() {
	cout << endl;
	cout << ":::::::::::: Picokey_Serial " + VERSION + " ::::::::::::\n";
	cout << "::                                               ::\n";
}

void printFooter() {
	cout << "::                                               ::\n";
	cout << ":::::::::::::::::::::::::::::::::::::::::::::::::::";
	cout << endl;
}

void printMiddle() {
	cout << ":: ............................................. ::\n";
}

void printLineEnd(int spaces) {
	for (int i = 0; i < spaces; i++)
		cout << " ";
	cout << "::\n";
}