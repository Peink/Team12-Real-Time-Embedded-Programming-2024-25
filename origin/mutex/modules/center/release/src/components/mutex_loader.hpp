#ifndef _MUTEX_LOADER_H_
#define _MUTEX_LOADER_H_

#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "mutex_common.hpp"
#include "mutex_node.hpp"
#include "mutex_node_mgr.hpp"

namespace lcy {

class mutexLoader {
 public:
  explicit mutexLoader();
  void run();
  int addmutexNode(void* param);

 private:
  std::unordered_map<std::string, std::unique_ptr<mutexNode>> node_objs_;
  mutexNodeMgr node_mgr_;
  void createTask();
};

}  // namespace lcy

#endif