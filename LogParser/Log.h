#pragma once

#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>

#include <iostream>

class Log
{
public:
    explicit Log(const std::filesystem::path& path = "Output.txt");
    ~Log();

    void Write(const std::string& data);

private:
    std::ofstream m_fileStream;
    std::mutex m_mutex;
};
