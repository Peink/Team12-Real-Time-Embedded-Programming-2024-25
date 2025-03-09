#include "mutex_node_factory.hpp"

#include <cstring>

// #include "mutex_ble_node/mutex_ble_node.hpp"
// #include "mutex_data_node/mutex_data_node.hpp"
// #include "mutex_task_node/mutex_task_node.hpp"
#include "mutex_wifi_node/mutex_wifi_node.hpp"

namespace lcy {
namespace mutexNodeFactory {
std::unique_ptr<mutexNode> createNode(const char* name) {
  const char* node_objs[] = {"data", "ble", "wifi", "task"};
  if (0 == strcmp(name, node_objs[0])) {
    // return std::make_unique<mutexDataNode>(node_objs[0]);
  } else if (0 == strcmp(name, node_objs[1])) {
    // return std::make_unique<mutexBleNode>(node_objs[1]);
  } else if (0 == strcmp(name, node_objs[2])) {
    return std::make_unique<mutexWifiNode>(node_objs[2]);
  } else if (0 == strcmp(name, node_objs[3])) {
    // return std::make_unique<mutexTaskNode>(node_objs[3]);
  }

  return nullptr;
}
}  // namespace mutexNodeFactory
}  // namespace lcy