#pragma once

#include <string>

class Version_Info
{
  public:
    Version_Info()
    {
    }

    Version_Info(std::wstring const &fileName) : m_fileName(fileName)
    {
    }

    Version_Info(std::wstring const &fileName, std::string const &content) :
        m_fileName(fileName),
        m_content(content)
    {
    }

    std::wstring GetFileName() const
    {
        return m_fileName;
    }

    std::string GetContent();

  private:
    std::wstring m_fileName;
    std::string m_content;
};
