#include <cassert>
#include "Log.h"

Log::Log(const std::filesystem::path& path) :
    m_fileStream{ path.string().c_str(), std::ios::binary }
{
    assert(m_fileStream.good());
}

Log::~Log()
{
    m_fileStream.close();
}

void Log::Write(const std::string& data)
{
    std::lock_guard lg{ m_mutex };
    m_fileStream.write(data.c_str(), data.length());
}
