#include "mutex_node_mgr.hpp"

#include <unistd.h>

#include <iostream>
#include <thread>

namespace lcy {

void mutexNodeMgr::publish(mutexEvent const* const event) {
  std::unique_lock<std::mutex> locker(pub_mutex_);
  pub_queue_.push(event);
  locker.unlock();
  pub_cond_.notify_one();
}

void mutexNodeMgr::subscribe(const EventId& id, mutexNode* node) {
  sub_mutex_.lock();
  sub_queue_[id].push_back(node);
  sub_mutex_.unlock();
}

void mutexNodeMgr::eventHandler() {
  while (threadRunFlag_) {
    std::unique_lock<std::mutex> locker(pub_mutex_);
    while (threadRunFlag_ && pub_queue_.empty()) {
      pub_cond_.wait(locker);
    }

    const auto event = pub_queue_.front();
    pub_queue_.pop();
    locker.unlock();

    sub_mutex_.lock();
    auto it = sub_queue_.find(event->getId());
    if (it != sub_queue_.end()) {
      auto node_list = it->second;
      for (const auto& node : node_list) {
        node->handleEvent(event);
      }
    }
    sub_mutex_.unlock();
  }
}

mutexNodeMgr::mutexNodeMgr() {
  std::thread(&mutexNodeMgr::eventHandler, this).detach();
}

mutexNodeMgr::~mutexNodeMgr() {
  threadRunFlag_ = false;
  pub_cond_.notify_all();
}

}  // namespace lcy