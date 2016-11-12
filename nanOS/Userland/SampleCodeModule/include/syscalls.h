#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>

#include "process_info.h"

#define STDOUT 0
#define STDIN 1
#define STDERR 2

int write(unsigned int fd, const void *buffer, unsigned int bytes);
int read(unsigned int fd, void *buffer, unsigned int bytes);
int time(int selector);
int paint(int color, int x, int y);
int wait(uint64_t milliseconds);
int screen_Xresolution();
int screen_Yresolution();
int text_rows();
int text_cols();
void * reserve (unsigned int bytes);
int free_reserve(void * address);
void * data_address();
int set_char_color (uint64_t r, uint64_t g, uint64_t b);
int set_bg_color (uint64_t r, uint64_t g, uint64_t b);
int mutex_open(char * name);
int mutex_close(int key);
int mutex_lock(int key);
int mutex_unlock(int key);
int fifo_open(char * name);
int fifo_close(int key);

int exec(void * ptr, uint64_t params); /* Devuelve PID de nuevo proceso */
void end();
void yield();
int kill(uint64_t pid);
int set_foreground(uint64_t pid);
int pid();
int ppid();

int get_process_info(uint64_t pid, struct process_info_c * pi);

#endif
