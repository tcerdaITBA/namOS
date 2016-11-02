#include "memoryAllocator.h"
#include "processManager.h"
#include "kernelMutex.h"
#include "strings.h"

#define NULL ((void *) 0) // ??

typedef struct mutex_node {
  process * p;
  struct mutex_node * next;
} mutex_node_t;

typedef struct {
  mutex_node_t * first;
  mutex_node_t * last;
} mutex_queue;

typedef struct {
  char * name;
  int locked;
  mutex_queue process_queue;
} mutex;

static mutex open_mutexes[MAX_MUTEXES];

extern int _unlocked(int * locknum);

static process * dequeue_process(mutex m);
static void queue_process(mutex m, process * p);
static void unblock_process(process * p);
static void block_process(process * p);
static void unblock_process(process * p);
static int is_open(int key);

static int is_open(int key) {
  return key < MAX_MUTEXES && open_mutexes[key].name != NULL;
}

int mutex_open(char * name) {
  int k, free_slot = -1;
  mutex m;
  for (k = 0; k < MAX_MUTEXES; k++) {
    if (!is_open(k)) {
      free_slot = k;
    }
    else if (strcmp(name, open_mutexes[k].name) == 0) {
      return k;
    }
  }

  m.process_queue.first = NULL;
  m.process_queue.last = NULL;

  open_mutexes[free_slot] = m;
  return free_slot;
}

int mutex_close(int key) {
  if (is_open(key)) {
    open_mutexes[key].name = NULL;
    return 1;
  }
  return NOT_OPEN;
}

int mutex_lock(int key) {
  if (is_open(key)) {
    mutex m = open_mutexes[key];
    if (!_unlocked(&m.locked)) { // Returns 0 if locked, 1 if unlocked
      process * p = get_current_process();
      block_process(p);
      queue_process(m, p);
    }
    return 1;
  }
  return NOT_OPEN;
}

int mutex_unlock(int key) {
  if (is_open(key)) {
    mutex m = open_mutexes[key];
    m.locked = UNLOCKED;
    process * p = dequeue_process(m);
    if (p != NULL)
      unblock_process(p);
    return 1;
  }
  return NOT_OPEN;
}

static void block_process(process * p) {
  p->st = BLOCKED;
}

static void unblock_process(process * p) {
  p->st = READY;
}

static void queue_process(mutex m, process * p) {
  mutex_node_t * node = (mutex_node_t *) get_page(sizeof(*node));
  node->p = p;
  if (m.process_queue.first == NULL) {
    m.process_queue.first = node;
    m.process_queue.last = node;
  }
  m.process_queue.last->next = node;
  m.process_queue.last = node;
}

static process * dequeue_process(mutex m) {
  if (m.process_queue.first == NULL)
    return NULL;

  process * p = m.process_queue.first->p;
  m.process_queue.first = m.process_queue.first->next;

  return p;
}
