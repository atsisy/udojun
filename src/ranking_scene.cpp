#include "gm.hpp"
#include "game_system.hpp"

RankingSceneMaster::RankingSceneMaster(GameData *game_data)
        : game_state(RANKING)
{
        std::vector<SaveData> &&save_data_vec = game_system::load_save_data("test_out.json");

        sf::Vector2f easy_pos(70, 245);
        sf::Vector2f hard_pos(715, 245);

        std::sort(std::begin(save_data_vec), std::end(save_data_vec),
                  [](SaveData &a, SaveData &b){
                          return a.get_score_information().score.get_current() >  b.get_score_information().score.get_current(); });

        char score_buf[2048] = { 0 };
        
        for(SaveData &data : save_data_vec){
                sf::Vector2f &pos = (data.get_score_information().level == LEVEL_EASY) ?
                        easy_pos : hard_pos;
                
                text_objects.push_front(
                        new DynamicText(
                                util::utf8_str_to_widechar_str(
                                        data.get_name())->data(),
                                game_data->get_font(JP_DEFAULT),
                                GLYPH_DESIGN2,
                                pos + sf::Vector2f(500, 0),
                                mf::ratio_step(pos, 0.1),
                                rotate::stop, get_count(), 24));

                sprintf(score_buf, "%11ld", data.get_score_information().score.get_current());
                text_objects.push_front(
                        new DynamicText(
                                util::utf8_str_to_widechar_str(
                                        std::string(score_buf))->data(),
                                game_data->get_font(JP_DEFAULT),
                                GLYPH_DESIGN2,
                                pos + sf::Vector2f(500, 0),
                                mf::ratio_step(pos + sf::Vector2f(100, 0), 0.1),
                                rotate::stop, get_count(), 24));
                text_objects.push_front(
                        new DynamicText(
                                util::utf8_str_to_widechar_str(
                                        data.get_date().to_string())->data(),
                                game_data->get_font(JP_DEFAULT),
                                GLYPH_DESIGN2,
                                pos + sf::Vector2f(500, 0),
                                mf::ratio_step(pos + sf::Vector2f(300, 0), 0.1),
                                rotate::stop, get_count(), 24));
                pos.y += 32;
        }

        text_objects.push_front(new DynamicText(L"得点ランキング", game_data->get_font(JP_DEFAULT),
                                           GLYPH_DESIGN1,
                                           sf::Vector2f(550, 100),
                                           mf::stop, rotate::stop, get_count(), 40));

        text_objects.push_front(new DynamicText(L"兎級 Easy", game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN3,
                                                sf::Vector2f(250, 145),
                                                mf::stop, rotate::stop, get_count(), 30));
        text_objects.push_front(new DynamicText(L"狐級 Hard", game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN4,
                                                sf::Vector2f(880, 145),
                                                mf::stop, rotate::stop, get_count(), 30));
        text_objects.push_front(new DynamicText(L"名前", game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN2,
                                                sf::Vector2f(70, 200),
                                                mf::stop, rotate::stop, get_count(), 26));
        text_objects.push_front(new DynamicText(L"得点", game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN2,
                                                sf::Vector2f(230, 200),
                                                mf::stop, rotate::stop, get_count(), 26));
        text_objects.push_front(new DynamicText(L"日時", game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN2,
                                                sf::Vector2f(420, 200),
                                                mf::stop, rotate::stop, get_count(), 26));

        text_objects.push_front(new DynamicText(L"名前", game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN2,
                                                sf::Vector2f(720, 200),
                                                mf::stop, rotate::stop, get_count(), 26));
        text_objects.push_front(new DynamicText(L"得点", game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN2,
                                                sf::Vector2f(900, 200),
                                                mf::stop, rotate::stop, get_count(), 26));
        text_objects.push_front(new DynamicText(L"日時", game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN2,
                                                sf::Vector2f(1150, 200),
                                                mf::stop, rotate::stop, get_count(), 26));

        background = new MoveObject(
                GameMaster::texture_table[LIGHT_GLASS_BACKGROUND1],
                sf::Vector2f(0, 0),
                mf::stop,
                rotate::stop,
                get_count()
                );

        create_view("background",
		    sf::FloatRect(0.f, 0.f, 1366.f, 768.f),
                    sf::FloatRect(0.f, 0.f, 1.f, 1.f));

        create_view("effect",
		    sf::FloatRect(0.f, 0.f, 1366.f, 768.f),
                    sf::FloatRect(0.f, 0.f, 1.f, 1.f));
        
        create_view("text",
		    sf::FloatRect(0.f, 0.f, 1366.f, 768.f),
                    sf::FloatRect(0.f, 0.f, 1.f, 1.f));

        key_listener.add_key_event(key::VKEY_3,
                                   [this](key::KeyStatus status){
                                           this->prepare_for_next_scene();
                                   });
}

void RankingSceneMaster::prepare_for_next_scene(void)
{
        game_state = START;
}

void RankingSceneMaster::pre_process(sf::RenderWindow &window)
{
        if(util::generate_random() % 30 == 0){
                effect_objects.push_front(
                        new FallingLeaf(
                                sf::Vector2f(
                                        (util::generate_random() % 1366) - 500,
                                        -100),
                                get_count()));
        }
        
        for(auto p : text_objects){
                p->move(get_count());
                p->effect(get_count());
        }

        effect_objects.remove_if(
                [this](EffectableGroup *p)
                        {
                                if(p->get_position().x > 1366 || p->get_position().y > 768){
                                        delete p;
                                        return true;
                                }else{
                                        p->effect(get_count());
                                        return false;
                                }
                        });
        
        key_listener.key_update();
        timer_list.check_and_call(get_count());
        update_count();
}

void RankingSceneMaster::drawing_process(sf::RenderWindow &window)
{
        post_draw_request_vargs("background", background);
        post_draw_request("effect", effect_objects);
        post_draw_request("text", text_objects);

        switch_view("background", window);
        get_view("background")->flush_draw_requests(window);

        switch_view("effect", window);
        get_view("effect")->flush_draw_requests(window);
        
        switch_view("text", window);
        get_view("text")->flush_draw_requests(window);
}

GameState RankingSceneMaster::post_process(sf::RenderWindow &window)
{
        return game_state;
}
