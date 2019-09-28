#pragma once

#include <unordered_map>
#include <functional>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Joystick.hpp>
#include "types.hpp"

namespace key {

        using JSButtonID = u32;
        
        enum KeySymbol {
                VKEY_1 = 0,
                VKEY_2,
                VKEY_3,
                VKEY_4,
                ARROW_KEY_UP,
                ARROW_KEY_RIGHT,
                ARROW_KEY_DOWN,
                ARROW_KEY_LEFT,
                MOD_VKEY_1,
                MOD_VKEY_2,
                UNDEFINED,
        };

        enum KeyboardType {
                GENERAL_KEYBOARD = 0,
                PS3_JOYSTICK,
        };

        using KeyStatus = u8;

        constexpr KeyStatus KEY_UP = 0x01;
        constexpr KeyStatus KEY_DOWN = 0x02;
        constexpr KeyStatus KEY_FIRST_PRESSED = 0x04;
        constexpr KeyStatus KEY_RELEASED = 0x08;
        constexpr KeyStatus KEY_KEEP_PRESSED = 0x10;
        
        inline KeySymbol keyboard_symbol(sf::Keyboard::Key key)
        {
                switch(key){
                case sf::Keyboard::Z:
                case sf::Keyboard::Enter:
                        return VKEY_1;
                case sf::Keyboard::X:
                        return VKEY_2;
                case sf::Keyboard::C:
                        return VKEY_3;
                case sf::Keyboard::Up:
                        return ARROW_KEY_UP;
                case sf::Keyboard::Right:
                        return ARROW_KEY_RIGHT;
                case sf::Keyboard::Down:
                        return ARROW_KEY_DOWN;
                case sf::Keyboard::Left:
                        return ARROW_KEY_LEFT;
                case sf::Keyboard::LShift:
                        return MOD_VKEY_1;
		case sf::Keyboard::RShift:
			return MOD_VKEY_2;
                default:
                        return UNDEFINED;
		}
        }

        class KeyboardTypeCoordinator {
        public:
                static std::vector<sf::Keyboard::Key> symbol_to_general_keyboard(KeySymbol sym)
                {
                        switch(sym){
                        case VKEY_1:
                                return { sf::Keyboard::Z, sf::Keyboard::Enter };
                        case VKEY_2:
                                return { sf::Keyboard::X };
                        case VKEY_3:
                                return { sf::Keyboard::C };
                        case ARROW_KEY_UP:
                                return { sf::Keyboard::Up };
                        case ARROW_KEY_RIGHT:
                                return { sf::Keyboard::Right };
                        case ARROW_KEY_DOWN:
                                return { sf::Keyboard::Down };
                        case ARROW_KEY_LEFT:
                                return { sf::Keyboard::Left };
                        case MOD_VKEY_1:
                                return { sf::Keyboard::LShift };
                        case MOD_VKEY_2:
                                return { sf::Keyboard::RShift };
                        default:
                                return { sf::Keyboard::Z };
                        }
                }
                
                static bool is_pressed_general_keyboard(sf::Keyboard::Key key)
                {
                        return sf::Keyboard::isKeyPressed(key);
                }

                static bool is_pressed_general_keyboard_vector(std::vector<sf::Keyboard::Key> key_vec)
                {
                        for(sf::Keyboard::Key key : key_vec){
                                if(is_pressed_general_keyboard(key))
                                        return true;
                        }

                        return false;
                }

                static bool judge_ps3_js_key_pressed(KeySymbol symbol)
		{
                        switch(symbol){
                        case VKEY_1:
                                return sf::Joystick::isButtonPressed(0, 1);
                        case VKEY_2:
                                return sf::Joystick::isButtonPressed(0, 2);
                        case VKEY_3:
                                return sf::Joystick::isButtonPressed(0, 0);
                        case ARROW_KEY_UP:
                                return sf::Joystick::getAxisPosition(0, sf::Joystick::Y) < -50;
                        case ARROW_KEY_RIGHT:
                                return sf::Joystick::getAxisPosition(0, sf::Joystick::X) > 50;
                        case ARROW_KEY_DOWN:
                                return sf::Joystick::getAxisPosition(0, sf::Joystick::Y) > 50;
                        case ARROW_KEY_LEFT:
                                return sf::Joystick::getAxisPosition(0, sf::Joystick::X) < -50;
                        case MOD_VKEY_1:
                                return sf::Joystick::isButtonPressed(0, 5);
                        case MOD_VKEY_2:
                                return sf::Joystick::isButtonPressed(0, 6);
                        default:
                                return false;
                        }
		}

		static bool is_pressed(KeyboardType type, KeySymbol sym)
                {
                        switch(type){
                        case GENERAL_KEYBOARD:
                                return is_pressed_general_keyboard_vector(
                                        symbol_to_general_keyboard(sym));
                        case PS3_JOYSTICK:
                                return judge_ps3_js_key_pressed(sym);
                        default:
                                return false;
                        }
                }

                static bool simple_pressed_check(KeySymbol sym)
                {
                        return is_pressed_general_keyboard_vector(
                                symbol_to_general_keyboard(sym)) || judge_ps3_js_key_pressed(sym);
                                
                }
        };

        class KeyboardListener {
        private:
                std::unordered_map<KeySymbol, bool> listening;
                std::unordered_map<KeySymbol, std::vector<std::function<void(KeyStatus)> > > func_map;
                
        public:
                KeyboardListener(void);
                void key_update(void);
                void add_key_event(KeySymbol sym, std::function<void(KeyStatus)> fn);
        };
}
