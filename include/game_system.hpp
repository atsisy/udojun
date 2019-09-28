#pragma once

#include "game_component.hpp"

namespace game_system {
        std::vector<SaveData> load_save_data(std::string json_file);
        void store_save_data(std::string json_file, SaveData save_data);
}
