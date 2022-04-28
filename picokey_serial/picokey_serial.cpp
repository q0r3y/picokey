#include <iostream>
#include "SimpleSerial.h"

using namespace std;

int main(int argc, char* argv[])
{
    try 
    {
        SimpleSerial serial("COM5", 115200);

        string input = serial.readLine();

        cout << input << endl;

    }
    catch (exception e)
    {
        cout << "Error: " << e.what() << endl;
        return 1;
    }
}