#include "game_system.hpp"
#include "picojson.h"
#include <fstream>

namespace game_system {

        
        std::vector<SaveData> load_save_data(std::string json_file)
        {
                std::vector<SaveData> ret;
                std::ifstream ifs(json_file, std::ios::in);
                picojson::value v;

                std::cout << "Loading save data from " << json_file << "..." << std::endl;

                if (ifs.fail()) {
                        std::cerr << "failed to open json file" << std::endl;
                        exit(1);
                }

                const std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                ifs.close();

                const std::string err = picojson::parse(v, json);
                if (err.empty() == false) {
                        std::cerr << err << std::endl;
                        exit(1);
                }

                auto &obj = v.get<picojson::object>();
                picojson::array &data_array = obj["save_data"].get<picojson::array>();

                for(auto elem : data_array){
                        auto &&elem_obj = elem.get<picojson::object>();
                        ret.emplace_back(
                                elem_obj["name"].get<std::string>(),
                                ScoreInformation(-1.f,
                                                 elem_obj["score"].get<double>(),
                                                 -1,
                                                 elem_obj["hit"].get<double>()));
                }

                std::cout << "Loading is done." << std::endl;

                return ret;
        }
        
        void store_save_data(std::string json_file, SaveData save_data)
        {
                std::ifstream ifs(json_file, std::ios::in);
                picojson::value v;

                std::cout << "Saving result data to " << json_file << "..." << std::endl;

                if (ifs.fail()) {
                        std::cerr << "failed to open json file" << std::endl;
                        exit(1);
                }

                const std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                ifs.close();

                const std::string err = picojson::parse(v, json);
                if (err.empty() == false) {
                        std::cerr << err << std::endl;
                        exit(1);
                }

                auto &obj = v.get<picojson::object>();
                picojson::array &data_array = obj["save_data"].get<picojson::array>();
                ScoreInformation &&info = save_data.get_score_information();

                picojson::object save_object;
                save_object.insert(std::make_pair("score", picojson::value((double)info.score.get_current())));
                save_object.insert(std::make_pair("name", picojson::value(save_data.get_name())));
                save_object.insert(std::make_pair("hit", picojson::value((double)info.hit.get_current())));

                data_array.push_back(picojson::value(save_object));

                std::ofstream ofs(json_file, std::ios::out);
                ofs << v.serialize();
                ofs.close();

                std::cout << "Saving is done." << std::endl;
        }

}
