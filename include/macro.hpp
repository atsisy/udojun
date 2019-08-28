#pragma once

#include "move_func.hpp"

enum MacroID {
	CIRCLE = 0,
        ELLIPSE,
        UDON_ELLIPSE,
	HART,
        UDON_CIRCLE1,
	UDON_TSUJO1,
	UDON_TSUJO2,
        UDON_SPELL1,
        UDON_SPELL2,
        UDON_SPELL4,
	N_WAY,
        RANDOM_CIRCLES,
	UNKNOWN_MACRO,
        MULTI_SHOT,
        JUNKO_SHOT_FAST_LV1,
        JUNKO_SHOT_FAST_LV2,
        JUNKO_SHOT_SLOW_LV1,
};

inline MacroID str_to_macroid(const char *str)
{
        if(!strcmp(str, enum_to_str(CIRCLE))){
                return CIRCLE;
        }else if(!strcmp(str, enum_to_str(HART))){
                return HART;
        }else if(!strcmp(str, enum_to_str(N_WAY))){
                return N_WAY;
	} else if (!strcmp(str, enum_to_str(UDON_TSUJO1))) {
		return UDON_TSUJO1;
	} else if (!strcmp(str, enum_to_str(UDON_TSUJO2))) {
		return UDON_TSUJO2;
	}

        str_to_idx_sub(str, RANDOM_CIRCLES);
        str_to_idx_sub(str, ELLIPSE);
        str_to_idx_sub(str, UDON_ELLIPSE);
        str_to_idx_sub(str, UDON_SPELL1);
        str_to_idx_sub(str, UDON_SPELL2);
        str_to_idx_sub(str, UDON_SPELL4);
        str_to_idx_sub(str, MULTI_SHOT);
        str_to_idx_sub(str, JUNKO_SHOT_FAST_LV1);
        str_to_idx_sub(str, JUNKO_SHOT_FAST_LV2);
        str_to_idx_sub(str, JUNKO_SHOT_SLOW_LV1);
        str_to_idx_sub(str, UDON_CIRCLE1);

	std::cout << "Unknown Macro ID: " << str << std::endl;
        
        return UNKNOWN_MACRO;
}


namespace macro {
        std::vector<BulletData *> expand_macro(picojson::object &data);
        std::vector<BulletData *> expand_dynamic_macro(picojson::object &data, DrawableCharacter *running_char, BulletData *bullet_data);
        std::vector<BulletData *> circle(sf::Vector2f origin, float r, u8 num, u64 time, float phase);
        std::vector<BulletData *> ellipse(sf::Vector2f origin, float r, float a, float b,
                                          u8 num, u64 time, float phase);
        std::vector<BulletData *> hart(sf::Vector2f origin, float r, u8 num, u64 time);
	std::vector<BulletData *> udon_tsujo1(sf::Vector2f origin, u64 time);
        
        std::vector<BulletData *> odd_n_way(sf::Vector2f origin,
                                            float r, float toward,
                                            float unit_rad, u8 num,
                                            u64 time, float speed);
	std::vector<BulletData *> even_n_way(sf::Vector2f origin, float r,
                                             float toward, float unit_rad, u8 num, u64 time, float speed);
        std::vector<BulletData *> n_way(sf::Vector2f origin, float r,
                                        float toward, float unit_rad, u8 num, u64 time, float speed);
        std::vector<BulletData *> udon_circle(sf::Vector2f origin, float speed, float r,
                                              u8 num, u64 time, float phase);
	std::vector<BulletData *> udon_circle2(sf::Vector2f origin, float speed,
					       float r, u16 num, u64 time,
					       float phase, float unit_rad);
        std::vector<BulletData *> random_circles(u16 circle_num, u16 num,
                                                 float speed, u64 time, u16 distance);
        std::vector<BulletData *> udon_spellcard1(sf::Vector2f origin, sf::Vector2f target,
                                                  float speed, u64 change_time, u64 time,
                                                  float r);
        std::vector<BulletData *> udon_spellcard2(float speed,
                                                  u64 enable_time, u64 disable_time, u64 time, float r, u64 num);

        /**
         * 第一引数: テクスチャID
         * 第二引数: 円の原点
         * 第三引数: スピード
         * 第四引数: 初めに弾幕が有効な時間
         * 第五引数: 弾幕が無効化される時間
         * 第六引数: 円を何個作るか
         * 第七引数: マクロが実行される時間
         * 第八引数: 円と円の生成が行われる間の時間
         * 第九引数: 円の初期半径
         * 第十引数: 円を形作る弾の数
         */
        std::vector<BulletData *> udon_spellcard4(TextureID txid, sf::Vector2f origin, float speed,
                                                  u64 enable_time, u64 disable_time, u64 cast_times,
                                                  u64 time, u64 time_offset, float r, u64 num);
        
        /**
         * 第一引数: テクスチャID
         * 第二引数: 弾丸の開始場所
         * 第三引数: 弾丸の数
         * 第四引数: スピード
         * 第五引数: 角度
         * 第六引数: 弾丸毎に変更されるスピード
         * 第七引数: マクロが始まる時刻
         * 第八引数: 弾幕間の空き時間
         */
        std::vector<BulletData *> multi_shot(TextureID txid, sf::Vector2f begin, u16 num,
                                             float speed, float angle,
                                             float bias,
                                             u64 time, u64 distance);
        
        std::vector<BulletData *> junko_shot_fast_lv1(TextureID txid, sf::Vector2f center, float speed, u64 time);
        std::vector<BulletData *> junko_shot_fast_lv2(TextureID txid1, TextureID txid2,
                                                      sf::Vector2f center, float speed, u64 time);
}
