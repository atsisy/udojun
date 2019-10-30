#include "gm.hpp"

CreditSceneMaster::CreditSceneMaster(GameData *game_data)
        : game_state(CREDIT)
{
        text_objects.push_front(new DynamicText(L"製作スタッフ", game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN1,
                                                sf::Vector2f(500, 100),
                                                mf::stop, rotate::stop, get_count(), 36));
        text_objects.push_front(new DynamicText(L"プログラム/絵/楽曲アレンジ", game_data->get_font(JP_DEFAULT),
                                           GLYPH_DESIGN2,
                                           sf::Vector2f(500, 150),
                                           mf::stop, rotate::stop, get_count(), 26));
        text_objects.push_front(new DynamicText(L"空き地", game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN2,
                                                sf::Vector2f(550, 200),
                                                mf::stop, rotate::stop, get_count(), 30));

        text_objects.push_front(new DynamicText(L"原作", game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN1,
                                                sf::Vector2f(500, 350),
                                                mf::stop, rotate::stop, get_count(), 40));
        text_objects.push_front(new DynamicText(L"上海アリス幻樂団　東方Project", game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN2,
                                                sf::Vector2f(500, 400),
                                                mf::stop, rotate::stop, get_count(), 30));

        text_objects.push_front(new DynamicText(L"楽曲アレンジ元", game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN1,
                                                sf::Vector2f(500, 510),
                                                mf::stop, rotate::stop, get_count(), 30));
        text_objects.push_front(new DynamicText(L"タイトル曲: 東方いつものテーマアレンジ", game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN2,
                                                sf::Vector2f(500, 560),
                                                mf::stop, rotate::stop, get_count(), 26));
        text_objects.push_front(new DynamicText(L"道中曲: 「シンデレラケージ　～ Kagome-Kagome」アレンジ",
                                                game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN2,
                                                sf::Vector2f(500, 610),
                                                mf::stop, rotate::stop, get_count(), 26));
        text_objects.push_front(new DynamicText(L"ボス曲: 「狂気の瞳　～ Invisible Full Moon」アレンジ",
                                                game_data->get_font(JP_DEFAULT),
                                                GLYPH_DESIGN2,
                                                sf::Vector2f(500, 660),
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

void CreditSceneMaster::prepare_for_next_scene(void)
{
        game_state = START;
}

void CreditSceneMaster::pre_process(sf::RenderWindow &window)
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

void CreditSceneMaster::drawing_process(sf::RenderWindow &window)
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

GameState CreditSceneMaster::post_process(sf::RenderWindow &window)
{
        return game_state;
}
