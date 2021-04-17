#pragma once

#include <atomic>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "ThreadPool.h"
#include "Log.h"

class LogParser
{
public:
    explicit LogParser(
        const std::filesystem::path& path,
        const std::filesystem::path& patterns,
        bool inverted = false);

    void Search() noexcept;
    void ReverseSearch() noexcept;

private:
    void SearchChunk(const std::vector<char>& chunk, const std::string& pattern) noexcept;
    void SearchChunk(const std::vector<char>& chunk) noexcept;

private:
    std::filesystem::path m_path;
    std::vector<std::string> m_patterns;
    bool m_inverted;
    std::unique_ptr<Log> m_log;
    std::unique_ptr<ThreadPool> m_threadPool;
};
