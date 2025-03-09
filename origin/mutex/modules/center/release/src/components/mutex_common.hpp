#ifndef _MUTEX_COMMON_H_
#define _MUTEX_COMMON_H_

#include <stdlib.h>

#include <condition_variable>
#include <cstring>
#include <functional>
#include <list>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace lcy {

typedef enum {
  CTL_OBJ,
  DATA_OBJ,
} DATA_TYPE;

typedef enum {} SIG;

typedef enum {

} PID;

typedef enum {
  REQ,
  RES,
} ACT;  //>

enum {
  BLE_INTERFACE,
  WIFI_INTERFACE,
};

class esp_ble_gatts_cb_param_t {
 public:
  int dest;  //>数据的目的地
  int size;  //>数据大小
  int cmd;   //>数据命令
  // int sig;                              //> 对应eventHanlder的switch type
  // int act;                              //>数据是请求还是响应
  // int type;                             //>数据类型
  char* data;  //>数据本身
  void* param;
  // int64_t timestamp;                    //>数据发送的时间
  std::string uuid;  //>该消息的UUID
  // void (*finishCallback)(void* param);  //> 数据到达后的回调

  esp_ble_gatts_cb_param_t() {
    uuid = "";
    data = nullptr;
    param = nullptr;
  }

  esp_ble_gatts_cb_param_t(const esp_ble_gatts_cb_param_t& other) {
    dest = other.dest;
    size = other.size;
    cmd = other.cmd;
    data = (char*)malloc(size * sizeof(char));
    memcpy(data, other.data, size);
    param = other.param;
    uuid = other.uuid;
  }

  esp_ble_gatts_cb_param_t(const esp_ble_gatts_cb_param_t* other) {
    dest = other->dest;
    size = other->size;
    cmd = other->cmd;
    data = (char*)malloc(size * sizeof(char));
    memcpy(data, other->data, size);
    param = other->param;
    uuid = other->uuid;
  }

  esp_ble_gatts_cb_param_t& operator=(const esp_ble_gatts_cb_param_t& other) {
    if (&other != this) {
      if (data != nullptr) free(data);
      data = nullptr;
      dest = other.dest;
      size = other.size;
      cmd = other.cmd;
      data = (char*)malloc(size * sizeof(char));
      memcpy(data, other.data, size);
      param = other.param;
      uuid = other.uuid;
    }
    return *this;
  }

  ~esp_ble_gatts_cb_param_t() {
    if (data != nullptr) {
      free(data);
      data = nullptr;
    }
  }
};

using eventHandler = std::function<void(void*)>;

#define __EVENTHANDLER eventHandler
}  // namespace lcy

#endif