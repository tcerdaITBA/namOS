#include "memoryAllocator.h"
#include "processManager.h"
#include "dirs.h"
#include "videoDriver.h"
#include "process.h"
#include "interrupts.h"

#define QUANTUM 1

typedef struct c_node {
	int quantum;
	process *p;
	struct c_node *next;
} list_node;


static void add_process(process *p);
static void set_next_current();

void _change_process(uint64_t rsp);

static int64_t process_lock = UNLOCKED;

extern int _unlocked(int64_t * locknum);


/* Proceso actualmente corriendo */
static list_node *current = NULL;
static list_node *prev = NULL;

void _yield_process();


static void lock_list() {
	while (!_unlocked(&process_lock))
		yield_process();
}

static void unlock_list() {
	process_lock = UNLOCKED;
}

process * get_current_process() {
	return current->p;
}

uint64_t next_process(uint64_t current_rsp) {
	if (current == NULL)
		return current_rsp;

	unassign_quantum();

	if (current->quantum > 0)
		return current_rsp;

	current->quantum = QUANTUM;

	set_rsp_process(current->p, current_rsp);

	prev = current;
	current = current->next;

	set_next_current();

	return get_rsp_process(current->p);
}

uint64_t exec_process(process * new_process) {
	int pid;
	assign_quantum();

	add_process(new_process);

	pid = pid_process(new_process);

	if (pid == 0)
		_change_process(get_rsp_process(current->p));

	unassign_quantum();

	return pid;
}

static void add_process(process * p) {
	lock_list();

	assign_quantum(); /* Aseguramos que no haya un cambio de contexto */

	list_node *new_node = (list_node *) get_page(sizeof(*new_node));

	new_node->p = p;
	new_node->quantum = QUANTUM;

	if (current == NULL) {
		current = new_node;
		current->next = current;
		prev = current;
	}
	else {
		new_node->next = current->next;
		current->next = new_node;
	}

	unlock_list();

	unassign_quantum(); /* Quitamos privilegio */
}

static void set_next_current() {
	while (is_blocked_process(current->p) || is_delete_process(current->p)) {
		list_node * next = current->next;

		if (is_delete_process(current->p)) {
			prev->next = next;
			destroy_process(current->p);
			store_page((uint64_t) current);
		}
		else
			prev = current;

		current = next;
	}
}

void yield_process() {
	current->next->quantum += 1; /* Quantum al siguiente proceso pues el actual quito tiempo */
	current->quantum = 0;
	_yield_process();
}

/* Se avanza con el proceso que está delante */
void end_process() {

	assign_quantum(); /* Aseguramos que no haya un cambio de contexto */

	lock_list();

	list_node * n = current;

	destroy_process(n->p);

	prev->next = current->next;

	current = current->next;

	store_page((uint64_t) n);

	set_next_current();

	unlock_list();

	assign_quantum();

	_change_process(get_rsp_process(current->p));
}

void assign_quantum() {
	current->quantum += 1;
}

void unassign_quantum() {
	current->quantum -= 1;
}