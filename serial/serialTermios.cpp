#include "serialTermios.hpp"

#include <unordered_map>

std::unordered_map<int, speed_t> speedMp = {
    {115200, B115200},
};

bool SerialTermios::serialOpen() {
  fd_ = open(portname_.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
  if (fd_ < 0) {
    std::cerr << "Error opening " << portname_ << ": " << strerror(errno)
              << std::endl;
    return false;
  }

### --------------------------------------------------------------------
### Set input/output baud rate
### Set the character size to 8 bits (CS8)
### Disable parity check (PARENB | PARODD)
### Stop position (clear CSTOPB)
### Disable hardware flow control (clear CRTSCTS)
### Set to non-standard mode, no echo, no mapping.
### Set the minimum read character to 0 and the timeout to 0.1 seconds (non-blocking read)
###	Call write() and read() respectively to complete data transmission.
### Use std::cerr to print error messages.
### --------------------------------------------------------------------
	
  if (configurePort()) {
    isOpen_ = true;
    printf("serial configure success : %s\r\n", portname_.c_str());
    return true;
  } else {
    printf("serial configure failed\r\n");
    return false;
  }
}

void SerialTermios::serialClose() {
  if (fd_ >= 0) {
    close(fd_);
    fd_ = -1;
  }
}

bool SerialTermios::configurePort() {
  struct termios tty;
  memset(&tty, 0, sizeof tty);

  if (tcgetattr(fd_, &tty) != 0) {
    std::cerr << "Error from tcgetattr: " << strerror(errno) << std::endl;
    return false;
  }

  if (speedMp.find(baudrate_) == speedMp.end()) {
    printf("baud speed is error\r\n");
    return false;
  }

  baudrate_b_ = speedMp[baudrate_];

  cfsetospeed(&tty, baudrate_b_);
  cfsetispeed(&tty, baudrate_b_);

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;  // 8-bit chars
  tty.c_iflag &= ~IGNBRK;                      // disable break processing
  tty.c_lflag = 0;                             // no signaling chars, no echo,
                                               // no canonical processing
  tty.c_oflag = 0;                             // no remapping, no delays
  tty.c_cc[VMIN] = 0;                          // read doesn't block
  tty.c_cc[VTIME] = 1;                         // 0.1 seconds read timeout

  tty.c_iflag &= ~(IXON | IXOFF | IXANY);  // shut off xon/xoff ctrl

  tty.c_cflag |= (CLOCAL | CREAD);    // ignore modem controls,
                                      // enable reading
  tty.c_cflag &= ~(PARENB | PARODD);  // shut off parity
  tty.c_cflag |= 0;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if (tcsetattr(fd_, TCSANOW, &tty) != 0) {
    std::cerr << "Error from tcsetattr: " << strerror(errno) << std::endl;
    return false;
  }

  return true;
}

ssize_t SerialTermios::sendData(const char* buffer, ssize_t buffer_len) {
  ssize_t len = write(fd_, buffer, buffer_len);
  if (len < 0) {
    std::cerr << "Error writing to " << portname_ << ": " << strerror(errno)
              << std::endl;
    return len;
  }
  return len;
}

ssize_t SerialTermios::receiveData(char* buffer, ssize_t max_len) {
  memset(buffer, 0, max_len);
  ssize_t len = read(fd_, buffer, max_len);
  if (len < 0) {
    std::cerr << "Error reading from " << portname_ << ": " << strerror(errno)
              << std::endl;
    return len;
  }
  return len;
}