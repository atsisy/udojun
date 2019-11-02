#include <iostream>
#include <fstream>
#include "gm.hpp"
#include "character.hpp"
#include "picojson.h"


#include <SFML/Window/Joystick.hpp>

TextureTable GameMaster::texture_table;
sound::SoundPlayer *GameMaster::sound_player;
util::xor128 util::generate_random;
GameConfig *GameMaster::game_config;
FpsCalculator GameMaster::fps_calc;
GameMasterPostObject *GameMaster::post_box;

GameData::GameData()
        : enemy_table({ "stage1_enemy.json" })
{
        font_container = new FontContainer("fonts.json");       
}

sf::Font *GameData::get_font(FontID id)
{
        return font_container->get(id);
}

EnemyCharacterMaterial *GameData::get_enemy_material(std::string name)
{
        return this->enemy_table.get(name);
}

GraphicBuffer::GraphicBuffer(sf::FloatRect window, sf::FloatRect viewport)
        : buffer(window), draw_requests(compare_depth)
{
        buffer.setViewport(viewport);
}

sf::View &GraphicBuffer::get_buffer(void)
{
        return buffer;
}

void GraphicBuffer::add_draw_request(DrawableComponent *p)
{
        draw_requests.push(p);
}

void GraphicBuffer::flush_draw_requests(sf::RenderWindow &window)
{
        while(draw_requests.size()){
                draw_requests.top()->draw(window);
                draw_requests.pop();
        }
}

GameMasterPostObject::GameMasterPostObject(void)
{}

GameMasterPostObject::~GameMasterPostObject(void)
{}


ScoreInformation::ScoreInformation(double power, u64 score, u64 graze, u64 _hit, GameLevel _level)
        : power(Score::MAX_POWER, power),
          score(Score::MAX_SCORE, score),
          graze(Score::MAX_GRAZE, graze),
          hit(Score::MAX_HIT, _hit),
          level(_level)
{}

RaceStatus::RaceStatus(void)
{
        hit_count = 0;
}

u64 RaceStatus::get_hit_count(void)
{
        return this->hit_count;
}

void RaceStatus::hit(void)
{
        hit_count++;
}

SaveData::SaveData(std::string _name, ScoreInformation _si, util::Date _date)
        : name(_name), score_info(_si), date(_date)
{}

std::string SaveData::get_name(void)
{
        return name;
}

ScoreInformation SaveData::get_score_information(void)
{
        return score_info;
}

void SaveData::reset_name(std::string _name)
{
        this->name = _name;
}

util::Date SaveData::get_date(void)
{
        return date;
}


StartToRace::StartToRace(GameLevel level,
                         std::vector<const char *> stage1_danmaku_file,
                         std::vector<const char *> stage1_road_file)
{
        this->level = level;
        this->stage1_danmaku_file = stage1_danmaku_file;
        this->stage1_road_file = stage1_road_file;
}

StartToRace::~StartToRace(void)
{}

FpsCalculator::FpsCalculator(void)
{
        this->last = 0.f;
}

void FpsCalculator::update(void)
{
        fps = 1.f / clock.restart().asSeconds();
}

float FpsCalculator::get_current_fps(void)
{
        return this->fps;
}

GameConfig::GameConfig(std::string json_path)
{
        std::ifstream ifs(json_path, std::ios::in);

        std::cout << "Loading game config written in " << json_path << "..." << std::endl;

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

        this->race_scene_time_offset = obj["race_scene_time_offset"].get<double>();

        std::cout << "Game config has been loaded." << std::endl;
}

GameMaster::GameMaster()
//        : window(sf::VideoMode::getFullscreenModes()[8], "udjn", sf::Style::Fullscreen)
        : window(sf::VideoMode(1366, 768), "udjn")
{
        window_handle = window.getSystemHandle();
        current_scene = nullptr;
        game_data = new GameData();
        sound_player = new sound::SoundPlayer("sound.json");
        game_config = new GameConfig("game_config.json");
        master_clock = 0;
}

void GameMaster::load_textures(const char *json_path)
{
        std::ifstream ifs(json_path, std::ios::in);
        sf::Texture *t;

        std::cout << "Loading textures listed in " << json_path << "..." << std::endl;

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
                t = new sf::Texture;
                std::cout << "\t" << val.to_str() << "...";
                if(!t->loadFromFile(val.to_str())){
                        exit(0);
                }
                std::cout << "done!!" << std::endl;
                t->setSmooth(true);
                texture_table.emplace(str_to_txid(key.c_str()), t);
        }

        std::cout << "All textures have been loaded." << std::endl;
}

void GameMaster::init_display(void)
{
        sf::Texture texture;
        if(!texture.loadFromFile("loading.png")){
                std::cerr << "Error: Failed to find loading.png" <<  std::endl;
        }
        sf::Sprite sprite;
        sprite.setTexture(texture);
        window.draw(sprite);
        window.display();
}

void GameMaster::init()
{
        window.setVerticalSyncEnabled(true);
        current_state = START;

        init_display();
        
        load_textures("textures.json");

	current_scene = new TitleSceneMaster(this->game_data);
        
}

SceneMaster *GameMaster::create_new_scene(GameState req)
{
	switch (req) {
	case START:
		return new TitleSceneMaster(this->game_data);
	case RACE:
        {
                StartToRace *p = dynamic_cast<StartToRace *>(GameMaster::get_posted_data());
                if(!p){
                        std::cerr << "No Data for Game Level" << std::endl;
                        exit(-1);
                }

                return new RaceSceneMaster(this->game_data, p);
        }
        case OPENING_EPISODE:
		return new OpeningEpisodeSceneMaster(this->game_data);
        case SAVE:
        {       
                return new SaveSceneMaster(
                        this->game_data,
                        dynamic_cast<RaceSceneMaster *>(current_scene)->export_score_information());
        }
        case RANKING:
                return new RankingSceneMaster(this->game_data);
        case CREDIT:
                return new CreditSceneMaster(this->game_data);
	case END:
                return nullptr;
        default:
                return nullptr;
	}

        return nullptr;
}

void GameMaster::switch_scene(GameState res)
{
        if(unlikely(current_state != res)){
                SceneMaster *old_scene = current_scene;
                if(res != RESET_CURRENT){
                        current_state = res;
                }else{
                        res = current_state;
                }
                current_scene = create_new_scene(res);
                delete old_scene;
        }
}

void GameMaster::main_loop()
{

        if(sf::Joystick::isConnected(0)){
                puts("input: Joystick 0 is connected.");
        }

        window.setFramerateLimit(60);
        
        while(window.isOpen()){
                sf::Event event;
                while(window.pollEvent(event)){
                        if(event.type == sf::Event::Closed)
                                window.close();
                }
                
                current_scene->pre_process(window);
                
                window.clear();

                current_scene->drawing_process(window);

		window.display();

		switch_scene(current_scene->post_process(window));

                GameMaster::sound_player->flush(master_clock);
                master_clock++;
                GameMaster::fps_calc.update();
	}
}

void GameMaster::posting_some_data(GameMasterPostObject *data)
{
        GameMaster::post_box = data;
}

GameMasterPostObject *GameMaster::get_posted_data(void)
{
        return GameMaster::post_box;
}

DrawingManager::DrawingManager(void)
        : queue(DrawingManager::compare_depth)
{
        
}

void DrawingManager::add(DrawableComponent *p)
{
        this->queue.push(p);
}

void DrawingManager::draw_and_clear(sf::RenderWindow &window)
{
        while(!queue.empty()){
                queue.top()->draw(window);
                queue.pop();
        }
}
