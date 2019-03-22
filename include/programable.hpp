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

class BulletFuncTable {
private:
        std::unordered_map<std::string, std::vector<BulletData *> *> table;
        std::vector<FunctionCallEssential> func_sched;

        std::pair<std::string, std::vector<BulletData *> *> parse(std::string sub_file);
        std::vector<FunctionCallEssential> highlevel_controll_statement(picojson::object &obj);
        std::vector<FunctionCallEssential> parse_builtin_for(picojson::object &obj);

	void parse_multiple_function_call(std::string func_name,
					  std::vector<BulletData *> *buf, u64 time);

    public:
        BulletFuncTable(std::string main_file);

        std::vector<BulletData *> call_function(FunctionCallEssential e);
        std::vector<FunctionCallEssential> get_func_sched();

	void add_function_dynamic(FunctionCallEssential e);
	void add_function_dynamic(std::string fn, u64 t);
};
