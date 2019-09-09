#include "programable.hpp"
#include <fstream>
#include <regex>


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
                parse(src_name);
        }

        /*
        for(auto &array_element : sched_array){
                picojson::object &data = array_element.get<picojson::object>();
                auto &&parsed = highlevel_controll_statement(data);
                //util::concat_container<std::vector<FunctionCallEssential>>(func_sched, parsed);
        }
        */
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
                        count,
                        SHOT_MASTER_UNDEFINED
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
                                SHOT_MASTER_UNDEFINED,
                                obj["time"].get<double>()
                                )
                                };
        }
}

std::pair<std::string, std::vector<BulletData *> *> BulletFuncTable::parse_function(picojson::object &obj)
{
        auto bullets_sched = new std::vector<BulletData *>;
        
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

void BulletFuncTable::parse(std::string sub_file)
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

        auto &obj = v.get<picojson::object>();
        auto function_array = obj["function_definition"].get<picojson::array>();

        for(auto &array_element : function_array){
                picojson::object &array_object = array_element.get<picojson::object>();
                table.emplace(parse_function(array_object));
        }
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

std::vector<BulletData *> BulletFuncTable::call_function(FunctionCallEssential e)
{
        std::vector<BulletData *> ret;

        try{
                auto &&original_bullets_data = table.at(e.func_name);
                for(auto &&bullet_data : *original_bullets_data){
                        BulletData *p = new BulletData(*bullet_data);
                        p->shot_master_id = e.shot_master_id;
                        p->appear_point += e.origin;
                        ret.push_back(p);
                }
        }
        catch(std::out_of_range &) {
                DEBUG_PRINT_HERE();
                std::cout << "Exception!! No such bullet schedule written in JSON: " << e.func_name << std::endl;
        }

	return ret;
}


EnemyCharacterTable::EnemyCharacterTable(std::vector<std::string> files)
{
        for(std::string &file : files){
                parse_file(file);
	}
}

void EnemyCharacterTable::parse_file(std::string json_path)
{
        std::ifstream ifs(json_path, std::ios::in);

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

        if(!check_magic(obj["magic"].get<std::string>())){
                std::cerr << "magic error" << obj["magic"].get<std::string>() << std::endl;
                DEBUG_PRINT_HERE();
        }
        
        picojson::array &obj_array = obj["enemy"].get<picojson::array>();

        for (auto &array_element : obj_array) {
                picojson::object &elem = array_element.get<picojson::object>();
                table.emplace(parse_object(elem));
        }
}

std::pair<std::string, EnemyCharacterMaterial *> EnemyCharacterTable::parse_object(picojson::object &obj)
{
        EnemyCharacterMaterial *material = new EnemyCharacterMaterial;
        material->name = obj["name"].get<std::string>();
        material->txid = str_to_txid(obj["TextureID"].get<std::string>().data());

        picojson::object &point_data = obj["point"].get<picojson::object>();
        material->point = sf::Vector2f(point_data["x"].get<double>(), point_data["y"].get<double>());
        
        picojson::object &scale_data = obj["scale"].get<picojson::object>();
        material->scale = sf::Vector2f(scale_data["x"].get<double>(), scale_data["y"].get<double>());

        picojson::object &mf_data = obj["move_func_description"].get<picojson::object>();
        material->move_func = select_bullet_function(str_to_bfid(mf_data["ID"].get<std::string>().data()), mf_data);

        picojson::object &rotf_data = obj["rot_func_description"].get<picojson::object>();
        material->rot_func = rotate::select_rotation_function(
                rotate::str_to_rotf_id(rotf_data["ID"].get<std::string>().data()), rotf_data);

        material->max_hp = obj["max_hp"].get<double>();
        material->init_hp = obj["init_hp"].get<double>();
        material->radius = obj["radius"].get<double>();
        
        picojson::array &shot_array = obj["shot"].get<picojson::array>();
        for(auto &array_element : shot_array){
                picojson::object &shot = array_element.get<picojson::object>();
                material->shot_data.emplace_back(parse_shot_field(shot));
        }

        /*
         * 出現時間は後で決定する。
         */
        material->time = 0;

        return std::make_pair(obj["name"].get<std::string>(), material);
}

bool EnemyCharacterTable::check_magic(std::string magic)
{
        return magic == "enemy_description";
}

FunctionCallEssential EnemyCharacterTable::parse_shot_field(picojson::object &obj)
{
        sf::Vector2f relative_point(0, 0);
        if(obj.find("at") != std::end(obj)){
                picojson::object &at = obj["at"].get<picojson::object>();
                relative_point.x = at["x"].get<double>();
                relative_point.y = at["y"].get<double>();
        }
        
        return FunctionCallEssential(
                obj["function_name"].get<std::string>(),
                obj["time"].get<double>(),
                SHOT_MASTER_UNDEFINED,
                relative_point
                );
}

EnemyCharacterMaterial *EnemyCharacterTable::get(std::string name)
{
        return table[name];
}
