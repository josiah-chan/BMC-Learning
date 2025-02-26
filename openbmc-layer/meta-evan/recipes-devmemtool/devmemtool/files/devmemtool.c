#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include "devmemtool.h"

void print_usage(const char *progname)
{
    printf("Usage: %s ADDRESS [TYPE] [VALUE]\n", progname);
    printf("       Address : Memory address to access (e.g., 0x12345678)\n");
    printf("       Type   : [b]yte, [h]alfword, [w]ord, [a] for byte matrix, [d] for word matrix\n");
    printf("       Value  : Value to write (optional, write operation if provided)\n");
}

void print_matrix_byte(uint8_t *data, unsigned long addr)
{
    printf("   ");
    for (int col = 0; col < 16; col++)
    {
        printf("%02x ", col);
    }
    printf("\n");

    for (int row = 0; row < 16; row++)
    {
        printf("%02x ", row);
        for (int col = 0; col < 16; col++)
        {
            printf("%02x ", data[row * 16 + col]);
        }
        printf(" | ");
        for (int col = 0; col < 16; col++) {
            uint8_t val = data[row * 16 + col];
            printf("%c", isprint(val) ? val : '.'); // 显示ASCII字符
        }
        printf("\n");
    }
}

void print_matrix_word(uint32_t *data, unsigned long addr)
{
    printf("   ");
    for (int col = 0; col < 4; col++)
    {
        printf("%02x%02x%02x%02x ", col * 4 + 3, col * 4 + 2, col * 4 + 1, col * 4);
    }
    printf("\n");

    for (int row = 0; row < 16; row++)
    {
        printf("%02x ", row);
        for (int col = 0; col < 4; col++)
        {
            printf("%08x ", data[row * 4 + col]);
        }
        printf(" | ");
        for (int col = 0; col < 4; col++) {
            uint32_t val = data[row * 4 + col];
            for (int i = 0; i < 4; i++) {
                char c = (val >> (i * 8)) & 0xFF;
                printf("%c", isprint(c) ? c : '.'); // 显示ASCII字符
            }
        }
        printf("\n");
    }
}

void write_memory_value(void *virt_addr, char type, uint32_t value)
{
    switch (type)
    {
    case 'b':
        *((volatile uint8_t *)virt_addr) = (uint8_t)value;
        break;
    case 'h':
        *((volatile uint16_t *)virt_addr) = (uint16_t)value;
        break;
    case 'w':
        *((volatile uint32_t *)virt_addr) = value;
        break;
    default:
        fprintf(stderr, "Invalid type for write operation.\n");
        break;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        print_usage(argv[0]);
        return 1;
    }

    // 解析输入的地址和类型
    unsigned long addr = strtoul(argv[1], NULL, 0);
    char type = argv[2][0];

    // 打开 /dev/mem
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1)
    {
        perror("Failed to open /dev/mem");
        return 1;
    }

    // 计算映射的起始地址和偏移量
    off_t target = addr & ~(MAP_MASK);
    void *map_base = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target);
    if (map_base == MAP_FAILED)
    {
        perror("mmap failed");
        close(fd);
        return 1;
    }

    void *virt_addr = (char *)map_base + (addr & MAP_MASK);

    // 判断是否为写操作
    if (argc == 4)
    {
        uint32_t value = strtoul(argv[3], NULL, 0);
        write_memory_value(virt_addr, type, value);
        printf("Wrote 0x%08x to address 0x%lx\n", value, addr);
    }
    else
    {
        // 读取操作
        if (type == 'a')
        {
            print_matrix_byte((uint8_t *)virt_addr, addr);
        }
        else if (type == 'd')
        {
            print_matrix_word((uint32_t *)virt_addr, addr);
        }
        else if (type == 'b')
        {
            printf("0x%02x\n", *((volatile uint8_t *)virt_addr));
        }
        else if (type == 'h')
        {
            printf("0x%04x\n", *((volatile uint16_t *)virt_addr));
        }
        else if (type == 'w')
        {
            printf("0x%08x\n", *((volatile uint32_t *)virt_addr));
        }
        else
        {
            fprintf(stderr, "Invalid or unsupported type for matrix display.\n");
        }
    }

    // 释放资源
    munmap(map_base, MAP_SIZE);
    close(fd);
    return 0;
}
