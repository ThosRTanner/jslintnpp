#pragma once

#include <string>

class Version_Info
{
  public:
    Version_Info() noexcept;

    Version_Info(std::wstring const &fileName);

    Version_Info(std::wstring const &fileName, std::string const &content);

    std::wstring GetFileName() const
    {
        return m_fileName;
    }

    std::string GetContent() const;

  private:
    std::wstring m_fileName;
    mutable std::string m_content;
};
