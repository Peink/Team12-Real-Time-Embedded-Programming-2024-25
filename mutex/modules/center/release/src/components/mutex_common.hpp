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

class mutex_cb_param_t {
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

  mutex_cb_param_t() {
    uuid = "";
    data = nullptr;
    param = nullptr;
  }

  mutex_cb_param_t(char* buf, int len) {
    uuid = "";
    data = buf;
    size = len;
    param = nullptr;
  }

  mutex_cb_param_t(const mutex_cb_param_t& other) {
    dest = other.dest;
    size = other.size;
    cmd = other.cmd;
    data = (char*)malloc(size * sizeof(char));
    memcpy(data, other.data, size);
    param = other.param;
    uuid = other.uuid;
  }

  mutex_cb_param_t(const mutex_cb_param_t* other) {
    dest = other->dest;
    size = other->size;
    cmd = other->cmd;
    data = (char*)malloc(size * sizeof(char));
    memcpy(data, other->data, size);
    param = other->param;
    uuid = other->uuid;
  }

  mutex_cb_param_t& operator=(const mutex_cb_param_t& other) {
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

  ~mutex_cb_param_t() {
    if (data != nullptr) {
      free(data);
      data = nullptr;
    }
  }
};

using eventHandler = std::function<void(void*)>;

#define __EVENTHANDLER eventHandler

#define __CREATE_MUTEX_CB_PARAM_T_(PARAM, BUF, SIZE)        \
  do {                                                      \
    char* eventData = nullptr;                              \
    eventData = (char*)malloc(sizeof(char) * (SIZE));       \
    memcpy(eventData, (BUF), (SIZE));                       \
    (PARAM) = new lcy::mutex_cb_param_t(eventData, (SIZE)); \
  } while (0)

#define __DELETE_MUTEX_CB_PARAM_T_(PARAM) \
  do {                                    \
    if (param != nullptr) {               \
      delete param;                       \
    }                                     \
    param = nullptr;                      \
  } while (0)

// namespace lcy

}  // namespace lcy

#endif