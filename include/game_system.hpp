#pragma once

#include <string>
#include <unordered_map>
#include "picojson.h"
#include "utility.hpp"
#include "game_component.hpp"

enum SpellCardType {
        GEKIHA = 0,
        TAIKYU,
        UNKNOWN_SCTYPE,
};

enum SpellCardID {
        TEST_SCID = 0,
        UNKNOWN_SCID,
};

inline SpellCardID str_to_scid(const char *str)
{
        str_to_idx_sub(str, TEST_SCID);
        return UNKNOWN_SCID;
}

inline SpellCardType str_to_sctype(const char *str)
{
	str_to_idx_sub(str, GEKIHA);
	str_to_idx_sub(str, TAIKYU);
	return UNKNOWN_SCTYPE;
}

class SpellCardInfo {
private:
        std::string name;
        SpellCardType type;

public:
        SpellCardInfo(picojson::object &json_data);
};

class SpellCardTable {
private:
        std::unordered_map<SpellCardID, SpellCardInfo *> spell_card_map;
        
public:
        SpellCardTable(const char *json);
        SpellCardInfo *get(SpellCardID id);
};
