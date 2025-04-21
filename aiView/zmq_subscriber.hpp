#ifndef _ZMQ_SUBSCRIBER_H_
#define _ZMQ_SUBSCRIBER_H_
#include <zmq.hpp>

class ZmqSubscriber {
 public:
  void Init(char* port);
  size_t Read(void* data);

 public:
  zmq::context_t context_{1};
  zmq::socket_t socket_{context_, zmq::socket_type::sub};
};

#endif
