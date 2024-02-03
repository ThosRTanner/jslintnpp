#include "StdHeaders.h"

#include "Version_Info.h"

#include <tchar.h>

#include <vector>

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
                size_t const nRead = fread(&buffer[0], 1, size, fp);
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
