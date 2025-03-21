#ifndef _DATA_SERIAL_IMPL_H_
#define _DATA_SERIAL_IMPL_H_

#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "mutex_node.hpp"
#include "serial/serialInterface.hpp"

#define READ_BUF_LEN 2048

class mutexSerialImpl {
 public:
  mutexSerialImpl() = default;
  ~mutexSerialImpl();

  mutexSerialImpl(lcy::mutexNode* node) : node_(node) {}

  void onInit();

  typedef struct {
    std::string port;
    int baudrate;
  } serialParam;

 private:
  std::unordered_map<std::string, serialParam> serialmp_ = {
      {"fingerPrint", {.port = "/dev/ttyUSB0", .baudrate = 115200}},
      {"ble", {.port = "/dev/ttyUSB1", .baudrate = 115200}},
  };
  std::vector<std::shared_ptr<serialInterface>> serialList_;
  lcy::mutexNode* node_;

  std::shared_ptr<std::thread> serialDataCheckThread_;
  void serialDataCheckRun_();
  bool serialDataCheckFlag_ = true;

  char readBuf_[READ_BUF_LEN] = {0};
};

#endif