#ifndef _MUTEX_EVENT_H_
#define _MUTEX_EVENT_H_

#include <mutex>
#include <string>
#include <vector>

enum EventId {
  EVENT_NULL = 0,
  DATA_RECEIVE_FINGER_DATA_EVENT,
};

typedef enum {
  BLE_DISCONNECT = 0,
  BLE_CONNECTED,
  BLE_CONNECTING,
  BLE_ADV,
} BleStatus;

typedef enum {
  WIFI_DISCONNECT = 0,
  WIFI_CONNECTED,
  WIFI_CONNECTING,
} WifiStatus;

typedef enum {
  MQTT_DISCONNECT = 0,
  MQTT_CONNECTED,
} MqttStatus;

typedef enum { GET = 0, SET, REPLY } cmdType;

namespace lcy {

typedef struct {
  int CacheNum;
  std::string msg_id;
} CacheAck;

typedef struct {
  int cmd;
  int code;
  int type;
  std::string msg;
  std::string UUID;
} AppReplyParam;

typedef struct {
  std::string ssid;
  std::string psk;
  std::string timezone;
  std::string uuid;
} wifiParam;

typedef struct {
  std::string act_model_name;
  void* param;
} generalReplyData;

typedef struct {
  std::string wifiStatus;
  std::string ip;
} wifiStatusData;

typedef struct {
  std::string wifiStatus;
  std::string mqttStatus;
  std::string ip;
} bindResultData;

typedef struct {
  std::string code;
  std::string UUID;
} pinCodeSet;

typedef struct {
  std::string sn;
  std::string ver;
} deviceParams;

class dummy_ {};

class mutexEvent {
 public:
  mutexEvent() : data_(nullptr), id_(EventId::EVENT_NULL) {}

  explicit mutexEvent(const EventId& id, void* data) : data_(data), id_(id) {}

  explicit mutexEvent(const EventId& id) : data_(nullptr), id_(id) {}

  EventId getId() const { return id_; }
  void setId(const EventId& id) { id_ = id; }

  template <class T>
  T* getData() const {
    return static_cast<T*>(data_);
  }

  void setData(void* data) { data_ = data; }

 private:
  void* data_;
  EventId id_;
};
}  // namespace lcy

#endif