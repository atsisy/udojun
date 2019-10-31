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

        SoundElement::SoundElement(sf::SoundBuffer *sb, u64 now, i16 id, SoundID sound_id, i8 priority)
        {
                this->start = now;
                sound.setBuffer(*sb);
                this->end = now + (60 * sb->getDuration().asSeconds());
                this->id = id;
                this->priority = priority;
                this->sound_id = sound_id;
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

        void SoundElement::set_priority(i8 priority)
        {
                this->priority = priority;
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

        bool SoundElement::priority_compare(SoundElement *p1, SoundElement *p2)
        {
                if(p1->priority != p2->priority){
                        return p1->priority > p2->priority;
                }else{
                        sf::SoundSource::Status p1_status = p1->sound.getStatus();
                        sf::SoundSource::Status p2_status = p2->sound.getStatus();

                        switch(p1_status){
                        case sf::SoundSource::Status::Stopped:
                                // p1が浮き上がるように
                                return false;
                        case sf::SoundSource::Status::Paused:
                                switch(p2_status){
                                case sf::SoundSource::Status::Stopped:
                                        return true;
                                case sf::SoundSource::Status::Playing:
                                case sf::SoundSource::Status::Paused:
                                        return false;
                                }
                        case sf::SoundSource::Status::Playing:
                                // p2が浮き上がるように
                                return true;
                        }
                }

                // 来ないはず
                return false;
        }
        
        void SoundElement::set_sound_id(SoundID id)
        {
                this->sound_id = id;
        }

        SoundID SoundElement::get_sound_id(void)
        {
                return this->sound_id;
        }
        
        SoundInformation::SoundInformation(SoundID id, i8 priority)
        {
                this->id = id;
                this->volume = 50.f;
                this->loop = false;
                this->instace_id = -1;
                this->priority = priority;
        }
        
        SoundInformation::SoundInformation(SoundID id, float volume, bool loop, i8 priority)
        {
                this->id = id;
                this->volume = volume;
                this->loop = loop;
                this->instace_id = -1;
                this->priority = priority;
        }
        
        bool operator ==(SoundInformation &info1, SoundInformation &info2)
        {
                return info1.id == info2.id &&
                        info1.loop == info2.loop &&
                        info1.volume == info2.loop &&
                        info1.priority == info2.priority;
        }


        SoundPlayer::SoundPlayer(std::string sound_data)
                : table(sound_data), sound_pool(SoundElement::priority_compare)
        {
                SoundElement *p;
                for(int i = 0;i < 255;i++){
                        p = new SoundElement(table[SELECTING_SOUND], 0, -1, NO_SOUND);
                        sound_pool.push(p);
                        union_sound_pool[i] = p;
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
                auto it = std::find_if(std::begin(union_sound_pool), std::end(union_sound_pool),
                                       [&](SoundElement *p){ return p->get_instance_id() == instance_id; });
                if(it != std::end(union_sound_pool)){
                        (*it)->stop();
                        return instance_id;
                }

                return -1;
        }

        i16 SoundPlayer::already_played(SoundID id)
        {
                for(auto p : union_sound_pool){
                        if(p->get_sound_id() == id){
                                return p->get_instance_id();
                        }
                }
                return -1;
        }

        i16 SoundPlayer::add_if_not_played(SoundInformation info)
        {
                i16 ret = already_played(info.id);
                if(ret != -1){
                        /*
                         * 既に再生されている
                         */
                        return ret;
                }

                /*
                 * 再生されていなかったので、add
                 */
                return add(info);
        }

        void SoundPlayer::flush(u64 now)
        {
                std::unique(std::begin(registered), std::end(registered));
                
                for(SoundInformation info : registered){
                        SoundElement *p = sound_pool.top();
                        sound_pool.pop();
                        p->stop();
                        p->override_instance_id(info.instace_id);
                        p->set_priority(info.priority);
                        p->reset_buffer(table[info.id], now);
                        p->config_loop(info.loop);
                        p->config_volume(info.volume);
                        p->set_sound_id(info.id);
                        p->play();
                        sound_pool.push(p);
                }

                registered.clear();
        }
}
