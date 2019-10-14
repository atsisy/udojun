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

        SoundElement::SoundElement(sf::SoundBuffer *sb, u64 now, i16 id)
        {
                this->start = now;
                sound.setBuffer(*sb);
                this->end = now + (60 * sb->getDuration().asSeconds());
                this->id = id;
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

        void SoundElement::config_loop(bool loop)
        {
                this->sound.setLoop(loop);
        }
        
        void SoundElement::config_volume(float volume)
        {
                this->sound.setVolume(volume);
        }

        i16 SoundElement::get_instance_id(void)
        {
                return this->id;
        }

        void SoundElement::override_instance_id(i16 new_id)
        {
                this->id = new_id;
        }

        SoundInformation::SoundInformation(SoundID id)
        {
                this->id = id;
                this->volume = 50.f;
                this->loop = false;
                this->instace_id = -1;
        }
        
        SoundInformation::SoundInformation(SoundID id, float volume, bool loop)
        {
                this->id = id;
                this->volume = volume;
                this->loop = loop;
                this->instace_id = -1;
        }
        
        bool operator ==(SoundInformation &info1, SoundInformation &info2)
        {
                return info1.id == info2.id &&
                        info1.loop == info2.loop &&
                                info1.volume == info2.loop;
        }


        SoundPlayer::SoundPlayer(std::string sound_data)
                : table(sound_data)
        {
                for(int i = 0;i < 255;i++){
                        sound_pool.push_back(new SoundElement(table[SELECTING_SOUND], 0, -1));
                }
        }

        i16 SoundPlayer::add(SoundInformation info)
        {
                static i16 instance_id;
                info.instace_id = instance_id++;
                registered.push_back(info);
                return info.instace_id;
        }

        i16 SoundPlayer::stop(i16 instance_id)
        {
                auto it = std::find_if(std::begin(sound_pool), std::end(sound_pool),
                                       [&](SoundElement *p){ return p->get_instance_id() == instance_id; });
                if(it != std::end(sound_pool)){
                        (*it)->stop();
                        return instance_id;
                }

                return -1;
        }

        void SoundPlayer::flush(u64 now)
        {
                std::unique(std::begin(registered), std::end(registered));
                
                for(SoundInformation info : registered){
                        SoundElement *p = sound_pool.front();
                        sound_pool.pop_front();
                        p->stop();
                        p->override_instance_id(info.instace_id);
                        p->reset_buffer(table[info.id], now);
                        p->config_loop(info.loop);
                        p->config_volume(info.volume);
                        p->play();
                        sound_pool.push_back(p);
                }

                registered.clear();
        }
}
