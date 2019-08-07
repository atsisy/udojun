#include <SFML/Graphics.hpp>
#include <SFML/System/Utf.hpp>
#include "utility.hpp"

std::wstring *util::utf8_str_to_widechar_str(std::string str)
{
        std::wstring *wstr = new std::wstring;
        str.push_back('\0');
        wstr->reserve(str.size());
        sf::Utf<8>::toWide(std::begin(str), std::end(str), std::begin(*wstr), 0);
        return wstr;
}
