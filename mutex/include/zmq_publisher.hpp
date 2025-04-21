#ifndef _ZMQ_PUBLISHER_H_
#define _ZMQ_PUBLISHER_H_

#include <stdio.h>

#include <zmq.hpp>

class ZmqPublisher {
 public:
  ZmqPublisher() = default;
  ~ZmqPublisher() = default;
  void onInit(char* port);
  void read(char* data, int size);
  void write(const char* data, int size);

 private:
  zmq::context_t context_{1};
  zmq::socket_t socket_{context_, zmq::socket_type::pub};
};
#endif