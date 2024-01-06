#include "StdHeaders.h"

#include "Version_Info.h"

#include <tchar.h>

std::string Version_Info::GetContent()
{
    if (m_content.empty())
    {
        FILE *fp = _tfopen(m_fileName.c_str(), L"rb");
        if (fp != NULL)
        {
            fseek(fp, 0, SEEK_END);
            long size = ftell(fp);
            if (size > 0)
            {
                fseek(fp, 0, SEEK_SET);
                char *buffer = new char[size + 1];
                size_t nRead = fread(buffer, 1, size, fp);
                if (nRead == size)
                {
                    m_content = std::string(buffer, size);
                }
                delete[] buffer;
            }
        }
    }

    return m_content;
}
