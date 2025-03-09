#include "periodTask.hpp"

std::unordered_map<std::string, periodTask_t *> TaskTable_;

void periodTaskInit(std::string &task_name, uint32_t period, uint32_t delay,
                    __EVENTHANDLER callback) {
  periodTask_t *task = (periodTask_t *)malloc(sizeof(periodTask_t));
  task->task_name = task_name;
  task->enable = false;
  task->delay = delay;
  task->period = period;
  task->callback = callback;
  TaskTable_.insert({task->task_name, task});
}

void periodTaskInit(periodTask_t *task) {
  TaskTable_.insert({task->task_name, task});
}

void periodTaskStart(std::string task_name) {
  if (task_name != "" && TaskTable_.find(task_name) != TaskTable_.end()) {
    TaskTable_[task_name]->enable = true;
  }
}

void periodTaskStop(std::string task_name) {
  if (task_name != "" && TaskTable_.find(task_name) != TaskTable_.end()) {
    TaskTable_[task_name]->enable = false;
  }
}
void periodTaskStatusJump(std::string desc_task_name,
                          std::string pro_task_name) {
  periodTaskStop(desc_task_name);
  periodTaskStart(pro_task_name);
}

void periodTaskRun(void) {
  for (auto &task_ : TaskTable_) {
    periodTask_t *task = task_.second;
    if (task->enable) {
      task->delay--;
      if (task->delay == 0) {
        if (task->callback != nullptr) {
          task->callback();
          task->delay = task->period;
        }
      }
    }
  }
}