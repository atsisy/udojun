#pragma once

#include <SFML/Audio.hpp>
#include <unordered_map>
#include "utility.hpp"
#include <deque>

namespace sound {

        enum SoundID {
                SELECTING_SOUND = 0,
                SHOOT1,
                UNKNOWN_SDID,
        };

        inline SoundID str_to_sdid(const char *str)
        {
                str_to_idx_sub(str, SELECTING_SOUND);
                str_to_idx_sub(str, SHOOT1);

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
                u64 start;
                u64 end;
                sf::Sound sound;
                
        public:
                SoundElement(sf::SoundBuffer *sb, u64 now);
                ~SoundElement(void);
                u64 get_start_time(void);
                u64 get_end_time(void);
                void play(void);
                void stop(void);
                void reset_buffer(sf::SoundBuffer *sb, u64 now);
        };

        class SoundPlayer {
        private:
                sf::Sound sound;
                SoundTable table;
                std::vector<SoundID> registered;
                std::deque<SoundElement *> sound_pool;
                
        public:
                SoundPlayer(std::string sound_data);
                void add(SoundID id);
                void flush(u64 now);
        };

}
