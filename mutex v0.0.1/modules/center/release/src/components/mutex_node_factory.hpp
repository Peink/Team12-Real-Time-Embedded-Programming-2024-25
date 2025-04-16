#ifndef _MUTEX_NODE_FACTORY_H_
#define _MUTEX_NODE_FACTORY_H_

#include <memory>

namespace lcy {
class mutexNode;
namespace mutexNodeFactory {
std::unique_ptr<mutexNode> createNode(const char* name);
};
}  // namespace lcy

#endif