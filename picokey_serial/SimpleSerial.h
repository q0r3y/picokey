#ifndef SIMPLESERIAL_H
#define SIMPLESERIAL_H
#include <iostream>
#include <asio.hpp>

class SimpleSerial
{
private:
    asio::io_service io;
    asio::serial_port serial;

public:
    SimpleSerial(std::string port, unsigned int baud_rate);

    void writeString(std::string s);

    std::string readLine();
};

#endif