#pragma once

#include <list>
#include <map>
#include <string>

class Profile_Handler
{
  public:
    Profile_Handler(std::wstring const &);

    ~Profile_Handler();

    /** Get the value of a key in a section.
     *
     * This returns the supplied default value if the key doesn't exist in the
     * section.
     */
    int get_int_value(
        std::wstring const &section, std::wstring const &key, int def_value = 0
    ) const;

    /** Get the value of a key in a section.
     *
     * This returns the supplied default value if the key doesn't exist in the
     * section.
     */
    std::wstring get_str_value(
        std::wstring const &section, std::wstring const &key,
        std::wstring const &def_value = L""
    ) const;

    /** Set the value of a key in a section. */
    void set_str_value(
        std::wstring const &section, std::wstring const &key,
        std::wstring const &value
    );

    //JSLint specific
    /** Get the build version */
    int get_build_version() const
    {
        return get_int_value(JSLint_Group_Name, JSLint_Build_Key);
    }

    // JSLint specific
    static constexpr auto Min_Version_Build = 110;

  private:
    std::wstring profile_file_;
    std::map<std::wstring, std::map<std::wstring, std::wstring>> values_;

    std::list<std::wstring> get_section_list(
        wchar_t const *section, wchar_t *pointer
    ) const;

    // JSLint specific
    static constexpr auto JSLint_Group_Name = L"JSLint";
    // JSLint specific
    static constexpr auto JSLint_Build_Key = L"build";
};
