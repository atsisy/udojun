#pragma once

#include "macro.hpp"
#include <vector>
#include <unordered_map>

class FunctionCallEssential {
public:
        std::string func_name;
        u64 time;

        FunctionCallEssential(std::string fn, u64 t);
};

class BulletFuncTable {
private:
        std::unordered_map<std::string, std::vector<BulletData *> *> table;
        std::vector<FunctionCallEssential> func_sched;

        std::pair<std::string, std::vector<BulletData *> *> parse(std::string sub_file);
        
public:
        BulletFuncTable(std::string main_file);

        std::vector<BulletData *> *call_function(std::string key);
        std::vector<FunctionCallEssential> get_func_sched();
};
