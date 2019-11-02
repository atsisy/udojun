#include "gm.hpp"
#include <fstream>
#include "picojson.h"
#include "effect.hpp"
#include "game_system.hpp"

SaveSceneMaster::SaveSceneMaster(GameData *game_data, ScoreInformation info)
        : keyboard(sf::Vector2f(200, 400), game_data->get_font(JP_DEFAULT), 0),
          save_data("", info, util::Date(nullptr))
{
        keyboard.set_buffer_length_limit(10);
        keyboard.register_handler_function("OK",
                                           [&, this](key::KeyStatus status){
                                                   if(status & key::KEY_FIRST_PRESSED){
                                                           std::cout << keyboard.get_buffer() << std::endl;
                                                           save_data.reset_name(keyboard.get_buffer());
                                                           save_as_json("test_out.json", save_data);
                                                           this->prepare_for_next_scene();
                                                   }
                                           });

        create_view("keyboard",
		    sf::FloatRect(0.f, 0.f, 1366.f, 768.f),
                    sf::FloatRect(0.f, 0.f, 1.f, 1.f));
        create_view("loading",
		    sf::FloatRect(0.f, 0.f, 1366.f, 768.f),
                    sf::FloatRect(0.f, 0.f, 1.f, 1.f));
        create_view("background",
		    sf::FloatRect(0.f, 0.f, 1366.f, 768.f),
                    sf::FloatRect(0.f, 0.f, 1.f, 1.f));
        create_view("effect",
		    sf::FloatRect(0.f, 0.f, 1366.f, 768.f),
                    sf::FloatRect(0.f, 0.f, 1.f, 1.f));
        
        display = new DynamicText(L"", game_data->get_font(JP_DEFAULT),
                                  GLYPH_DESIGN1,
                                  sf::Vector2f(250, 230),
                                  mf::stop, rotate::stop, get_count(), 40);

        objects.push_front(new DynamicText(L"得点を記録する", game_data->get_font(JP_DEFAULT),
                                     GLYPH_DESIGN1,
                                     sf::Vector2f(300, 100),
                                          mf::stop, rotate::stop, get_count(), 40));
;
        objects.push_front(
                new DynamicText(
                        util::utf8_str_to_widechar_str(
                                "結果 " +
                                std::string("\t")
                                +
                                std::to_string(save_data.get_score_information().score.get_current())
                                +
                                std::string("\t") +
                                save_data.get_date().to_string())->data(),
                        game_data->get_font(JP_DEFAULT),
                        GLYPH_DESIGN2,
                        sf::Vector2f(800, 200),
                        mf::ratio_step(sf::Vector2f(500, 200), 0.1),
                        rotate::stop, get_count(), 24));


        objects.push_front(new MoveObject(
                                   GameMaster::texture_table[NAVY_BACKGROUND1],
                                   sf::Vector2f(0, 0),
                                   mf::stop,
                                   rotate::stop,
                                   get_count()
                                   ));

        game_state = SAVE;
}

void SaveSceneMaster::pre_process(sf::RenderWindow &window)
{
        keyboard.check();
        keyboard.move(get_count());

        display->set_text(keyboard.get_buffer().data());

        for(auto p : objects){
                p->move(get_count());
                p->effect(get_count());
        }

        flush_effect_buffer(get_count());

        if(util::generate_random() % 10 == 0){
                effect_objects.push_front(
                        new RainWave(
                                sf::Vector2f(
                                        util::generate_random() % 1366,
                                        util::generate_random() % 768),
                                get_count()));
        }

        effect_objects.remove_if([this](EffectableGroup *p){
                                         if(!p->visible()){
                                                 return true;
                                         }else{
                                                 p->effect(get_count());
                                                 return false;
                                         }
                                 });
        
        timer_list.check_and_call(get_count());
        update_count();
}

void SaveSceneMaster::drawing_process(sf::RenderWindow &window)
{
        post_draw_request_vargs("keyboard", &keyboard, display);
        post_draw_request("background", objects);
        post_draw_request("effect", effect_objects);


        switch_view("background", window);
        get_view("background")->flush_draw_requests(window);

        switch_view("effect", window);
        get_view("effect")->flush_draw_requests(window);
        
        switch_view("keyboard", window);
        get_view("keyboard")->flush_draw_requests(window);

        switch_view("loading", window);
        draw_animation(window);
}

GameState SaveSceneMaster::post_process(sf::RenderWindow &window)
{
        return game_state;
}

void SaveSceneMaster::save_as_json(std::string out_file, SaveData save_data)
{
        game_system::store_save_data(out_file, save_data);
}

void SaveSceneMaster::prepare_for_next_scene(void)
{       
        timer_list.add_timer(
                [this](void){
                        auto p = new MoveObject(GameMaster::texture_table[BLACK_ANTEN],
                                                sf::Vector2f(0, 0),
                                                mf::stop,
                                                rotate::stop,
                                                get_count());
                        p->set_alpha(0);
                        add_animation_object(p);
                        p->add_effect({ effect::fade_in(100) });
                },
                60, get_count());
        timer_list.add_timer(
                [this](void){
                        this->game_state = START;
                },
                160, get_count());
}
