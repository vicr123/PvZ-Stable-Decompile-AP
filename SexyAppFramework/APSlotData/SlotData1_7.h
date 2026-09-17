#pragma once

#include "SlotData1_6.h"

class SlotData1_7 : public SlotData1_6
{
public:
    explicit SlotData1_7(nlohmann::json slot_data) : SlotData1_6(std::move(slot_data)) {}
    
    std::string version() override
    {
        return "1.7";
    }
    
    bool ringlink_enabled() override
    {
        return slot_data["ringlink_enabled"].get<int>();
    }
    
    bool progressive_sun_capacity_items() override
    {
        return slot_data["progressive_sun_capacity_items"].get<int>();
    }
    
    bool individual_tile_unlock_items() override
    {
        return slot_data["individual_tile_unlock_items"].get<int>();
    }
    
    bool is_eligible_for_individual_tile_unlock_items(int level) override
    {
        const int ineligible_levels[] = {5, 15, 35, 52, 53, 55, 57, 58, 59, 65, 68, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 99, 103};
        if (std::ranges::find(ineligible_levels, level) != std::ranges::end(ineligible_levels))
        {
            return false;
        }
        return true;
    }
    
    std::optional<std::map<ZombieType, int>> zombie_weights_for_level(int level) override
    {
        auto weight_map = slot_data["zombie_weight_map"];
        switch (slot_data["zombie_weight_randomisation"].get<int>())
        {
        case 0:
            // Off
            return {};
        case 2:
            {
                // Per Level
                auto level_weight_map = weight_map[std::to_string(level)];
                if (level_weight_map.is_discarded())
                {
                    // No map for this level
                    return {};
                }
                weight_map = level_weight_map;
                
                [[fallthrough]];
            }
        case 1:
            {
                // Randomise Once
                std::map<ZombieType, int> result;
                for (const auto& [zombie_json, weight_json] : weight_map.items())
                {
                    auto seed = static_cast<ZombieType>(std::stoi(zombie_json));
                    result.insert_or_assign(seed, weight_json.get<int>());
                }
                return result;
            }
        }
        
        return {};
    }
};