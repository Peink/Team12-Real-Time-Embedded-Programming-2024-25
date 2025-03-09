#ifndef _MUTEX_NODE_H_
#define _MUTEX_NODE_H_

#include <error.h>

#include <condition_variable>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "mutex_common.hpp"
#include "mutex_node_mgr.hpp"

namespace lcy {

using SubscribeCallback = std::function<void(mutexEvent const* const evt)>;

class mutexNodeMgr;
class mutexEvent;

class mutexNode {
 public:
  explicit mutexNode(std::string name);
  explicit mutexNode() = default;
  virtual ~mutexNode();

  virtual void onInit() = 0;
  virtual void run(void* param) = 0;

  void init();

  void setNodeMgr(mutexNodeMgr* node_mgr);
  const std::string& getName() const;
  void publish(mutexEvent const* const event);
  void subscribe(const EventId& id);
  void setSubscribeCallback(const SubscribeCallback& callback);
  void handleEvent(mutexEvent const* const event);
  const mutexEvent* getEvent();
  bool getThreadRunFlag();

 private:
  const std::string name_;
  mutexNodeMgr* mutexNodeMgr_;           // 共享的消息指针
  std::queue<const mutexEvent*> queue_;  // 待处理的消息队列
  std::mutex queue_mutex_;               //>队列锁
  std::condition_variable queue_cond_;   // 队列条件变量
  SubscribeCallback callback_;
  bool ThreadRunFlag_ = true;
};
}  // namespace lcy

#endif