#include <unistd.h>

#include <cstdlib>
#include <iostream>

#include "node_impl.hpp"

int main() {
  std::cout << "lvChengYiNoBug" << std::endl;

  NodeImpl node_;
  node_.start();
  // int result =
  // system("/home/team12/embedded-project-door-lock/mutex/scripts/camera.sh");
  // if (result == 0) {
  //   std::cout << "Script executed successfully." << std::endl;
  // } else {
  //   std::cerr << "Script execution failed with code: " << result <<
  //   std::endl; return 0;
  // }
  while (1) {
    sleep(1);
  }
}