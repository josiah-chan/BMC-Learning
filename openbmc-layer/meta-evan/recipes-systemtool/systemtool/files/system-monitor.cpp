#include <iostream> 
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

class SystemMonitor
{
public:
    void printUsage()
    {
        std::cout << std::fixed << std::setprecision(1)
        << "CPU: " << getCPUUsage() << "% | "
        << "Mem: " << getMemoryUsage() << "%\n";
    }

private:
    double getCPUUsage()
    {
        std::ifstream statFile("/proc/stat");
        std::string line;
        std::getline(statFile, line);
    
        std::string cpu;
        unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    
        std::istringstream iss(line);
        iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    
        unsigned long long idleTime = idle + iowait;
        unsigned long long activeTime = user + nice + system + irq + softirq + steal;
        unsigned long long totalTime = idleTime + activeTime;
    
        return totalTime > 0 ? 100.0 * activeTime / totalTime : 0.0;
    }

    double getMemoryUsage()
    {
        std::ifstream memFile("/proc/meminfo");
        unsigned long total = 0, available = 0;
        
        for (std::string line; getline(memFile, line);)
        {
            if (line.find("MemTotal:") == 0)
            {
                std::istringstream(line.substr(9)) >> total;
            }
            else if (line.find("MemAvailable:") == 0)
            {
                std::istringstream(line.substr(13)) >> available;
                break;
            }
        }

        return total > 0 ? 100.0 * (1.0 - (double)available / total) : 0.0;
    }
};

int main()
{
    SystemMonitor monitor;
    monitor.printUsage();
    return 0;
}
