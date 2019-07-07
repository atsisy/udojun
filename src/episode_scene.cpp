#include "gm.hpp"
#include "effect.hpp"
#include <fstream>
#include <cstdlib>
#include <locale> 
#include <codecvt>
#include <SFML/System/Utf.hpp>

EpisodeController::EpisodeController(const char *path, sf::Font *font)
        : page_index(0)
{
        std::vector<wchar_t *> buf;
        std::ifstream ifs(path);
        std::string str;

        if (ifs.fail()){
                std::cerr << "failed to load episode text: " << path << std::endl;
        }

        std::cout << "Loading opening episode text..." << std::endl;
        setlocale(LC_CTYPE, "Japanese");
	while (getline(ifs, str)){
		if(str == std::string("#!\\clear")){
                        episode.push_back(new NovelText(
                                                  buf,
                                                  font,
                                                  sf::Vector2f(100, 500), 20, 20));
                        buf.clear();
                }else{
                        str.push_back('\0');
                        std::cout << str << std::endl;
                        std::wstring *wstr = new std::wstring;
                        wstr->reserve(str.size());
                        sf::Utf<8>::toWide(std::begin(str), std::end(str), std::begin(*wstr), 0);
                        buf.push_back(wstr->data());
                }
        }
}

void EpisodeController::next(void)
{
        page_index++;
}

void EpisodeController::back(void)
{
        page_index--;
}

void EpisodeController::top(void)
{
        page_index = 0;
}

void EpisodeController::end(void)
{
        page_index = episode.size() - 1;
}

NovelText *EpisodeController::get_current_page(void)
{
        return episode[page_index];
}

OpeningEpisodeSceneMaster::OpeningEpisodeSceneMaster(GameData *game_data)
	: background(GameMaster::texture_table[SAMPLE_BACKGROUND1],
		     sf::Vector2f(0, 0), sf::IntRect(0, 0, 1366, 768),
		     sf::Vector2f(1, 1)),
          episode("opening_episode.txt", game_data->get_font(JP_DEFAULT))
{
        game_state = OPENING_EPISODE;

        auto p = new MoveObject(GameMaster::texture_table[BLACK_ANTEN],
                                sf::Vector2f(0, 0),
                                mf::stop,
                                get_count());
        add_animation_object(p);
        p->add_effect({ effect::fade_out(100) });

        key_listener.add_key_event(key::VKEY_1, [&](key::KeyStatus status){
                                                        if(status & key::KEY_FIRST_PRESSED){
                                                                episode.next();
                                                        }
                                                });
        
}

void OpeningEpisodeSceneMaster::pre_process(sf::RenderWindow &window)
{
        key_listener.key_update();
        flush_effect_buffer(get_count());
}

void OpeningEpisodeSceneMaster::drawing_process(sf::RenderWindow &window)
{
        background.draw(window);
        episode.get_current_page()->draw(window);
        draw_animation(window);
}

GameState OpeningEpisodeSceneMaster::post_process(sf::RenderWindow &window)
{
	update_count();
        return game_state;
}
