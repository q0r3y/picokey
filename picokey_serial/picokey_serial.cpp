#include <iostream>
#include <vector>
#include <exception>
#include "SHA1.h"
#include "HMAC.h"
#include "SimpleSerial.h"
#include "sqlite3.h"

using namespace std;
const string VERSION = "2022-04-28";

void printHeader();
void printFooter();
void printMiddle();
void printLineEnd(int spaces);

// Verification url:
// https://codebeautify.org/hmac-generator
// ASIO
// https://sourceforge.net/projects/asio/files/asio/1.22.1%20%28Stable%29/asio-1.22.1.zip/download
// Place ASIO in sdk folder, add asio/include to VC++ include directories

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

 vector<string> retrieveDeviceData(string& inputDeviceId) {
	 sqlite3* db;
	 sqlite3_stmt* stmt;
	 sqlite3_open("../data/picokey_db.sqlite3", &db);
	 cout << ":: [*] Searching database for Device ID.."; 
	 printLineEnd(8);
	 // Lookup device in database by deviceId
	 bool deviceIdFound = false;
	 if (inputDeviceId == "486197")
		 deviceIdFound = true;
	 
	 if (deviceIdFound) {
		 // Serial number, Count, Key
		 vector<string> deviceData = { "486197", "1", "3679243789" };
		 return deviceData;
	 }
	 else {
		 throw exception("Device ID not found in database.");
	 }
}


int main(int argc, char* argv[])
{
	string inputData;
	string inputDeviceId;
	vector<string> deviceData;

	printHeader();

	try 
	{
		inputData = getSerialInput();
		//inputData = "486197d7f1a6c30cc051";
		cout << ":: [*] Device Input: " << inputData;
		printLineEnd(8);
		inputDeviceId = inputData.substr(0, 6);;
		cout << ":: [*] Device ID: " << inputDeviceId;
		printLineEnd(25);
		deviceData = retrieveDeviceData(inputDeviceId);
		cout << ":: [+] Device ID found!";
		printLineEnd(26);
		printMiddle();
	}
	catch (exception& e)
	{
		cout << ":: [-] Error: " << e.what();
		printLineEnd(3);
		cout << endl;
		printFooter();
		return 1;
	}

	// Computes sha1 hmac and slices off last 14
	cout << ":: [*] Computing SHA1 HMAC...";
	printLineEnd(20);

	string sha1hmac = hmac<SHA1>(deviceData[1], deviceData[2]);
	string sha1hmacSlice = sha1hmac.substr(26, 14);

	cout << ":: [*] SHA1 HMAC Result (sliced): " << sha1hmacSlice;
	printLineEnd(1);

	string finalResult = deviceData[0] + sha1hmacSlice;

	cout << ":: [*] Final result: " << finalResult;
	printLineEnd(8);

	if (inputData == finalResult) {
		cout << ":: [+] Input matches result!";
		printLineEnd(21);
		cout << ":: [+] Successfully authenticated.";
		printLineEnd(15);
	}
	else {
		cout << ":: [-] Input does not result!";
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