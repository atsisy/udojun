#include "game_system.hpp"
#include <fstream>

SpellCardInfo::SpellCardInfo(picojson::object &obj)
        : name(obj["name"].get<std::string>()),
          type(str_to_sctype(obj["type"].get<std::string>().data()))
{}

SpellCardTable::SpellCardTable(const char *json)
{
	std::ifstream ifs(json, std::ios::in);
	if (ifs.fail()) {
		std::cerr << "failed to read json file: " << json << std::endl;
		exit(1);
	}
	const std::string json_raw((std::istreambuf_iterator<char>(ifs)),
			       std::istreambuf_iterator<char>());
	ifs.close();
	picojson::value v;
	const std::string err = picojson::parse(v, json_raw);
	if (err.empty() == false) {
		std::cerr << err << std::endl;
		exit(1);
	}

	auto &obj = v.get<picojson::object>();
	picojson::array &info_array = obj["spell_info"].get<picojson::array>();

	for (auto &array_element : info_array) {
		auto &info_obj = array_element.get<picojson::object>();
                spell_card_map.emplace(
                        str_to_scid(info_obj["id"].get<std::string>().data()),
                        new SpellCardInfo(info_obj));
	}
}
