#pragma once
#include <set>

class PVZRAPData::SlotData::SlotDataInner
{
public:
    virtual ~SlotDataInner() = default;
    
    virtual bool is_valid() = 0;
    virtual std::string version() = 0;
    virtual bool requires_replanted() = 0;
    
    virtual AdventureModeProgression adventure_mode_progression() = 0;
    
    virtual LevelRandomisation minigame_levels() = 0;
    virtual LevelRandomisation puzzle_levels() = 0;
    virtual LevelRandomisation survival_levels() = 0;
    
    virtual std::map<int, int> minigame_unlocks() = 0;
    virtual std::map<int, int> survival_unlocks() = 0;
    virtual std::map<int, int> izombie_unlocks() = 0;
    virtual std::map<int, int> vasebreaker_unlocks() = 0;
    
    virtual GoalProgress goal_requirements() = 0;
    virtual bool fast_goal() = 0;
    
    virtual std::optional<std::set<ZombieType>> zombies_on_level(int level) = 0;
    virtual std::optional<std::map<SeedType, int>> conveyor_seeds_for_level(int level) = 0;
    virtual std::optional<std::vector<SeedType>> conveyor_order_for_level(int level) = 0;
    virtual std::optional<std::map<ZombieType, int>> zombie_weights_for_level(int level) = 0;
    virtual std::optional<std::vector<SeedType>> banned_plants_for_level(int level) = 0;
    
    virtual std::optional<std::map<SeedType, int>> vasebreaker_seeds_for_level(int level, int wave) = 0;
    virtual std::optional<std::map<ZombieType, int>> vasebreaker_zombies_for_level(int level, int wave) = 0;
    
    virtual bool lock_conveyor() = 0;
    virtual bool lock_vasebreaker() = 0;
    virtual bool lock_izombie() = 0;
    
    virtual bool easy_upgrade_plants() = 0;
    virtual bool disable_storm_flashes() = 0;
    virtual bool imitater_open() = 0;
    
    virtual int sun_per_upgrade() = 0;
    virtual bool progressive_sun_capacity_items() = 0;
    virtual bool individual_tile_unlock_items() = 0;
    virtual bool is_eligible_for_individual_tile_unlock_items(int level) = 0;
    virtual bool is_eligible_for_harder_zombie_spawns(int level) = 0;
    
    virtual bool deathlink_enabled() = 0;
    virtual bool energylink_enabled() = 0;
    virtual bool ringlink_enabled() = 0;
    virtual bool lawnlink_enabled() = 0;
    virtual bool seedlink_enabled() = 0;
    virtual bool harder_zombie_spawns() = 0;
    
    virtual std::optional<LawnlinkChance> lawnlink_chances() = 0;
    
    virtual std::optional<SeedStats> seed_stats(SeedType seed) = 0;
    virtual std::optional<ProjectileStats> projectile_stats(ProjectileType projectile) = 0;
};
