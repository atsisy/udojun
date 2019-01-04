#include "programable.hpp"
#include <fstream>

FunctionCallEssential::FunctionCallEssential(std::string fn, u64 t)
        : func_name(fn), time(t)
{}

BulletFuncTable::BulletFuncTable(std::string main_file)
{
        std::ifstream ifs(main_file, std::ios::in);

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

        auto &obj = v.get<picojson::object>();
        picojson::array &sched_array = obj["schedule"].get<picojson::array>();
        picojson::array &src_array = obj["src"].get<picojson::array>();

        for(auto &array_element : src_array){
                std::string &src_name = array_element.get<std::string>();
                table.emplace(parse(src_name));
        }

        for(auto &array_element : sched_array){
                picojson::object &data = array_element.get<picojson::object>();
                func_sched.emplace_back(
                        data["function"].get<std::string>(),
                        data["time"].get<double>());
        }
        
        std::sort(std::begin(func_sched), std::end(func_sched),
                  [](const FunctionCallEssential &fce1, const FunctionCallEssential &fce2){
                          return fce1.time < fce2.time;
                  });
}

std::pair<std::string, std::vector<BulletData *> *> BulletFuncTable::parse(std::string sub_file)
{
        std::ifstream ifs(sub_file, std::ios::in);

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

        auto bullets_sched = new std::vector<BulletData *>;
        auto &obj = v.get<picojson::object>();
        picojson::array &array = obj["schedule"].get<picojson::array>();
        std::string func_name = obj["function"].get<std::string>();

        for(auto &array_element : array){
                picojson::object &data = array_element.get<picojson::object>();
                if(data["type"].get<std::string>() == "macro"){
                        auto &&gen = macro::expand_macro(data);
                        std::copy(std::begin(gen), std::end(gen), std::back_inserter(*bullets_sched));
                }else if(data["type"].get<std::string>() == "dynamic-macro"){
                        bullets_sched->push_back(new BulletData(data, DYNAMIC_MACRO));
                }else if(data["type"].get<std::string>() == "dynamic-unit"){
                        bullets_sched->push_back(new BulletData(data, AIMING_SELF));
                }else{
                        bullets_sched->push_back(new BulletData(data));
                }
        }
        
        std::sort(std::begin(*bullets_sched), std::end(*bullets_sched),
                  [](const BulletData *b1, const BulletData *b2){
                          return b1->appear_time < b2->appear_time;
                  });

        return std::make_pair(func_name, bullets_sched);
}

std::vector<FunctionCallEssential> BulletFuncTable::get_func_sched()
{
        return func_sched;
}

std::vector<BulletData *> *BulletFuncTable::call_function(std::string key)
{
        return table[key];
}
