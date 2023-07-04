#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "lcm.h"
#include "task.h"

Task tasks[100];
int num_tasks = 0;
int lcmp = 0;

void read_system();
void print_system();
bool is_schedulable();
void schedule();
int get_earliest_deadline_task_index();

int main()
{
  read_system();
  print_system();

  // calcula o tempo mínimo de execução do sistema com base no MMC dos períodos
  lcmp = lcm_period(tasks, num_tasks);
  printf("LCM: %d\n", lcmp);

  if (is_schedulable())
  {
    printf("The system is schedulable.\n");
    schedule();
  }
  else
  {
    printf("The system is not schedulable.\n");
  }

  return 0;
}

void schedule()
{
  int time = 0;

  // guarda as variáveis originais de deadline, períodos e tempo de execução de cada tarefa
  int deadlines[num_tasks];
  int periods[num_tasks];
  int execution_times[num_tasks];

  for (int j = 0; j < num_tasks; j++)
  {
    deadlines[j] = tasks[j].deadline;
    periods[j] = tasks[j].period;
    execution_times[j] = tasks[j].execution_time;
  }

  char timeline[num_tasks][lcmp + 1];

  for (int i = 0; i < num_tasks; i++)
  {
    for (int j = 0; j < lcmp; j++)
    {
      timeline[i][j] = '-';
    }
    timeline[i][lcmp] = '\0';
  }

  // inicia a execução do algoritmo até o MMC dos períodos
  while (time < lcmp)
  {
    // pega o INDEX da tarefa que possui a menor deadline
    int earliest_deadline_task = get_earliest_deadline_task_index();

    // se nenhuma tarefa foi retornada (nenhuma tarefa executando),
    // signifca que o sistema está parado até o tempo atual chegar no próximo período
    if (earliest_deadline_task == -1)
    {
      printf("time[%d->%d]: IDLE\n", time, time + 1);
    }
    else // alguma tarefa está executando
    {
      // diminui o tempo de execução da tarefa
      // basicamente, executa uma unidade de tempo da tarefa por loop
      tasks[earliest_deadline_task].execution_time--;
      printf("time[%d->%d]: T%d\n", time, time + 1, earliest_deadline_task);
      // se o tempo de execução da tarefa chegou em 0, significa que a tarefa executou seu tempo necessário naquele período
      if (tasks[earliest_deadline_task].execution_time == 0)
      {
        // então, a tarefa entra em estado de não execução
        tasks[earliest_deadline_task].is_executing = false;
        tasks[earliest_deadline_task].deadline += tasks[earliest_deadline_task].period; // a deadline aumenta com base no período
        // o tempo que foi reduzido, volta ao original para que possa executar corretamente numa próxima execução
        tasks[earliest_deadline_task].execution_time = execution_times[earliest_deadline_task];
      }
      timeline[earliest_deadline_task][time] = 'o';
    }

    time++;

    for (int j = 0; j < num_tasks; j++)
    {
      // para cada tarefa, é checado se o tempo atual é igual ao período de início dela
      // caso seja, a tarefa entra em estado de executando e está disponível para a lista de execução
      int period = periods[j] * (time / periods[j]);
      if (period == time)
      {
        tasks[j].is_executing = true;
      }
    }
  }

  for (int i = 0; i < num_tasks; i++)
  {
    printf("T%d | %s\n", i, timeline[i]);
  }
}

int get_earliest_deadline_task_index()
{
  int earliest_deadline_task = -1;

  for (int i = 0; i < num_tasks; i++)
  {
    // retorna somente tarefas que estejam executando
    if (tasks[i].is_executing)
    {
      // pega a tarefa com menor deadline
      if (earliest_deadline_task == -1 || tasks[i].deadline < tasks[earliest_deadline_task].deadline)
        earliest_deadline_task = i;
    }
  }

  return earliest_deadline_task;
}

// função para determinar se o sistema é escalonável ou não
bool is_schedulable()
{
  // somatório do (tempo de execução / intervalo) de cada tarefa
  float condition = 0;
  for (int i = 0; i < num_tasks; i++)
  {
    float c = tasks[i].execution_time;
    float t = tasks[i].period;
    condition += c / t;
  }
  // caso o somatório seja menor ou igual a 1, o sistema é escalonável
  return condition <= 1;
}

void print_system()
{
  printf("TaskName\tPeriod\tExecution\tDeadline\n");
  for (int i = 0; i < num_tasks; i++)
  {
    printf("T%d\t\t%d\t%d\t\t%d\n", i, tasks[i].period, tasks[i].execution_time, tasks[i].deadline);
  }
}

void read_system()
{
  FILE *file;
  char line[100];

  file = fopen("samples/sistema3.txt", "r");
  if (file == NULL)
  {
    printf("Failed to open the file.\n");
    exit(1);
  }

  while (fgets(line, sizeof(line), file))
  {
    if (isalpha(line[0]))
      continue;

    int period, execution_time, deadline;

    sscanf(line, "%d %d %d", &period, &execution_time, &deadline);

    Task task = {period, execution_time, deadline, true};
    tasks[num_tasks] = task;
    num_tasks++;
  }

  fclose(file);
}
