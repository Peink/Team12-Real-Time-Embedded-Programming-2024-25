#ifndef _NODE_BASE_H_
#define _NODE_BASE_H_

#include <memory>

class Node {
 public:
  Node() : impl_(std::make_unique<NodeImpl>()) {}

  ~Node();
  void onInit();

 private:
  class NodeImpl;
  std::unique_ptr<NodeImpl> impl_;
};

#endif