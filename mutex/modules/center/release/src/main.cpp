#include <unistd.h>

#include <iostream>

#include "node_impl.hpp"

int main() {
  std::cout << "lvChengYiNoBug" << std::endl;

  NodeImpl node_;
  node_.start(&node_);
  while (1) {
    sleep(1);
  }
}