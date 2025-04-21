#include "data.hpp"

#include <unistd.h>

#include <iostream>

char recv_data[DATA_LEN] = {0};
char order_data[DATA_LEN] = {0};
bool capture_flag = false;
std::mutex data_mutex;

void data_interface::init() {
  data_thread_ =
      std::make_unique<std::thread>(&data_interface::data_run_, this);
  data_thread_->detach();

  order_thread_ =
      std::make_unique<std::thread>(&data_interface::order_run_, this);
  order_thread_->detach();
}

void data_interface::data_run_() {
  ZmqSubscriber camera_subscriber;
  camera_subscriber.Init("5557");
  while (thread_run_flag) {
    std::unique_lock<std::mutex> locker(data_mutex);
    memset(recv_data, 0, DATA_LEN);
    int size = camera_subscriber.Read(recv_data);
    locker.unlock();
    if (size > 0) {
      std::cout << "data size:  " << size << std::endl;
    }
    usleep(10000);
  }
}

void data_interface::order_run_() {
  ZmqSubscriber capture_subscriber;
  capture_subscriber.Init("5558");
  while (thread_run_flag) {
    memset(order_data, 0, DATA_LEN);
    int size = capture_subscriber.Read(order_data);
    if (size > 0) {
      std::cout << "order size:  " << size << std::endl;
      printf("===================================== order :%s\r\n", order_data);
      if (strncmp(order_data, "start_capture", strlen("start_capture")) == 0) {
        capture_flag = true;
        printf("capture flag = true\r\n");
      }
    }
    usleep(10000);
  }
}