#include "serialInterface.hpp"

// Constructors and destructors use the default implementation
serialInterface::serialInterface() = default;
serialInterface::~serialInterface() = default;

// Default serial port opening function
bool serialInterface::serialOpen() {
  printf("default serialOpen\r\n");  // The default implementation is used
  return false;                      // Default return fails
}

// The default serial port closes the function
void serialInterface::serialClose() {
  printf("default serialClose\r\n");
  return;
}

// Default receive function
ssize_t serialInterface::receiveData(char* buffer, ssize_t max_len) {
  printf("default receiveData\r\n");
  return 0;  // No data is received by default
}

// Default send function
ssize_t serialInterface::sendData(const char* buffer, ssize_t buffer_len) {
  printf("default sendData\r\n");
  return 0;  // No data is received by default
}
