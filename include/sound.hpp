#pragma once

#include <SFML/Audio.hpp>
#include <unordered_map>
#include "utility.hpp"
#include <queue>
#include <array>

namespace sound {

        enum SoundID {
                SELECTING_SOUND = 0,
                SHOOT1,
                UNKNOWN_SDID,
                TITLE_BGM,
                JUNKO_HIT,
                BGM2,
                BGM3,
                SE_ITEM,
                SE_LARGE_ITEM,
                SE_PLAYER_SPELLCARD,
        };

        inline SoundID str_to_sdid(const char *str)
        {
                str_to_idx_sub(str, SELECTING_SOUND);
                str_to_idx_sub(str, SHOOT1);
                str_to_idx_sub(str, TITLE_BGM);
                str_to_idx_sub(str, JUNKO_HIT);
                str_to_idx_sub(str, BGM2);
                str_to_idx_sub(str, BGM3);

                str_to_idx_sub(str, SE_ITEM);
                str_to_idx_sub(str, SE_LARGE_ITEM);

                str_to_idx_sub(str, SE_PLAYER_SPELLCARD);

                return UNKNOWN_SDID;
        }

        class SoundTable {
        private:
                std::unordered_map<SoundID, sf::SoundBuffer *> table;

        public:
                SoundTable(std::string json_path);
                sf::SoundBuffer *operator[](SoundID id);
        };

        class SoundElement {
        private:
                i8 id;
                i8 priority;
                u64 start;
                u64 end;
                sf::Sound sound;
                
        public:
                SoundElement(sf::SoundBuffer *sb, u64 now, i16 id, i8 priority = 0);
                ~SoundElement(void);
                u64 get_start_time(void);
                u64 get_end_time(void);
                void play(void);
                void stop(void);
                void reset_buffer(sf::SoundBuffer *sb, u64 now);
                void config_loop(bool loop);
                void config_volume(float volume);
                void set_priority(i8 priority);

                i16 get_instance_id(void);
                void override_instance_id(i16 new_id);

                static bool priority_compare(SoundElement *p1, SoundElement *p2);
        };

        class SoundInformation {
        public:
                SoundID id;
                float volume;
                bool loop;
                i16 instace_id;
                i8 priority;

                SoundInformation(SoundID id, i8 priority = 0);
                SoundInformation(SoundID id, float volume, bool loop, i8 priority = 0);

        };

        bool operator ==(SoundInformation &info1, SoundInformation &info2);
        
        class SoundPlayer {
        private:
                sf::Sound sound;
                SoundTable table;
                std::vector<SoundInformation> registered;
                std::priority_queue<SoundElement *,
                                    std::vector<SoundElement *>,
                                    std::function<bool(SoundElement *, SoundElement *)>> sound_pool;
                std::array<SoundElement *, 256> union_sound_pool;
                
        public:
                SoundPlayer(std::string sound_data);
                i16 add(SoundInformation info);
                void flush(u64 now);
                i16 stop(i16 instance_id);
        };

}
