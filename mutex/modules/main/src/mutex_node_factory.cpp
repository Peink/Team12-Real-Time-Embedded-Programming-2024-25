#include "mutex_node_factory.hpp"

#include <cstring>

#include "data_finger_impl.hpp"
#include "data_keyboard_impl.hpp"

namespace lcy {
namespace mutexNodeFactory {
std::unique_ptr<mutexNode> createNode(const char* name) {
  const char* node_objs[] = {"keyBoard", "serial"};
  if (0 == strcmp(name, node_objs[0])) {
    return std::make_unique<dataKeyBoardImpl>();
  } else if (0 == strcmp(name, node_objs[1])) {
    return std::make_unique<dataFingerImpl>();
  }

  return nullptr;
}
}  // namespace mutexNodeFactory
}  // namespace lcy