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

void printHeader();
void printFooter();
void printMiddle();
void printLineEnd(int spaces);

const string VERSION = "2022-04-28";
vector<string> dbPicokey;

string getSerialInput() {
	try
	{
		cout << ":: [*] Waiting for serial input.."; 
		printLineEnd(16);
		SimpleSerial serial("COM5", 115200);
		string input = serial.readLine();
		return input;
	}
	catch (exception e)
	{
		throw e;
	}
}

int dbCallback(void* NotUsed, int argc, char** argv, char** azColName) {
	// int argc: holds the number of results
	// (array) azColName: holds each column returned
	// (array) argv: holds each value

	for (int i = 0; i < argc; i++) {
		//cout << azColName[i] << ": " << argv[i] << endl;
		dbPicokey.push_back(argv[i]);
	}
	return 0;
}

void dbRetrieveDeviceData(string& inputDeviceId) {
	 //int dbDeviceId;
	 sqlite3* db;
	 const string sql = "SELECT * FROM keys WHERE deviceId=" + inputDeviceId + ";";

	 if (sqlite3_open("../data/picokey_db.sqlite3", &db) != SQLITE_OK) {
		 throw exception("Unable to open database.");
	 }

	 cout << ":: [*] Searching database for Device ID.."; 
	 printLineEnd(8);
	 sqlite3_exec(db, sql.c_str(), dbCallback, 0, NULL);

	 if (dbPicokey.size() == 0) {
		 sqlite3_close(db);
		 throw exception("Device ID not found in database.");
	 }

	 sqlite3_close(db);
}


int main(int argc, char* argv[])
{
	string inputData;
	string inputDeviceId;

	printHeader();

	try 
	{
		inputData = getSerialInput();
		//inputData = "486197d7f1a6c30cc051";
		cout << ":: [*] Device Input: " << inputData;
		printLineEnd(8);
		inputDeviceId = inputData.substr(0, 6);
		cout << ":: [*] Device ID: " << inputDeviceId;
		printLineEnd(25);
		dbRetrieveDeviceData(inputDeviceId);
		cout << ":: [+] Device ID found!";
		printLineEnd(26);
		printMiddle();
	}
	catch (exception& e)
	{
		cout << ":: [-] Error: " << e.what();
		printLineEnd(3);
		printFooter();
		return 1;
	}

	// Computes sha1 hmac and slices off last 14
	cout << ":: [*] Computing SHA1 HMAC...";
	printLineEnd(20);

	string sha1hmac = hmac<SHA1>(dbPicokey[1], dbPicokey[2]);
	string sha1hmacSlice = sha1hmac.substr(26, 14);

	cout << ":: [*] SHA1 HMAC Result (sliced): " << sha1hmacSlice;
	printLineEnd(1);

	string finalResult = dbPicokey[0] + sha1hmacSlice;

	cout << ":: [*] Final result: " << finalResult;
	printLineEnd(8);

	if (inputData == finalResult) {
		cout << ":: [+] Input matches result!";
		printLineEnd(21);
		cout << ":: [+] Successfully authenticated.";
		printLineEnd(15);
		// set device data
	}
	else {
		cout << ":: [-] Input does not match result!";
		printLineEnd(20);
		cout << ":: [-] Unable to authenticate.";
		printLineEnd(19);
	}

	printFooter();
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