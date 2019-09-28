#include "key_input.hpp"
#include <iostream>

key::KeyboardListener::KeyboardListener(void)
{
	listening = { std::make_pair(VKEY_1, false),
                      std::make_pair(VKEY_2, false),
                      std::make_pair(VKEY_3, false),
                      std::make_pair(VKEY_4, false),
                      std::make_pair(ARROW_KEY_UP, false),
                      std::make_pair(ARROW_KEY_RIGHT, false),
                      std::make_pair(ARROW_KEY_DOWN, false),
                      std::make_pair(ARROW_KEY_LEFT, false),
                      std::make_pair(MOD_VKEY_1, false),
                      std::make_pair(MOD_VKEY_2, false) };
	for (auto &[sym, on] : listening) {
		func_map.emplace(
			sym, std::vector<std::function<void(KeyStatus)> >());
	}
}

void key::KeyboardListener::add_key_event(
	KeySymbol sym, std::function<void(KeyStatus)> fn)
{
        func_map[sym].push_back(fn);
}

void key::KeyboardListener::key_update(void)
{
        for (auto &[sym, prev_key_state] : listening) {
                bool current = KeyboardTypeCoordinator::is_pressed(GENERAL_KEYBOARD, sym) ||
                        KeyboardTypeCoordinator::is_pressed(PS3_JOYSTICK, sym);

                /*
                 * 前回とキーの状態が異なる
                 */
                if(prev_key_state != current){
                        /*
                         * キーが押された
                         */
                        if(current == true){
                                /*
                                 * 初めて入力されたステータスを渡して関数実行
                                 */
                                for(auto &f : func_map[sym]){
                                        f(KEY_DOWN | KEY_FIRST_PRESSED);
                                }
                        }else{
                                /*
                                 * キーが離された
                                 */
                                for(auto &f : func_map[sym]){
                                        f(KEY_UP | KEY_RELEASED);
                                }
                        }
                }else{
                        /*
                         * キーが押されていた
                         */
                        if(current == true){
                                /*
                                 * 継続入力としてステータスを渡して関数実行
                                 */
                                for(auto &f : func_map[sym]){
                                        f(KEY_DOWN | KEY_KEEP_PRESSED);
                                }
                        }else{
                                /*
                                 * キーが離され続ける場合は特に何もしない
                                 */
                        }
                }

                prev_key_state = current;
	}
}


