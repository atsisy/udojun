#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>

namespace key {
        enum KeySymbol {
                VKEY_1 = 0,
                VKEY_2,
                VKEY_3,
                VKEY_4,
                MOD_VKEY_1,
                MOD_VKEY_2,
                UNDEFINED,
        };

        enum KeyboardType {
                GENERAL_KEYBOARD = 0,
        };

        KeySymbol keyboard_symbol(sf::Keyboard::Key key)
        {
                switch(key){
                case sf::Keyboard::Z:
                        return VKEY_1;
                case sf::Keyboard::X:
                        return VKEY_2;
                case sf::Keyboard::C:
                        return VKEY_3;
                case sf::Keyboard::LShift:
                        return MOD_VKEY_1;
		case sf::Keyboard::RShift:
			return MOD_VKEY_2;
                default:
                        return UNDEFINED;
		}
        }
}
