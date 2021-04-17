#include <algorithm>
#include <fstream>
#include <iterator>
#include "LogParser.h"
#include "Utils.h"

LogParser::LogParser(const std::filesystem::path& path, const std::filesystem::path& patterns, bool inverted) :
    m_path{ path },
    m_inverted{ inverted },
    m_log{ std::make_unique<Log>() },
    m_threadPool{ std::make_unique<ThreadPool>() }
{
    if (std::ifstream fileStream{ patterns.string().c_str(), std::ios::binary }; fileStream.good())
    {
        std::string pattern;
        while (std::getline(fileStream, pattern))
        {
            pattern.erase(std::remove_if(pattern.begin(), pattern.end(), [](char c)
            {
                return c == '\n' || c == '\r';
            }));
            if (pattern.size() > 0)
            {
                m_patterns.emplace_back(pattern);
            }
        }

        fileStream.close();
    }
}

void LogParser::Search() noexcept
{
    for (auto& p : m_patterns)
    {
        if (std::filesystem::file_size(m_path) < p.size())
        {
            return;
        }

        if (std::ifstream fileStream{ m_path.string().c_str(), std::ios::binary }; fileStream.good())
        {
            while (true)
            {
                std::vector<char> chunk;
                const auto pageSize = PageSize();
                chunk.resize(pageSize);
                fileStream.read(&chunk[0], pageSize);
                const std::size_t readCount = fileStream.gcount();

                // EOF
                if (readCount < p.length())
                {
                    break;
                }

                // align to EOL
                if (readCount == pageSize && chunk.back() != '\n')
                {
                    auto eol = std::find(chunk.rbegin(), chunk.rend(), '\n');
                    const std::size_t chunkSize = std::distance(eol, chunk.rend());
                    fileStream.seekg(chunkSize - readCount, std::ios_base::cur);
                    chunk.resize(chunkSize);
                }
                else
                {
                    chunk.resize(readCount);
                }

                auto threaded = !(std::filesystem::file_size(m_path) < pageSize);
                if (threaded)
                {
                    auto task = [this, chunk{ std::move(chunk) }, p]
                    {
                        SearchChunk(chunk, p);
                    };

                    m_threadPool->AddTask(task);
                }
                else
                {
                    SearchChunk(chunk, p);
                }
            }

            fileStream.close();
        }
    }
}

void LogParser::ReverseSearch() noexcept
{
    if (std::ifstream fileStream{ m_path.string().c_str(), std::ios::binary }; fileStream.good())
    {
        while (true)
        {
            std::vector<char> chunk;
            const auto pageSize = PageSize();
            chunk.resize(pageSize);
            fileStream.read(&chunk[0], pageSize);
            const std::size_t readCount = fileStream.gcount();

            // EOF
            if (readCount == 0)
            {
                break;
            }

            // align to EOL
            if (readCount == pageSize && chunk.back() != '\n')
            {
                auto eol = std::find(chunk.rbegin(), chunk.rend(), '\n');
                const std::size_t chunkSize = std::distance(eol, chunk.rend());
                fileStream.seekg(chunkSize - readCount, std::ios_base::cur);
                chunk.resize(chunkSize);
            }
            else
            {
                chunk.resize(readCount);
            }

            auto threaded = !(std::filesystem::file_size(m_path) < pageSize);
            // some weird bug occurs when trying to access source concurrently
            // further debugging is needed
            if (false)
            {
                auto task = [this, chunk{ std::move(chunk) }]
                {
                    SearchChunk(chunk);
                };

                m_threadPool->AddTask(task);
            }
            else
            {
                SearchChunk(chunk);
            }
        }

        fileStream.close();
    }
}

void LogParser::SearchChunk(const std::vector<char>& chunk, const std::string& pattern) noexcept
{
    for (auto searchPos = chunk.begin(), eol = chunk.begin();
        searchPos != chunk.end();
        searchPos = eol == chunk.end() ? chunk.end() : eol + 1)
    {
        std::boyer_moore_searcher<std::string::const_iterator> searcher{ pattern.begin(), pattern.end() };
        searchPos = std::search(searchPos, chunk.end(), searcher);
        eol = std::find(searchPos, chunk.end(), '\n');

        if (searchPos != chunk.end())
        {
            auto it = std::make_reverse_iterator(searchPos);
            auto lower = std::find(it, chunk.rend(), '\n');
            if (eol != chunk.end())
            {
                std::string output(lower.base(), eol + 1);
                m_log->Write(output);
            }
            else
            {
                std::string output(lower.base(), eol);
                m_log->Write(output);
            }
        }
    }
}

void LogParser::SearchChunk(const std::vector<char>& chunk) noexcept
{
    for (auto start = chunk.begin(), eol = chunk.end();
        start != chunk.end();
        start = eol == chunk.end() ? chunk.end() : eol + 1)
    {
        auto searchPos = start;
        eol = std::find(start, chunk.end(), '\n');

        for (auto& p : m_patterns)
        {
            std::boyer_moore_searcher<std::string::const_iterator> searcher{ p.begin(), p.end() };
            searchPos = std::search(start, eol, searcher);
            if (searchPos != eol)
            {
                break;
            }
        }

        if (searchPos == eol)
        {
            if (eol != chunk.end())
            {
                std::string output(start, eol + 1);
                m_log->Write(output);
            }
            else
            {
                std::string output(start, eol);
                m_log->Write(output);
            }
        }
    }
}
