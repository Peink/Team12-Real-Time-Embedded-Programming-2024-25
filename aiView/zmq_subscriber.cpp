#include "zmq_subscriber.hpp"

#include <unistd.h>

#include <cassert>
#include <iostream>

#include "zmq.h"
#include "zmq.hpp"
#define LEN 2048
#define IP_ADDR "192.168.143.8"

static char ip_port[LEN] = {0};

void ZmqSubscriber::Init(char* port) {
  memset(ip_port, 0, LEN);
  sprintf(ip_port, "tcp://%s:%s", IP_ADDR, port);
  socket_.connect(ip_port);
  socket_.setsockopt(ZMQ_SUBSCRIBE, "", 0);
}

size_t ZmqSubscriber::Read(void* data) {
  zmq::message_t msg;
  auto ret = socket_.recv(msg, zmq::recv_flags::none);
  if (ret > 0) {
    std::memcpy(data, msg.data(), ret.value());
  }

  return ret.value();
}