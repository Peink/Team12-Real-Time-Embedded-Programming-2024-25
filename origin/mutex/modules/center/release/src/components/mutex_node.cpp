#include "mutex_node.hpp"

#include <iostream>

#include "mutex_common.hpp"
#include "mutex_node_mgr.hpp"

namespace lcy {
mutexNode::mutexNode(std::string name)
    : name_(std::move(name)), mutexNodeMgr_(nullptr) {}

mutexNode::~mutexNode() {
  ThreadRunFlag_ = false;
  queue_cond_.notify_all();
}

void mutexNode::init() { onInit(); }

void mutexNode::setNodeMgr(mutexNodeMgr* node_mgr) { mutexNodeMgr_ = node_mgr; }

void mutexNode::publish(mutexEvent const* const event) {
  mutexNodeMgr_->publish(event);
}

void mutexNode::subscribe(const EventId& id) {
  mutexNodeMgr_->subscribe(id, this);
}

void mutexNode::setSubscribeCallback(const SubscribeCallback& callback) {
  callback_ = callback;
}

bool mutexNode::getThreadRunFlag() { return ThreadRunFlag_; }

void mutexNode::handleEvent(mutexEvent const* const event) {
  if (callback_) {
    callback_(event);
  }
  std::unique_lock<std::mutex> locker(queue_mutex_);
  queue_.push(event);
  locker.unlock();
  queue_cond_.notify_one();
}

const mutexEvent* mutexNode::getEvent() {
  std::unique_lock<std::mutex> locker(queue_mutex_);
  while (ThreadRunFlag_ && queue_.empty()) {
    queue_cond_.wait(locker);
  }
  const mutexEvent* ev = nullptr;
  if (!queue_.empty()) {
    ev = queue_.front();
    queue_.pop();
  }
  locker.unlock();
  return ev;
}

const std::string& mutexNode::getName() const { return name_; }

}  // namespace lcy