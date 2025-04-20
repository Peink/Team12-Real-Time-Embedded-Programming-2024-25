#include "mutex_loader.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <thread>

#include "mutex_node_factory.hpp"

namespace lcy {

std::vector<std::string> node_list = {"finger", "keyBoard", "echo"};

mutexLoader::mutexLoader() {}

void mutexLoader::run() { createNode(); }

void mutexLoader::createNode() {
  for (int i = 0; i < node_list.size(); i++) {
    auto node = mutexNodeFactory::createNode(node_list[i].data());
    if (node) {
      printf("create success \r\n");
      node->onInit();
      node_objs_.push_back(node);
      node->setNodeCallbackInterfaceList(&node_objs_);
    }
  }
}

}  // namespace lcy