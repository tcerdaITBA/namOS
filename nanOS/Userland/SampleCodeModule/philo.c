#include "philo.h"
#include "time.h"
#include "stdio.h"
#include "syscalls.h"
#include "strings.h"
#include "random.h"
#include "executer.h"
#include "stdlib.h"

#define PHILO_PROCESS_NAME "philosopher"

int critical_m;
int state[MAX_PHILOSOPHERS];
int mut[MAX_PHILOSOPHERS];
int philosopherCount = 0;
int philosophers_PID[MAX_PHILOSOPHERS];

int philosophers_die[MAX_PHILOSOPHERS];

int pause = UNPAUSED;

static void take_forks(int i);
static void put_forks(int i);
static void test(int i);
static int philosopher(int argc, char * argv[]);
static void setState(int philo, int st);

static int add_philosopher();
static void remove_philosopher();
static void remove_all_philosophers();

static void pause_philosophers();
static int is_paused();

static int should_die(int i);

void listen_commands();

int start_philosophers_problem(int philoNumber) {
  int i;
  philoNumber = philoNumber > MAX_PHILOSOPHERS ? MAX_PHILOSOPHERS : philoNumber;

  critical_m = mutex_open(MAIN_MUTEX_NAME);
  srand(seconds() * minutes() * hour());

  for (i = 0; i < philoNumber; i++) {
    philosophers_PID[i] = add_philosopher();
  }

  listen_commands();
  return 0;
}

void listen_commands() {
  char c;
  while((c = getchar())) {
    switch (c) {
      case 'e':
      if (!is_paused())
        remove_all_philosophers();
      return;
      break;
      case 'a':
      if (!is_paused())
        add_philosopher();
      break;
      case 'r':
      if (!is_paused())
        remove_philosopher();
      break;
      case 'p':
      pause_philosophers();
      break;
    }
  }
}

static void pause_philosophers() {
  if (!is_paused()) {
    pause = PAUSED;
    mutex_lock(critical_m);
    printf("PAUSED\n");
  }
  else {
    pause = UNPAUSED;
    mutex_unlock(critical_m);
    printf("RESUMED\n");
  }
}

static int is_paused() {
  return pause == PAUSED;
}

static int add_philosopher() {
  char name[] = "PhilosopherMutex000";
  char args[3];
  int new_pid = -1, philo_index;

  printf("Locked in add\n");
  mutex_lock(critical_m);
  if (philosopherCount < MAX_PHILOSOPHERS) {
    philo_index = philosopherCount;
    philosopherCount += 1;
    itoa(philo_index, name + strlen(name) - 3, 10);

    printf("MutName: %s\n", name);

    mut[philo_index] = mutex_open(name);

    printf("ME CLAVE\n");
    mutex_lock(mut[philo_index]);
    printf("ME DESCLAVE\n");

    state[philo_index] = THINKING;
    itoa(philo_index, args, 10);

    new_pid = execp(philosopher, args, PHILO_PROCESS_NAME);
    philosophers_PID[philo_index] = new_pid;
  }
  mutex_unlock(critical_m);
  printf("UNLOCKED add\n");
  return new_pid;
}

static void remove_philosopher() {
  printf("Locked in remove\n");
  mutex_lock(critical_m);
  printf("IN remove\n");
  if (philosopherCount > 0) {
    philosopherCount -= 1;
    printf("Closing Mutex\n");
    mutex_close(mut[philosopherCount]);
    printf("Killing philosopher %d\n", philosopherCount);
    philosophers_die[philosopherCount] = 1;
  }
  mutex_unlock(critical_m);
  printf("UNLOCKED remove\n");
  yield();
}

static void remove_all_philosophers() {
  int i, philoCountAux = philosopherCount;
  for (i = 0; i < philoCountAux; i++)
    remove_philosopher();
}

static int philosopher(int argc, char * argv[]) {
  int i = atoi(argv[0]);
  while(1) {
    sleep(rand_int_range(0, 3) * 1000);
    take_forks(i);

    if (should_die(i)) 
      return 1;

    sleep(rand_int_range(0, 3) * 1000);
    put_forks(i);

    if (should_die(i)) 
      return 1;
  }
}

static int should_die(int i) {
  int die;
  mutex_lock(critical_m);

  die = philosophers_die[i];

  if (die)
    philosophers_die[i] = 0;

  mutex_unlock(critical_m);

  return die;
}

static void take_forks(int i) {
  printf("Locked in take forks ( %d )\n", i);
  mutex_lock(critical_m);
  printf("IN take forks %d\n", i);
  setState(i, HUNGRY);
  test(i);
  mutex_unlock(critical_m);
  printf("Unlocked take forks ( %d )\n", i);
  printf("Locked in mut[ %d ]\n", i);
  mutex_lock(mut[i]);
}

static void put_forks(int i) {
  printf("Locked in put forks ( %d )\n", i);
  mutex_lock(critical_m);
  printf("IN put forks %d\n", i);
  if (state[i] == HUNGRY)
    printf("EXPLOTAR\n");

  setState(i, THINKING);
  test(LEFT(i, philosopherCount));
  test(RIGHT(i, philosopherCount));
  mutex_unlock(critical_m);
  printf("Unlocked put forks ( %d )\n", i);
}

static void setState(int philo, int st) {
  state[philo] = st;
  render(state, philosopherCount);
}

static void test(int i) {
  if (state[i] == HUNGRY && state[LEFT(i, philosopherCount)] != EATING \
    && state[RIGHT(i, philosopherCount)] != EATING) {
    setState(i, EATING);
    mutex_unlock(mut[i]);
    printf("UNLOCKED mut [ %d ]\n", i);
  }
}
