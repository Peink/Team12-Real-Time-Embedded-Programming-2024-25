#ifndef _SERIAL_INTERFACE_H_
#define _SERIAL_INTERFACE_H_

#include <stdio.h>
#include <unistd.h>

#include <string>

class serialInterface {
 public:
  serialInterface();
  ~serialInterface();

  virtual bool serialOpen();
  virtual void serialClose();

  virtual ssize_t receiveData(char* buffer, ssize_t max_len);
  virtual ssize_t sendData(const char* buffer, ssize_t buffer_len);

  std::string name_;
  bool isOpen_ = false;
};

#endif