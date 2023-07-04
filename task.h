#pragma once

#include <stdbool.h>

typedef struct
{
  int period;
  int execution_time;
  int deadline;
  bool is_executing;
} Task;