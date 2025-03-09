#ifndef _MUTEX_NODE_MGR_H_
#define _MUTEX_NODE_MGR_H_

#include <condition_variable>
#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <queue>

#include "mutex_event.hpp"
#include "mutex_node.hpp"
namespace lcy {
class mutexNode;
class mutexNodeMgr {
 public:
  mutexNodeMgr();
  ~mutexNodeMgr();

  void publish(mutexEvent const* const event);
  void subscribe(const EventId& id, mutexNode* node);

  void eventHandler();

 private:
  std::map<EventId, std::list<mutexNode*>> sub_queue_{};

  std::mutex sub_mutex_;
  std::mutex pub_mutex_;
  std::queue<const mutexEvent*> pub_queue_;
  std::condition_variable pub_cond_;

  bool threadRunFlag_ = true;
};

}  // namespace lcy

#endif