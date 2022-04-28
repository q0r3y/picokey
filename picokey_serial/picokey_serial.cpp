#include <iostream>
#include <vector>
#include <exception>
#include "SHA1.h"
#include "HMAC.h"
#include "SimpleSerial.h"

using namespace std;

const string VERSION = "2022-04-28";

// Verification url:
// https://codebeautify.org/hmac-generator

string getSerialInput() {
	try
	{
		cout << ":: [*] Waiting for serial input.."; 
		cout << "                ::" << endl;;
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
	 cout << ":: [*] Searching database for Device ID.."; 
	 cout << "        ::" << endl;
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

	cout << endl;
	cout << ":::::::::::: Picokey_Serial " + VERSION + " ::::::::::::\n";
	cout << "::                                               ::\n";
	try 
	{
		//inputData = getSerialInput();
		inputData = "486197d7f1a6c30cc051";
		cout << ":: [*] Device Input: " << inputData << "        ::\n";
		inputDeviceId = inputData.substr(0, 6);;
		cout << ":: [*] Device ID: " << inputDeviceId;
		cout << "                         ::\n";
		deviceData = retrieveDeviceData(inputDeviceId);
		cout << ":: [+] Device ID found!                          ::\n";
		cout << ":: ............................................. ::\n";
	}
	catch (exception& e)
	{
		cout << ":: [-] Error: " << e.what() << "   ::" << endl;
		cout << "::                                               ::\n";
		cout << ":::::::::::::::::::::::::::::::::::::::::::::::::::";
		cout << endl;
		return 1;
	}
	

	// Computes sha1 hmac and slices off last 14
	cout << ":: [*] Computing SHA1 HMAC...                    ::\n";
	string sha1hmac = hmac<SHA1>(deviceData[1], deviceData[2]);
	string sha1hmacSlice = sha1hmac.substr(26, 14);

	cout << ":: [*] SHA1 HMAC Result (sliced): " << sha1hmacSlice << " ::\n";

	string finalResult = deviceData[0] + sha1hmacSlice;
	cout << ":: [*] Final result: " << finalResult << "        ::\n";

	if (inputData == finalResult) {
		cout << ":: [+] Input matches result!                     ::\n";
		cout << ":: [+] Successfully authenticated.               ::\n";
	}
	else {
		cout << ":: [-] Input does not result!                    ::\n";
		cout << ":: [-] Unable to authenticate.                   ::\n";
	}

	cout << "::                                               ::\n";
	cout << ":::::::::::::::::::::::::::::::::::::::::::::::::::" << endl;
	
	

	return 0;
}