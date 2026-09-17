#pragma once

#include "SlotData1_9.h"

class SlotData1_10 : public SlotData1_9
{
public:
    explicit SlotData1_10(nlohmann::json slot_data) : SlotData1_9(std::move(slot_data)) {}
    
    std::string version() override
    {
        return "1.10";
    }
    
    bool harder_zombie_spawns() override
    {
        return slot_data["harder_zombie_spawns"].get<int>();
    }
    
    bool is_eligible_for_harder_zombie_spawns(int level) override
    {
        const int ineligible_levels[] = {45, 54, 61, 62, 63, 66, 69, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 104};
        if (std::ranges::find(ineligible_levels, level) != std::ranges::end(ineligible_levels))
        {
            return false;
        }
        return true;
    }
    
    std::optional<std::vector<SeedType>> banned_plants_for_level(int level) override
    {
        auto root_map = slot_data["plant_banlist"];
        if (root_map.is_null())
        {
            return {};
        }
        
        auto level_data = root_map[std::to_string(level)];
        if (level_data.is_null())
        {
            return {};
        }

        return level_data.get<std::vector<SeedType>>();
    }
    
    std::optional<std::map<SeedType, int>> vasebreaker_seeds_for_level(int level, int wave) override
    {
        return extract_vasebreaker_data<SeedType>("vasebreaker_plants_map",level,wave);
    }
    
    std::optional<std::map<ZombieType, int>> vasebreaker_zombies_for_level(int level, int wave) override
    {
        return extract_vasebreaker_data<ZombieType>("vasebreaker_zombies_map",level,wave);
    }
    
private:
    template<typename T> std::optional<std::map<T, int>> extract_vasebreaker_data(const char* map_entry,int level, int wave)
    {
        auto root_map = slot_data[map_entry];
        if (root_map.is_null())
        {
            return {};
        
        }
        auto level_data = root_map[std::to_string(level)];
        if (level_data.is_null())
        {
            return {};
        }
        
        auto wave_data = level_data[wave];
        if (wave_data.is_null())
        {
            return {};
        }

        std::map<T, int> result;
        for (const auto& [key, amount] : wave_data.items())
        {
            auto seed = static_cast<T>(std::stoi(key));
            if constexpr (std::is_same_v<T, SeedType>)
            {
                // Special case for leftpeater
                if (key == "107")
                {
                    seed = SeedType::SEED_LEFTPEATER;
                }
            }
            result.insert_or_assign(seed, amount.get<int>());
        }
        return result;
    } 
};