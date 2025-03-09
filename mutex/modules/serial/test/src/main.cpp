#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

class SerialPort {
 public:
  SerialPort(const std::string& portname, speed_t baudrate)
      : portname_(portname), baudrate_(baudrate) {}

  bool serialOpen() {
    fd_ = open(portname_.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd_ < 0) {
      std::cerr << "Error opening " << portname_ << ": " << strerror(errno)
                << std::endl;
      return false;
    }

    return configurePort();
  }

  void serialClose() {
    if (fd_ >= 0) {
      close(fd_);
      fd_ = -1;
    }
  }

  bool sendData(const std::string& data) {
    ssize_t len = write(fd_, data.c_str(), data.size());
    if (len < 0) {
      std::cerr << "Error writing to " << portname_ << ": " << strerror(errno)
                << std::endl;
      return false;
    }
    return true;
  }

  bool receiveData(std::string& buffer, size_t max_len) {
    buffer.resize(max_len);
    ssize_t len = read(fd_, &buffer[0], max_len);
    if (len < 0) {
      std::cerr << "Error reading from " << portname_ << ": " << strerror(errno)
                << std::endl;
      return false;
    }
    buffer.resize(len);
    return true;
  }

 private:
  bool configurePort() {
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(fd_, &tty) != 0) {
      std::cerr << "Error from tcgetattr: " << strerror(errno) << std::endl;
      return false;
    }

    cfsetospeed(&tty, baudrate_);
    cfsetispeed(&tty, baudrate_);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;  // 8-bit chars
    tty.c_iflag &= ~IGNBRK;                      // disable break processing
    tty.c_lflag = 0;                             // no signaling chars, no echo,
                                                 // no canonical processing
    tty.c_oflag = 0;                             // no remapping, no delays
    tty.c_cc[VMIN] = 0;                          // read doesn't block
    tty.c_cc[VTIME] = 5;                         // 0.5 seconds read timeout

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

  std::string portname_;
  speed_t baudrate_;
  int fd_ = -1;
};

int main() {
  SerialPort serial("/dev/ttyAMA0",
                    B115200);  // Change "/dev/ttyS0" to your serial port

  if (!serial.serialOpen()) {
    std::cerr << "Failed to open serial port" << std::endl;
    return 1;
  }

  std::string sendData = "Hello, Serial Port!";
  if (!serial.sendData(sendData)) {
    std::cerr << "Failed to send data" << std::endl;
  } else {
    std::cout << "Data sent: " << sendData << std::endl;
  }

  std::string receiveData;
  if (!serial.receiveData(receiveData, 100)) {
    std::cerr << "Failed to receive data" << std::endl;
  } else {
    std::cout << "Data received: " << receiveData << std::endl;
  }

  serial.serialClose();
  return 0;
}