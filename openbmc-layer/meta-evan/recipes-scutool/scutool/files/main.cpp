#include <iostream>
#include <string>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <cstdint>
#include <sys/mman.h>
#include <iomanip>  // 支持 setw 和 setfill
#include "Sqlite3Database.h"

// 将输入的十六进制字符串转换为带'h'的偏移地址格式
std::string formatOffset(const std::string &input)
{
    // 去掉前缀 '0x'
    std::string offset = input.substr(2);
    // 确保长度为3位（不包括'h'），不足的部分用0填充
    while (offset.length() < 3)
    {
        offset = "0" + offset;
    }
    return offset + "h";
}

void* mapAddress(uint64_t address)
{
    int fd = open("/dev/mem", O_RDONLY | O_SYNC);
    if (fd == -1)
    {
        throw std::runtime_error("Failed to open /dev/mem");
    }

    // 计算页对齐的地址和偏移量
    size_t pageSize = sysconf(_SC_PAGE_SIZE);
    off_t pageOffset = address & (pageSize - 1);
    off_t alignedAddress = address & ~(pageSize - 1);

    // 映射页对齐的地址
    void* mapped = mmap(NULL, pageSize, PROT_READ, MAP_SHARED, fd, alignedAddress);
    if (mapped == MAP_FAILED)
    {
        close(fd);
        throw std::runtime_error("Failed to map memory address");
    }
    close(fd);

    // 返回精确的地址
    return static_cast<uint8_t*>(mapped) + pageOffset;
}

// 读取并打印指定地址的数据
uint32_t readMemoryValue(uint64_t address)
{
    try
    {
        void* mapped_ptr = mapAddress(address);
        // 获取页对齐的基地址
        void* page_aligned_addr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(mapped_ptr) & ~(sysconf(_SC_PAGE_SIZE) - 1));      
        // 读取值
        uint32_t value = *reinterpret_cast<volatile uint32_t*>(mapped_ptr);
        
        // 解除映射
        munmap(page_aligned_addr, sysconf(_SC_PAGE_SIZE));
        
        return value;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Error reading memory: " << e.what() << std::endl;
        throw;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <Offset>\n";
        return 1;
    }

    // 解析命令行参数
    std::string inputOffset = argv[1];
    std::string formattedOffset = formatOffset(inputOffset);

    try
    {
        // 创建数据库对象并连接到数据库
        Sqlite3Database db;
        if (!db.connectToDatabase("/etc/scutool/scutool.db"))
        {
            std::cerr << "Failed to connect to database.\n";
            return 1;
        }

        // 构建查询语句
        std::string sql = "SELECT register_name, description, full_address FROM scu_registers WHERE offset = '" + formattedOffset + "'";

        // 执行查询
        VecResult result = db.querySql(sql);

        // 检查查询结果
        if (result.empty())
        {
            std::cout << "No matching record found for Offset: " << formattedOffset << "\n";
        }
        else
        {
            // 打印查询结果
            for (const auto &row : result)
            {
                // 打印完整地址
                std::cout << "Address: " << row[2] << "\n";
                // 打印 Offset 和寄存器信息
                std::cout << "Offset:  " << formattedOffset << "  |  "
                          << row[0] << ": " << row[1] << "\n";

                // 解析完整地址
                uint64_t address = std::stoull(row[2], nullptr, 16);
                
                // 读取并打印内存值
                uint32_t value = readMemoryValue(address);
                std::cout << "Value:   " << "0x" << std::setfill('0') << std::setw(8) << std::hex << value << std::endl;
            }
        }

        // 关闭数据库连接
        db.closeDatabaseConnection();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}