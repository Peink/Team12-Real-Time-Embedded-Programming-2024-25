#include "mutex_loader.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <thread>

#include "mutex_node_factory.hpp"

namespace lcy {

std::vector<std::string> node_list = {"data", "task", "wifi", "ble"};

mutexLoader::mutexLoader() {}

void mutexLoader::run() { createTask(); }

int mutexLoader::addmutexNode(void* param) {
  int node_num = 0;
  for (int i = 0; i < node_list.size(); i++) {
    auto node = mutexNodeFactory::createNode(node_list[i].data());
    if (node) {
      node->setNodeMgr(&node_mgr_);
      node->init();
      node_objs_.insert(std::make_pair(node_list[i], std::move(node)));
      node_num++;
    }
  }

  return node_num;
}

void mutexLoader::createTask() {
  for (auto& task : node_objs_) {
    auto& node = task.second;
    std::thread(&mutexNode::run, node.get(), nullptr).detach();
  }
}

}  // namespace lcy