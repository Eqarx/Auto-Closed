#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <cstdlib>


// 函数声明
void readConfigFile(const std::string& configFile, std::vector<std::chrono::system_clock::time_point>& targetTimes, std::vector<std::string>& targetPrograms);
void forceCloseProgram(const std::string& targetProgram);

int main() {
    // 输出当前工作目录
    std::cout << "当前工作目录：" << std::filesystem::current_path() << std::endl;

    // 尝试读取配置文件
    std::vector<std::chrono::system_clock::time_point> targetTimes;
    std::vector<std::string> targetPrograms;
    readConfigFile("config.txt", targetTimes, targetPrograms);

    std::cout << "读取配置文件：共" << targetTimes.size() << "项。" << std::endl;

    // 输出当前时间
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << "当前时间：" << std::ctime(&now);


    // 等待到达目标时间点
    for (size_t i = 0; i < targetTimes.size(); ++i) {
        auto currentTime = std::chrono::system_clock::now();

        while (currentTime < targetTimes[i]) {
            // 计算剩余时间
            auto timeDifference = targetTimes[i] - currentTime;

            // 显式指定使用的 duration 类型
            std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::steady_clock::duration>(timeDifference));

            // 重新获取当前时间
            currentTime = std::chrono::system_clock::now();
        }

        std::cout << "到达目标时间。关闭程序：" << targetPrograms[i] << std::endl;

        // 强制关闭目标程序
        forceCloseProgram(targetPrograms[i]);

        std::cout << "剩余：" << targetTimes.size() - (i + 1) << "项" << std::endl;
    }

    return 0;
}

// 从配置文件中读取目标时间点和程序名称
void readConfigFile(const std::string& configFile, std::vector<std::chrono::system_clock::time_point>& targetTimes, std::vector<std::string>& targetPrograms) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "错误：无法读取配置文件。" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(file, line)) {
        int hour, minute, second;
        std::string programName;

        if (sscanf_s(line.c_str(), "%d:%d:%d", &hour, &minute, &second) == 3) {
            // 构造目标时间时，考虑本地时区的偏移量
            auto now = std::chrono::system_clock::now();
            auto time_t_now = std::chrono::system_clock::to_time_t(now);
            auto local_tm = std::localtime(&time_t_now);

            local_tm->tm_hour = hour;
            local_tm->tm_min = minute;
            local_tm->tm_sec = second;

            auto targetTime = std::chrono::system_clock::from_time_t(std::mktime(local_tm));
            targetTimes.push_back(targetTime);
            targetPrograms.push_back(line.substr(line.find_last_of(' ') + 1));
        }
    }
    std::cout << "从配置文件中读取第" << targetTimes.size() << "项。" << std::endl;

    file.close();
}

// 强制关闭目标程序
void forceCloseProgram(const std::string& targetProgram) {
    // 在实际应用中，这里可能需要根据操作系统的不同采用不同的方法来关闭程序
    // 这里只是一个简单的演示，实际情况可能需要使用平台特定的API

    // 构建 taskkill 命令
    std::string command = "taskkill /F /IM " + targetProgram;

    // 使用 system 函数执行命令
    int result = std::system(command.c_str());

    if (result == 0) {
        std::cout << "强制关闭程序：" << targetProgram << std::endl;
    }
    else {
        std::cerr << "错误：无法关闭目标程序：" << targetProgram << std::endl;
    }
}