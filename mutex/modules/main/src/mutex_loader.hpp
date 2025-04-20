#ifndef _MUTEX_LOADER_H_
#define _MUTEX_LOADER_H_

#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "mutex_node.hpp"
namespace lcy {

class mutexLoader {
 public:
  explicit mutexLoader();
  void run();

 private:
  std::vector<std::shared_ptr<mutexNode>> node_objs_;
  void createNode();
};

}  // namespace lcy

#endif