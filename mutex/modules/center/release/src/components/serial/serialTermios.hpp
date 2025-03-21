#ifndef _SERIAL_TERMIOS_H_
#define _SERIAL_TERMIOS_H_

#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <serial/serialInterface.hpp>
#include <string>

class SerialTermios : public serialInterface {
 public:
  SerialTermios(const std::string& portname, speed_t baudrate, std::string name)
      : portname_(portname), baudrate_(baudrate) {
    name_ = name;
  }

  bool serialOpen() override;
  void serialClose() override;

  ssize_t receiveData(char* buffer, ssize_t max_len) override;
  ssize_t sendData(const char* buffer, ssize_t buffer_len) override;

 private:
  bool configurePort();

  std::string portname_;
  int baudrate_;
  speed_t baudrate_b_;
  int fd_ = -1;
};

#endif
