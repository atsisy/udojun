#include "programable.hpp"
#include <fstream>
#include <regex>

FunctionCallEssential::FunctionCallEssential(std::string fn, u64 t,
					     sf::Vector2f origin_p)
	: func_name(fn), time(t), origin(origin_p)
{}

GeneralJSONFunctionArgs::GeneralJSONFunctionArgs(void)
        : position(0, 0), time(0)
{}

void GeneralJSONFunctionArgs::move_position(sf::Vector2f distance)
{
        this->position += distance;
}

void GeneralJSONFunctionArgs::postpone(u64 time)
{
        this->time += time;
}

void GeneralJSONFunctionArgs::advance(u64 time)
{
        this->time -= time;
}

void GeneralJSONFunctionArgs::set_position(sf::Vector2f pos)
{
        this->position = pos;
}

void GeneralJSONFunctionArgs::set_position(float x, float y)
{
        this->position = sf::Vector2f(x, y);
}

void GeneralJSONFunctionArgs::set_time(u64 time)
{
        this->time = time;
}

sf::Vector2f GeneralJSONFunctionArgs::get_position(void)
{
        return this->position;
}

u64 GeneralJSONFunctionArgs::get_time(void)
{
        return this->time;
}

GeneralJSONFunctionArgs
GeneralJSONFunctionArgs::constract_this_object(
	picojson::object &data)
{
        GeneralJSONFunctionArgs args;

	if (data.find("time") != std::end(data)) {
		args.set_time(data["time"].get<double>());
	}
	if (data.find("position") != std::end(data)) {
		auto &&pos_data = data["position"].get<picojson::object>();
		args.set_position(pos_data["x"].get<double>(),
                                  pos_data["y"].get<double>());
	}

        return args;
}

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
                auto &&parsed = highlevel_controll_statement(data);
                util::concat_container<std::vector<FunctionCallEssential>>(func_sched, parsed);
        }

	std::sort(std::begin(func_sched), std::end(func_sched),
                  [](const FunctionCallEssential &fce1, const FunctionCallEssential &fce2){
                          return fce1.time < fce2.time;
                  });
}

std::vector<FunctionCallEssential>
BulletFuncTable::parse_builtin_for(picojson::object &obj)
{
        std::vector<FunctionCallEssential> ret;

        u64 dist_count = obj["distance"].get<double>();
        u64 count = obj["time"].get<double>();
        u64 times = obj["times"].get<double>();
        std::string call = obj["call"].get<std::string>();

        while(times--){
                ret.emplace_back(
                        call,
                        count
                        );
                count += dist_count;
        }

        return ret;
}

std::vector<FunctionCallEssential>
BulletFuncTable::highlevel_controll_statement(picojson::object &obj)
{
        std::string func_name = obj["function"].get<std::string>();

        if(func_name == "builtin-for"){
                return parse_builtin_for(obj);
        }else{
                return std::vector<FunctionCallEssential>{
                        FunctionCallEssential(
                                func_name,
                                obj["time"].get<double>()
                                )
                                };
        }
}

std::pair<std::string, std::vector<BulletData *> *> BulletFuncTable::parse(std::string sub_file)
{
        std::ifstream ifs(sub_file, std::ios::in);

        if (ifs.fail()) {
                std::cerr << "failed to read json file" << std::endl;
                exit(1);
        }

        std::cout << "Loading " << sub_file << "..." << std::endl;

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

        std::cout << "\tParse: schedule, function" << std::endl;

	std::string func_name = obj["function"].get<std::string>();
	picojson::array &array = obj["schedule"].get<picojson::array>();

	std::cout << "\tfunction: "  << func_name << " ... ";
        std::fflush(stdout);
        
        for(auto &array_element : array){
                picojson::object &data = array_element.get<picojson::object>();
                if(data["type"].get<std::string>() == "macro"){
                        auto &&gen = macro::expand_macro(data);
                        std::copy(std::begin(gen), std::end(gen), std::back_inserter(*bullets_sched));
                }else if(data["type"].get<std::string>() == "dynamic-macro"){
                        bullets_sched->push_back(new BulletData(data, DYNAMIC_MACRO));
                }else if(data["type"].get<std::string>() == "dynamic-unit"){
                        bullets_sched->push_back(new BulletData(data, AIMING_SELF));
                }else if(data["type"].get<std::string>() == "laser"){
                        bullets_sched->push_back(new BulletData(data, LASER_BULLET));
		}else if(data["type"].get<std::string>() == "function-call") {
			parse_multiple_function_call(
				data,
				bullets_sched);
		} else {
			bullets_sched->push_back(new BulletData(data));
		}
	}

        std::sort(std::begin(*bullets_sched), std::end(*bullets_sched),
                  [](const BulletData *b1, const BulletData *b2){
                          return b1->appear_time < b2->appear_time;
                  });

        std::cout << "OK!!" << std::endl;
        
        return std::make_pair(func_name, bullets_sched);
}

void BulletFuncTable::parse_multiple_function_call(picojson::object &data,
				  std::vector<BulletData *> *buf)
{
	std::regex regex("^builtin.*");
        std::string called_func = data["function"].get<std::string>();

	if(std::regex_match(called_func, regex)){
		parse_builtin_function_multiple_calling(called_func, data, buf);
	}else{
		parse_general_multiple_function_call(called_func,
                                                     GeneralJSONFunctionArgs::constract_this_object(data),
                                                     buf);
	}
}

void BulletFuncTable::parse_general_multiple_function_call(
	std::string func_name, GeneralJSONFunctionArgs args,
	std::vector<BulletData *> *buf)
{
	std::vector<BulletData *> *fn_body = table[func_name];
	BulletData *d;

	for (BulletData *b_data : *fn_body) {
		d = new BulletData(*b_data);
		d->offset += args.get_time();
		d->appear_point += args.get_position();
		buf->push_back(d);
	}
}

void BulletFuncTable::parse_builtin_function_multiple_calling(std::string func_name,
					     picojson::object &data,
					     std::vector<BulletData *> *buf)
{
        auto &&args = GeneralJSONFunctionArgs::constract_this_object(data);
	u64 dist_count = data["distance"].get<double>();
	u64 times = data["times"].get<double>();
	std::string call = data["call"].get<std::string>();
	sf::Vector2f pos_offset(0, 0);

	if(data.find("move") != std::end(data)){
		auto &&move_data = data["move"].get<picojson::object>();
		pos_offset.x = move_data["x"].get<double>();
		pos_offset.y = move_data["y"].get<double>();
	}

	while (times--) {
                parse_general_multiple_function_call(call, args, buf);
                args.postpone(dist_count);
                args.move_position(pos_offset);
	}
}

std::vector<FunctionCallEssential> BulletFuncTable::get_func_sched()
{
        return func_sched;
}

std::vector<BulletData *> BulletFuncTable::call_function(FunctionCallEssential e)
{
        std::vector<BulletData *> ret;
        auto &&original_bullets_data = table[e.func_name];

	for(auto &&bullet_data : *original_bullets_data){
		BulletData *p = new BulletData(*bullet_data);
		p->appear_point += e.origin;
                ret.push_back(p);
	}

	return ret;
}

void BulletFuncTable::add_function_dynamic(std::string fn, u64 t)
{
        func_sched.emplace_back(fn, t);
}

void BulletFuncTable::add_function_dynamic(FunctionCallEssential e)
{
	func_sched.push_back(e);
}

void BulletFuncTable::clear_func_sched(void)
{
        std::cout << "func sched size: " << func_sched.size() << std::endl;
        func_sched.clear();
}
