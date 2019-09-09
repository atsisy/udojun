#pragma once

#include "macro.hpp"
#include <vector>
#include <unordered_map>
#include "rotate_func.hpp"
#include "enemy_character.hpp"

class GeneralJSONFunctionArgs {
private:
        sf::Vector2f position;
        u64 time;

public:
        GeneralJSONFunctionArgs(void);

        void move_position(sf::Vector2f distance);
        void postpone(u64 time);
        void advance(u64 time);

        void set_position(sf::Vector2f pos);
	void set_position(float x, float y);
        void set_time(u64 time);
        
        sf::Vector2f get_position(void);
        u64 get_time(void);
        

        static GeneralJSONFunctionArgs
        constract_this_object(picojson::object &data);
};

class BulletFuncTable {
private:
        std::unordered_map<std::string, std::vector<BulletData *> *> table;

        std::pair<std::string, std::vector<BulletData *> *> parse_function(picojson::object &obj);
        void parse(std::string sub_file);
        
        std::vector<FunctionCallEssential> highlevel_controll_statement(picojson::object &obj);
        std::vector<FunctionCallEssential> parse_builtin_for(picojson::object &obj);

	void parse_multiple_function_call(picojson::object &data,
					  std::vector<BulletData *> *buf);
	void
	parse_general_multiple_function_call(std::string func_name,
                                             GeneralJSONFunctionArgs args,
					     std::vector<BulletData *> *buf);
	void
	parse_builtin_function_multiple_calling(std::string func_name,
						picojson::object &data,
						std::vector<BulletData *> *buf);

    public:
        BulletFuncTable(std::string main_file);

	std::vector<BulletData *> call_function(FunctionCallEssential e);
};

class EnemyCharacterTable {
private:
        std::unordered_map<std::string, EnemyCharacterMaterial *> table;

        bool check_magic(std::string magic);
        void parse_file(std::string json_path);
        std::pair<std::string, EnemyCharacterMaterial *> parse_object(picojson::object &obj);
        FunctionCallEssential parse_shot_field(picojson::object &obj);


public:
        EnemyCharacterTable(std::vector<std::string> files);
        
        EnemyCharacterMaterial *get(std::string name);
};
