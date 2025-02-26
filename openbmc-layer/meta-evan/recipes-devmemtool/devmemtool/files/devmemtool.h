#ifndef DEVMEMTOOL_H
#define DEVMEMTOOL_H

#include <stdint.h>

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

void print_usage(const char *progname);
void print_matrix_byte(uint8_t *data, unsigned long addr);
void print_matrix_word(uint32_t *data, unsigned long addr);
void write_memory_value(void *virt_addr, char type, uint32_t value);

#endif // DEVMEMTOOL_H
