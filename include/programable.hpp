#pragma once

#include "macro.hpp"
#include <vector>
#include <unordered_map>

class FunctionCallEssential {
public:
        std::string func_name;
        u64 time;

        sf::Vector2f origin;

	FunctionCallEssential(std::string fn, u64 t, sf::Vector2f origin_p = sf::Vector2f(0, 0));
};

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

        std::pair<std::string, std::vector<BulletData *> *> parse(std::string sub_file);
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
