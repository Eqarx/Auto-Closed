#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <cstdlib>


// ��������
void readConfigFile(const std::string& configFile, std::vector<std::chrono::system_clock::time_point>& targetTimes, std::vector<std::string>& targetPrograms);
void forceCloseProgram(const std::string& targetProgram);

int main() {
    // �����ǰ����Ŀ¼
    std::cout << "��ǰ����Ŀ¼��" << std::filesystem::current_path() << std::endl;

    // ���Զ�ȡ�����ļ�
    std::vector<std::chrono::system_clock::time_point> targetTimes;
    std::vector<std::string> targetPrograms;
    readConfigFile("config.txt", targetTimes, targetPrograms);

    std::cout << "��ȡ�����ļ�����" << targetTimes.size() << "�" << std::endl;

    // �����ǰʱ��
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << "��ǰʱ�䣺" << std::ctime(&now);


    // �ȴ�����Ŀ��ʱ���
    for (size_t i = 0; i < targetTimes.size(); ++i) {
        auto currentTime = std::chrono::system_clock::now();

        while (currentTime < targetTimes[i]) {
            // ����ʣ��ʱ��
            auto timeDifference = targetTimes[i] - currentTime;

            // ��ʽָ��ʹ�õ� duration ����
            std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::steady_clock::duration>(timeDifference));

            // ���»�ȡ��ǰʱ��
            currentTime = std::chrono::system_clock::now();
        }

        std::cout << "����Ŀ��ʱ�䡣�رճ���" << targetPrograms[i] << std::endl;

        // ǿ�ƹر�Ŀ�����
        forceCloseProgram(targetPrograms[i]);

        std::cout << "ʣ�ࣺ" << targetTimes.size() - (i + 1) << "��" << std::endl;
    }

    return 0;
}

// �������ļ��ж�ȡĿ��ʱ���ͳ�������
void readConfigFile(const std::string& configFile, std::vector<std::chrono::system_clock::time_point>& targetTimes, std::vector<std::string>& targetPrograms) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "�����޷���ȡ�����ļ���" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(file, line)) {
        int hour, minute, second;
        std::string programName;

        if (sscanf_s(line.c_str(), "%d:%d:%d", &hour, &minute, &second) == 3) {
            // ����Ŀ��ʱ��ʱ�����Ǳ���ʱ����ƫ����
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
    std::cout << "�������ļ��ж�ȡ��" << targetTimes.size() << "�" << std::endl;

    file.close();
}

// ǿ�ƹر�Ŀ�����
void forceCloseProgram(const std::string& targetProgram) {
    // ��ʵ��Ӧ���У����������Ҫ���ݲ���ϵͳ�Ĳ�ͬ���ò�ͬ�ķ������رճ���
    // ����ֻ��һ���򵥵���ʾ��ʵ�����������Ҫʹ��ƽ̨�ض���API

    // ���� taskkill ����
    std::string command = "taskkill /F /IM " + targetProgram;

    // ʹ�� system ����ִ������
    int result = std::system(command.c_str());

    if (result == 0) {
        std::cout << "ǿ�ƹرճ���" << targetProgram << std::endl;
    }
    else {
        std::cerr << "�����޷��ر�Ŀ�����" << targetProgram << std::endl;
    }
}