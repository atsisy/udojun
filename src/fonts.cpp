#include "fonts.hpp"
#include <iostream>
#include "picojson.h"
#include <fstream>

FontContainer::FontContainer(const char *path)
{
        std::ifstream ifs(path, std::ios::in);
        sf::Font *font;

        std::cout << "Loading fonts..." << std::endl;
        if (ifs.fail()) {
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
                font = new sf::Font;
                std::cout << "\t" << val.to_str() << "...";
                if(!font->loadFromFile(val.to_str())){
                        std::cerr << "Failed to load font" << std::endl;
                        exit(0);
                }
                std::cout << "done!!" << std::endl;
                this->font_map.emplace(str_to_fid(key.c_str()), font);
        }

        std::cout << "All fonts have been loaded." << std::endl;
}
