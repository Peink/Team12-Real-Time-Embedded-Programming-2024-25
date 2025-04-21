#ifndef DATA_HPP
#define DATA_HPP

#include <memory>
#include <mutex>
#include <thread>

#include "zmq_subscriber.hpp"

#define DATA_LEN 480 * 640 * 3 * 2 + sizeof(uint16_t)

extern char recv_data[DATA_LEN];
extern std::mutex data_mutex;
extern bool capture_flag;

class data_interface {
 public:
  ~data_interface() { thread_run_flag = false; }
  void init();

 private:
  std::unique_ptr<std::thread> data_thread_;
  std::unique_ptr<std::thread> order_thread_;
  void data_run_();
  void order_run_();

  bool thread_run_flag = true;
};

#endif  // DATA_HPP
