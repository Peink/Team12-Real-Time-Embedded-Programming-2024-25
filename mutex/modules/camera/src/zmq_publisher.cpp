#include "zmq_publisher.hpp"

#include <assert.h>
#include <unistd.h>

#include <iostream>

#include "zmq.hpp"

#define LEN 64

static char ip_port[LEN] = {0};

void ZmqPublisher::onInit(char* port) {
  memset(ip_port, 0, LEN);
  sprintf(ip_port, "tcp://*:%s", port);
  socket_.bind(ip_port);
}

void ZmqPublisher::read(char* data, int size) {}

void ZmqPublisher::write(const char* data, int size) {
  int len = socket_.send(data, size, 0);
  // printf("len : %d\r\n", len);
}