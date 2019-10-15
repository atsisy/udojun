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
                TITLE_BGM,
                JUNKO_HIT,
        };

        inline SoundID str_to_sdid(const char *str)
        {
                str_to_idx_sub(str, SELECTING_SOUND);
                str_to_idx_sub(str, SHOOT1);
                str_to_idx_sub(str, TITLE_BGM);
                str_to_idx_sub(str, JUNKO_HIT);

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
                i16 id;
                u64 start;
                u64 end;
                sf::Sound sound;
                
        public:
                SoundElement(sf::SoundBuffer *sb, u64 now, i16 id);
                ~SoundElement(void);
                u64 get_start_time(void);
                u64 get_end_time(void);
                void play(void);
                void stop(void);
                void reset_buffer(sf::SoundBuffer *sb, u64 now);
                void config_loop(bool loop);
                void config_volume(float volume);

                i16 get_instance_id(void);
                void override_instance_id(i16 new_id);
        };

        class SoundInformation {
        public:
                SoundID id;
                float volume;
                bool loop;
                i16 instace_id;

                SoundInformation(SoundID id);
                SoundInformation(SoundID id, float volume, bool loop);

        };

        bool operator ==(SoundInformation &info1, SoundInformation &info2);
        
        class SoundPlayer {
        private:
                sf::Sound sound;
                SoundTable table;
                std::vector<SoundInformation> registered;
                std::deque<SoundElement *> sound_pool;
                
        public:
                SoundPlayer(std::string sound_data);
                i16 add(SoundInformation info);
                void flush(u64 now);
                i16 stop(i16 instance_id);
        };

}
