#ifndef PERIOD_TASK_H
#define PERIOD_TASK_H

#include <stdbool.h>
#include <stdint.h>

#include <functional>
#include <string>
#include <unordered_map>
using eventHandler = std::function<void()>;

#define __EVENTHANDLER eventHandler

typedef struct {
  bool enable;
  std::string task_name;
  uint32_t period;
  uint32_t delay;
  __EVENTHANDLER callback;
} periodTask_t;

extern std::unordered_map<std::string, periodTask_t*> TaskTable_;

void periodTaskInit(std::string& task_name, uint32_t period, uint32_t delay,
                    __EVENTHANDLER callback);
void periodTaskInit(periodTask_t* task);
void periodTaskStart(std::string task_name);
void periodTaskStop(std::string task_name);
void periodTaskStatusJump(std::string desc_task_name,
                          std::string pro_task_name);
void periodTaskRun(void);

#endif  // PERIOD_TASK_H