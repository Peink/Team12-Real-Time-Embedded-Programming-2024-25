#include "node_impl.hpp"

NodeImpl::NodeImpl() : loader_(std::make_unique<lcy::mutexLoader>()) {}

NodeImpl::~NodeImpl() {}

void NodeImpl::start() { loader_->run(); }
