#include "gm.hpp"
#include <fstream>
#include "picojson.h"

SaveSceneMaster::SaveSceneMaster(GameData *game_data, ScoreInformation info)
        : keyboard(sf::Vector2f(200, 400), game_data->get_font(JP_DEFAULT), 0),
          save_data(info)
{
        keyboard.register_handler_function("OK",
                                           [&, this](key::KeyStatus status){
                                                   if(status & key::KEY_FIRST_PRESSED){
                                                           std::cout << keyboard.get_buffer() << std::endl;
                                                           this->save_as_json("test_out.json", keyboard.get_buffer(), save_data);
                                                           keyboard.clear_buffer();
                                                           this->game_state = START;
                                                   }
                                           });

        create_view("keyboard",
		    sf::FloatRect(0.f, 0.f, 1366.f, 768.f),
                    sf::FloatRect(0.f, 0.f, 1.f, 1.f));

        game_state = SAVE;
}

void SaveSceneMaster::pre_process(sf::RenderWindow &window)
{
        keyboard.check();
        keyboard.move(get_count());

        update_count();
}

void SaveSceneMaster::drawing_process(sf::RenderWindow &window)
{
        post_draw_request_vargs("keyboard", &keyboard);

        switch_view("keyboard", window);
        get_view("keyboard")->flush_draw_requests(window);
}

GameState SaveSceneMaster::post_process(sf::RenderWindow &window)
{
        return game_state;
}

void SaveSceneMaster::save_as_json(std::string out_file, std::string name, ScoreInformation info)
{
        std::ifstream ifs(out_file, std::ios::in);
        picojson::value v;

        std::cout << "Saving result data to " << out_file << "..." << std::endl;

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

        picojson::object save_object;
        save_object.insert(std::make_pair("score", picojson::value((double)info.score.get_current())));
        save_object.insert(std::make_pair("name", picojson::value(name)));

        data_array.push_back(picojson::value(save_object));

        std::ofstream ofs(out_file, std::ios::out);
        ofs << v.serialize();
        ofs.close();

        std::cout << "Saving is done." << std::endl;
        
}
