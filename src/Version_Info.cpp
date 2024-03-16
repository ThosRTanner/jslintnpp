#include "StdHeaders.h"

#include "Version_Info.h"

#include <tchar.h>

#include <vector>

Version_Info::Version_Info() noexcept
{
}

Version_Info::Version_Info(std::wstring const &fileName) : m_fileName(fileName)
{
}

Version_Info::Version_Info(
    std::wstring const &fileName, std::string const &content
) :
    m_fileName(fileName),
    m_content(content)
{
}

std::string Version_Info::GetContent() const
{
    if (m_content.empty())
    {
        FILE *fp = _tfopen(m_fileName.c_str(), L"rb");
        if (fp != nullptr)
        {
            fseek(fp, 0, SEEK_END);
            long const size = ftell(fp);
            if (size > 0)
            {
                fseek(fp, 0, SEEK_SET);
                std::vector<char> buffer;
                buffer.resize(size);
                size_t const nRead = fread(&*buffer.begin(), 1, size, fp);
                if (nRead == size)
                {
                    m_content = std::string(buffer.begin(), buffer.end());
                }
            }
            fclose(fp);
        }
    }
    return m_content;
}
