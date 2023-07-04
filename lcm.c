#include "lcm.h"

int gdc(int a, int b)
{
  if (b == 0)
    return a;
  return gdc(b, a % b);
}

int lcm(int a, int b)
{
  return a * b / gdc(a, b);
}

int lcm_period(Task *tasks, int n)
{
  int result = tasks[0].period;
  for (int i = 1; i < n; i++)
  {
    result = lcm(result, tasks[i].period);
  }
  return result;
}