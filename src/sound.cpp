#include "sound.hpp"
#include <iostream>
#include "picojson.h"
#include <fstream>
#include <algorithm>

namespace sound {
        
        SoundTable::SoundTable(std::string json_path)
        {
                std::ifstream ifs(json_path, std::ios::in);

                std::cout << "Loading sound data listed in " << json_path << "..." << std::endl;

                if (ifs.fail()) {
                        DEBUG_PRINT_HERE();
                        std::cerr << "failed to read json file" << std::endl;
                        exit(1);
                }

                const std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                ifs.close();

                picojson::value v;
                const std::string err = picojson::parse(v, json);
                if (err.empty() == false) {
                        std::cerr << err << std::endl;
                        exit(1);
                }

                picojson::object& obj = v.get<picojson::object>();

                for(const auto &[key, val] : obj){
                        sf::SoundBuffer *sb = new sf::SoundBuffer;
                        std::cout << "\t" << val.to_str() << "...";
                        if(!sb->loadFromFile(val.to_str())){
                                exit(0);
                        }
                        std::cout << "done!!" << std::endl;
                        table.emplace(str_to_sdid(key.c_str()), sb);
                }

                std::cout << "All sound data have been loaded." << std::endl;
        }

        sf::SoundBuffer *SoundTable::operator [](SoundID id)
        {
		try {
			return table.at(id);
		} catch (std::out_of_range &e) {
                        DEBUG_PRINT_HERE();
                        std::cerr << "ERROR: " << e.what() << std::endl;
                }

                return nullptr;
	}

        SoundElement::SoundElement(sf::SoundBuffer *sb, u64 now)
        {
                this->start = now;
                sound.setBuffer(*sb);
                this->end = now + (60 * sb->getDuration().asSeconds());
        }

        SoundElement::~SoundElement(void)
        {
        }

        u64 SoundElement::get_start_time(void)
        {
                return start;
        }

        u64 SoundElement::get_end_time(void)
        {
                return end;
        }

        void SoundElement::play(void)
        {
                sound.play();
        }

        void SoundElement::stop(void)
        {
                sound.stop();
        }

        void SoundElement::reset_buffer(sf::SoundBuffer *sb, u64 now)
        {
                sound.resetBuffer();
                sound.setBuffer(*sb);
                start = now;
                this->end = now + (60 * sb->getDuration().asSeconds());
        }

        SoundPlayer::SoundPlayer(std::string sound_data)
                : table(sound_data)
        {
                for(int i = 0;i < 255;i++){
                        sound_pool.push_back(new SoundElement(table[SELECTING_SOUND], 0));
                }
        }

        void SoundPlayer::add(SoundID id)
        {
                registered.push_back(id);
        }

        void SoundPlayer::flush(u64 now)
        {
                std::unique(std::begin(registered), std::end(registered));
                
                for(SoundID id : registered){
                        SoundElement *p = sound_pool.front();
                        sound_pool.pop_front();
                        p->stop();
                        p->reset_buffer(table[id], now);
                        p->play();
                        sound_pool.push_back(p);
                }

                registered.clear();
        }
}
