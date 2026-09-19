#pragma once

#include <nlohmann/json.hpp>
#include "../APData.h"
#include "SlotDataInner.h"
#include "../../Lawn/Plant.h"

class SlotData1_3 : public PVZRAPData::SlotData::SlotDataInner
{
public:
    explicit SlotData1_3(nlohmann::json slot_data) : slot_data(std::move(slot_data)) {}
    nlohmann::json slot_data;
    
    bool is_valid() override
    {
        return true;
    }
    
    std::string version() override
    {
        return "1.3";
    }
    
    bool requires_replanted() override
    {
        // Not supported in 1.3
        return false;
    }
    
    PVZRAPData::SlotData::AdventureModeProgression adventure_mode_progression() override
    {
        return parse_adventure_mode_progression(slot_data["adventure_mode_progression"].get<int>());
    }

    PVZRAPData::SlotData::LevelRandomisation minigame_levels() override
    {
        return parse_level_randomisation(slot_data["minigame_levels"].get<int>());
    }
    
    PVZRAPData::SlotData::LevelRandomisation puzzle_levels() override
    {
        return parse_level_randomisation(slot_data["puzzle_levels"].get<int>());
    }
    
    PVZRAPData::SlotData::LevelRandomisation survival_levels() override
    {
        return parse_level_randomisation(slot_data["survival_levels"].get<int>());
    }

    std::map<int, int> minigame_unlocks() override
    {
        return parse_unlocks(slot_data["minigame_unlocks"]);
    }
    
    std::map<int, int> survival_unlocks() override
    {
        return parse_unlocks(slot_data["survival_unlocks"]);
    }
    
    std::map<int, int> izombie_unlocks() override
    {
        return parse_unlocks(slot_data["izombie_unlocks"]);
    }
    
    std::map<int, int> vasebreaker_unlocks() override
    {
       return parse_unlocks(slot_data["vasebreaker_unlocks"]);
    }
    
    PVZRAPData::SlotData::GoalProgress goal_requirements() override
    {
        PVZRAPData::SlotData::GoalProgress gp;
	
        gp.adventure_levels_goal = slot_data["adventure_levels_goal"];
        gp.adventure_areas_goal = slot_data["adventure_areas_goal"];
        gp.minigame_levels_goal = slot_data["minigame_levels_goal"];
        gp.puzzle_levels_goal = slot_data["puzzle_levels_goal"];
        gp.survival_levels_goal = slot_data["survival_levels_goal"];
        gp.overall_levels_goal = slot_data["overall_levels_goal"];
        gp.taco_goal = 0;
        
        return gp;
    }
    
    bool fast_goal() override
    {
        return slot_data["fast_goal"].get<int>() == 1;
    }
    
    std::optional<std::set<ZombieType>> zombies_on_level(int level) override
    {
        auto zombie_map = slot_data["zombie_map"];
        auto available_zombies_for_level = zombie_map[std::to_string(level)];
        if (available_zombies_for_level.is_null())
        {
            return {};
        }
        
        std::set<ZombieType> result;
        for (auto zombie : available_zombies_for_level)
        {
            if (zombie < ZOMBIE_NORMAL || zombie > ZOMBIE_REDEYE_GARGANTUAR)
            {
                continue;
            }
            result.insert(static_cast<ZombieType>(zombie.get<int>()));
        }
        return result;
    }
    
    std::optional<std::map<SeedType, int>> conveyor_seeds_for_level(int level) override
    {
        // Not supported in 1.3
        return {};
    }
    
    std::optional<std::vector<SeedType>> conveyor_order_for_level(int level) override
    {
        // Not supported in 1.3
        return {};
    }
    
    std::optional<std::map<ZombieType, int>> zombie_weights_for_level(int level) override
    {
        // Not supported in 1.3
        return {};
    }
    
    std::optional<std::vector<SeedType>> banned_plants_for_level(int level) override
    {
        // Not supported in 1.3
        return {};
    }
    
    bool lock_conveyor() override
    {
        // Not supported in 1.3
        return {};
    }
    
    bool lock_vasebreaker() override
    {
        // Not supported in 1.3
        return {};
    }
    
    bool lock_izombie() override
    {
        // Not supported in 1.3
        return {};
    }
    
    bool easy_upgrade_plants() override
    {
        return slot_data["easy_upgrade_plants"].get<int>() > 0;
    }
    
    bool disable_storm_flashes() override
    {
        return slot_data["disable_storm_flashes"].get<int>() > 0;
    }
    
    bool imitater_open() override
    {
        return slot_data["imitater_open"].get<bool>();
    }
    
    bool deathlink_enabled() override
    {
        return slot_data["deathlink_enabled"].get<int>() == 1;
    }
    
    bool energylink_enabled() override
    {
        // Not supported in 1.3
        return true;
    }
    
    bool ringlink_enabled() override
    {
        // Not supported in 1.3
        return false;
    }
    
    bool lawnlink_enabled() override
    {
        // Not supported in 1.3
        return false;
    }
    
    bool seedlink_enabled() override
    {
        // Not supported in 1.3
        return false;
    }
    
    bool harder_zombie_spawns() override
    {
        // Not supported in 1.3
        return false;
    }
    
    std::optional<PVZRAPData::SlotData::LawnlinkChance> lawnlink_chances() override
    {
        // Not supported in 1.3
        return {};
    }
    
    bool progressive_sun_capacity_items() override
    {
        // Not supported in 1.3
        return false;
    }
    
    bool individual_tile_unlock_items() override
    {
        // Not supported in 1.3
        return false;
    }
    
    bool is_eligible_for_individual_tile_unlock_items(int level) override
    {
        // Not supported in 1.3
        return false;
    }
    
    bool is_eligible_for_harder_zombie_spawns(int level) override
    {
        // Not supported in 1.3
        return false;
    }
    
    std::optional<PVZRAPData::SlotData::SeedStats> seed_stats(SeedType seed) override
    {
        // Apply easy upgrade tax
        if (easy_upgrade_plants())
        {
            int easy_upgrade_tax;
            switch (seed)
            {
            case SeedType::SEED_GATLINGPEA:
                easy_upgrade_tax = 200;
                break;
            case SeedType::SEED_TWINSUNFLOWER:
                easy_upgrade_tax = 50;
                break;
            case SeedType::SEED_GLOOMSHROOM:
                easy_upgrade_tax = 75;
                break;
            case SeedType::SEED_CATTAIL:
                easy_upgrade_tax = 25;
                break;
            case SeedType::SEED_WINTERMELON:
                easy_upgrade_tax = 300;
                break;
            case SeedType::SEED_GOLD_MAGNET:
                easy_upgrade_tax = 100;
                break;
            case SeedType::SEED_SPIKEROCK:
                easy_upgrade_tax = 100;
                break;
            case SeedType::SEED_COBCANNON:
                easy_upgrade_tax = 200;
                break;
            default:
                // No easy upgrade tax applies
                return {};
            }

            const auto plant_def = GetPlantDefinition(seed);
            
            PVZRAPData::SlotData::SeedStats stats;
            stats.sun_price = plant_def.mSeedCost + easy_upgrade_tax;
            return stats;
        }
        return {};
    }
    
    std::optional<PVZRAPData::SlotData::ProjectileStats> projectile_stats(const ProjectileType projectile) override
    {
        // Not supported in 1.3
        return {};
    }
    
    int sun_per_upgrade() override
    {
        // Not supported in 1.3
        return {};
    }
    
    std::optional<std::map<SeedType, int>> vasebreaker_seeds_for_level(int level, int wave) override
    {
        // Not supported in 1.3
        return {};
    }
    
    std::optional<std::map<ZombieType, int>> vasebreaker_zombies_for_level(int level, int wave) override
    {
        // Not supported in 1.3
        return {};
    }

private:
    static PVZRAPData::SlotData::AdventureModeProgression parse_adventure_mode_progression(int adventure_mode_progression)
    {
        switch (adventure_mode_progression)
        {
        case 0:
            return PVZRAPData::SlotData::AdventureModeProgression::Linear;
        case 1:
            return PVZRAPData::SlotData::AdventureModeProgression::AreaUnlockItems;
        case 2:
            return PVZRAPData::SlotData::AdventureModeProgression::OpenAreaUnlockItems;
        case 3:
            return PVZRAPData::SlotData::AdventureModeProgression::LevelItems;
        default:
            // ???
            return PVZRAPData::SlotData::AdventureModeProgression::Linear;
        }
    }
    
    static PVZRAPData::SlotData::LevelRandomisation parse_level_randomisation(int level_randomisation)
    {
        switch (level_randomisation)
        {
        case 0:
            return PVZRAPData::SlotData::LevelRandomisation::Off;
        case 1:
            return PVZRAPData::SlotData::LevelRandomisation::Vanilla;
        case 2:
            return PVZRAPData::SlotData::LevelRandomisation::RandomisedOrder;
        case 3:
            return PVZRAPData::SlotData::LevelRandomisation::Open;
        case 4:
            return PVZRAPData::SlotData::LevelRandomisation::LevelItems;
        default:
            // ???
            return PVZRAPData::SlotData::LevelRandomisation::Off;
        }
    }
    
    static std::map<int, int> parse_unlocks(nlohmann::json unlocks)
    {
        std::map<int, int> result;
        for (auto unlock = unlocks.begin(); unlock != unlocks.end(); ++unlock)
        {
            result[std::stoi(unlock.key())] = unlock.value().get<int>();
        }
        
        return result;
    }
};
