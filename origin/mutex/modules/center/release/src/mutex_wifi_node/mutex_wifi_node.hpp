#ifndef _MUTEX_WIFI_NODE_H_
#define _MUTEX_WIFI_NODE_H_

#include "mutex_node.hpp"

namespace lcy {
class mutexWifiNode : public mutexNode {
 public:
  explicit mutexWifiNode(const std::string& name);
  ~mutexWifiNode() override;

  void onInit() override;
  void run(void* param) override;

 private:
  class mutexWifiImpl;
  std::unique_ptr<mutexWifiImpl> impl_;
};
}  // namespace lcy

#endif