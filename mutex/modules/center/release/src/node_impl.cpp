#include "node_impl.hpp"

NodeImpl::NodeImpl() : loader_(std::make_unique<lcy::mutexLoader>()) {}

NodeImpl::~NodeImpl() { mutexImplThreadRunFlag_ = false; }

void NodeImpl::start(void* param) {
  thread_ = std::make_unique<std::thread>(&NodeImpl::run_, this, param);
  thread_->detach();
}

void NodeImpl::run_(void* param) {
  onInit(param);
  if (node_num_) {
    loader_->run();
  }
}

void NodeImpl::onInit(void* param) { node_num_ = loader_->addmutexNode(param); }