#include "mutex_node_factory.hpp"

#include <cstring>

#include "data_camera_impl.hpp"
#include "data_echo_impl.hpp"
#include "data_finger_impl.hpp"
#include "data_keyboard_impl.hpp"

namespace lcy {
namespace mutexNodeFactory {
std::shared_ptr<mutexNode> createNode(const char* name) {
  const char* node_objs[] = {"keyBoard", "finger", "echo", "camera"};
  if (0 == strcmp(name, node_objs[0])) {
    return std::make_shared<dataKeyBoardImpl>();
  } else if (0 == strcmp(name, node_objs[1])) {
    return std::make_shared<dataFingerImpl>();
  } else if (0 == strcmp(name, node_objs[2])) {
    return std::make_shared<dataEchoImpl>();
  } else if (0 == strcmp(name, node_objs[3])) {
    return std::make_shared<dataCameraImpl>();
  }

  return nullptr;
}
}  // namespace mutexNodeFactory
}  // namespace lcy