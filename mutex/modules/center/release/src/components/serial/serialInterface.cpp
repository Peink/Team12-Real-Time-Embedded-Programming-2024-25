#include "serial/serialInterface.hpp"

serialInterface::serialInterface() = default;
serialInterface::~serialInterface() = default;

bool serialInterface::serialOpen() {
  printf("default serialOpen\r\n");
  return false;
}
void serialInterface::serialClose() {
  printf("default serialClose\r\n");
  return;
}

ssize_t serialInterface::receiveData(char* buffer, ssize_t max_len) {
  printf("default receiveData\r\n");
  return 0;
}

ssize_t serialInterface::sendData(const char* buffer, ssize_t buffer_len) {
  printf("default sendData\r\n");
  return 0;
}