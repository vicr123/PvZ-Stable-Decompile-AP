#ifndef APDATA_H
#define APDATA_H

#include <cstdint>
#include <optional>
#include <set>
#include <nlohmann/json_fwd.hpp>

#include "../ConstEnums.h"

namespace PVZRAPData
{
    namespace Locations
    {
        /// Get the AP location ID for clearing a level
        /// For example, LevelClear(3, 2) will get the AP location ID for Level 2-2
        /// Dr. Zomboss is 5-10
        inline int64_t LevelClear(const int location, const int level)
        {
            if (location < 1)
            {
                return -1;
            }
            else if (location > 5)
            {
                return -1;
            }
            else if (level < 1)
            {
                return -1;
            }
            else if (level > 10)
            {
                return -1;
            }
            
            return 1000 + (location - 1) * 10 + (level - 1);
        }

        inline int64_t LevelClear(const int level)
        {
            if (level < 1 || level > 50)
            {
                return -1;
            }
            return 1000 + level - 1;
        }

        inline int64_t Twiddydinkie(const int index)
        {
            if (index < 0 || index > 95)
            {
                return -1;
            }
            return 5000 + index;
        }
        
        inline int64_t GamemodeClear(GameMode game_mode)
        {
            if (game_mode >= GameMode::GAMEMODE_CHALLENGE_WAR_AND_PEAS && game_mode <= GameMode::GAMEMODE_CHALLENGE_FINAL_BOSS)
            {
                return 1050 + game_mode - GAMEMODE_CHALLENGE_WAR_AND_PEAS;
            }
            if (game_mode >= GameMode::GAMEMODE_SCARY_POTTER_1 && game_mode <= GAMEMODE_SCARY_POTTER_9)
            {
                return 1070 + game_mode - GAMEMODE_SCARY_POTTER_1;
            }
            if (game_mode >= GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_1 && game_mode <= GAMEMODE_PUZZLE_I_ZOMBIE_9)
            {
                return 1079 + game_mode - GAMEMODE_PUZZLE_I_ZOMBIE_1;
            }
            if (game_mode >= GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1 && game_mode <= GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_5)
            {
                return 1088 + game_mode - GAMEMODE_SURVIVAL_NORMAL_STAGE_1;
            }
            if (game_mode >= GameMode::GAMEMODE_SURVIVAL_HARD_STAGE_1 && game_mode <= GameMode::GAMEMODE_SURVIVAL_HARD_STAGE_5)
            {
                return 1093 + game_mode - GAMEMODE_SURVIVAL_HARD_STAGE_1;
            }
            return -1;
        }
        
        inline int64_t Wave(int64_t apLevelId, int64_t wave)
        {
            return apLevelId * 10000 + wave;
        }
    }
    
    namespace Items
    {
        inline int64_t Seed(SeedType seed)
        {
            switch (seed)
            {
            case SEED_EXPLODE_O_NUT: return 82;
            case SEED_GIANT_WALLNUT: return 83;
            case SEED_LEFTPEATER: return 84;
            case SEED_ZOMBIE_NORMAL: return 85;
            case SEED_ZOMBIE_TRAFFIC_CONE: return 86;
            case SEED_ZOMBIE_PAIL: return 87;
            case SEED_ZOMBIE_FOOTBALL: return 88;
            case SEED_ZOMBIE_SCREEN_DOOR: return 89;
            case SEED_ZOMBIE_DIGGER: return 90;
            case SEED_ZOMBIE_LADDER: return 91;
            case SEED_ZOMBIE_BUNGEE: return 92;
            case SEED_ZOMBIE_BALLOON: return 93;
            case SEED_ZOMBIE_POLEVAULTER: return 94;
            case SEED_ZOMBIE_IMP: return 95;
            case SEED_ZOMBIE_GARGANTUAR: return 96;
            case SEED_ZOMBIE_DANCER: return 97;
            default:
                break;
            }
            if (seed < SEED_PEASHOOTER || seed > SEED_IMITATER)
            {
                return -1;
            }
            return 100 + seed;
        }
        
        constexpr int64_t MUSIC_VIDEO = 1;
        constexpr int64_t CAR_KEYS = 2;
        constexpr int64_t EXTRA_SEED_SLOT = 3;
        constexpr int64_t SHOVEL = 4;
        constexpr int64_t ALMANAC = 5;
        constexpr int64_t ZEN_GARDEN = 6;
        constexpr int64_t MINIGAMES = 7;
        constexpr int64_t PUZZLE_MODE = 8;
        constexpr int64_t SURVIVAL_MODE = 9;
        constexpr int64_t CLOUDY_DAY = 10;
        constexpr int64_t BONUS_LEVELS = 11;
        constexpr int64_t ROOF_CLEANERS = 12;
        constexpr int64_t POOL_CLEANERS = 13;
        constexpr int64_t LAWN_MOWERS = 14;
        constexpr int64_t TWIDDYDINKIES_RESTOCK = 15;
        constexpr int64_t WALL_NUT_FIRST_AID = 16;
        constexpr int64_t RAKE = 17;
        constexpr int64_t ADDITIONAL_STARTING_SUN = 18;
        constexpr int64_t MOWER_REWARD_UPGRADE = 19;
        constexpr int64_t DAY_ACCESS = 20;
        constexpr int64_t NIGHT_ACCESS = 21;
        constexpr int64_t POOL_ACCESS = 22;
        constexpr int64_t FOG_ACCESS = 23;
        constexpr int64_t ROOF_ACCESS = 24;
        constexpr int64_t CHINA_ACCESS = 25;
        constexpr int64_t PROGRESSIVE_SHOVEL = 26;
        constexpr int64_t TACO = 27;
        constexpr int64_t PROGRESSIVE_TWIDDYDINKIES = 28;
        constexpr int64_t PROGRESSIVE_LOOT_RATE = 29;
        constexpr int64_t GARDENING_GLOVE = 30;
        constexpr int64_t GOLD_WATERING_CAN = 31;
        constexpr int64_t PHONOGRAPH = 32;
        constexpr int64_t STINKY = 33;
        constexpr int64_t WHEELBARROW = 34;
        constexpr int64_t PROGRESSIVE_SUN_CAPACITY = 35;
        
        constexpr int64_t MUSTACHE_MODE = 50;
        constexpr int64_t FUTURE_ZOMBIES_MODE = 51;
        constexpr int64_t TRICKED_OUT_MODE = 52;
        constexpr int64_t DAISIES_MODE = 53;
        constexpr int64_t PINATA_MODE = 54;
        constexpr int64_t ALTERNATE_BRAINS_SOUND = 55;
        constexpr int64_t DANCING_ZOMBIES_MODE = 56;
        
        constexpr int64_t SILVER_COIN = 60;
        constexpr int64_t GOLD_COIN = 61;
        constexpr int64_t DIAMOND = 62;
        constexpr int64_t BACON = 63;
        constexpr int64_t RANDOM_SEED_PACKET = 64;
        constexpr int64_t TREE_FOOD = 65;
        constexpr int64_t FERTILIZER = 66;
        constexpr int64_t BUG_SPRAY = 67;
        constexpr int64_t CHOCOLATE = 68;
        constexpr int64_t MASS_ZOMBIE_FREEZE = 69;
        
        constexpr int64_t TRAP_MOWER_DEPLOY = 70;
        constexpr int64_t TRAP_PACKET_COOLDOWN = 71;
        constexpr int64_t TRAP_ZOMBIE_AMBUSH = 72;
        constexpr int64_t TRAP_ZOMBIE_SHUFFLE = 73;
        constexpr int64_t ZEN_GARDEN_SPROUT = 74;
        constexpr int64_t TRAP_RV = 75;
        constexpr int64_t TRAP_LAWN_FLIP = 76;
        constexpr int64_t TRAP_LAWN_RANDOMISER = 77;
        constexpr int64_t TRAP_ZOMBIE_CAFFEINE = 78;
        constexpr int64_t TRAP_ZOMBIE_HYPNOSIS = 79;
        constexpr int64_t TRAP_CRATER = 80;
        constexpr int64_t SUN_BURST = 81;
        constexpr int64_t TRAP_INVISIGHOUL = 98;
        constexpr int64_t TRAP_SEED_BANK_RANDOMISER = 99;
        
        constexpr int64_t TRAP_PARANOIA = 2001;
        constexpr int64_t TRAP_LADDER = 2002;
        
        inline SeedType SeedItem(int64_t item)
        {
            switch (item)
            {
            case 82: return SEED_EXPLODE_O_NUT;
            case 83: return SEED_GIANT_WALLNUT;
            case 84: return SEED_LEFTPEATER;
            case 85: return SEED_ZOMBIE_NORMAL;
            case 86: return SEED_ZOMBIE_TRAFFIC_CONE;
            case 87: return SEED_ZOMBIE_PAIL;
            case 88: return SEED_ZOMBIE_FOOTBALL;
            case 89: return SEED_ZOMBIE_SCREEN_DOOR;
            case 90: return SEED_ZOMBIE_DIGGER;
            case 91: return SEED_ZOMBIE_LADDER;
            case 92: return SEED_ZOMBIE_BUNGEE;
            case 93: return SEED_ZOMBIE_BALLOON;
            case 94: return SEED_ZOMBIE_POLEVAULTER;
            case 95: return SEED_ZOMBIE_IMP;
            case 96: return SEED_ZOMBIE_GARGANTUAR;
            case 97: return SEED_ZOMBIE_DANCER;
            }
            
            if (item < 100 || item > 100 + SEED_IMITATER)
            {
                return SEED_NONE;
            }
            return static_cast<SeedType>(item - 100);
        }
        
        inline int Level(const int level)
        {
            if (level < 1 || level > 50)
            {
                return -1;
            }
            return 200 + level;
        }
        
        inline int Gamemode(const GameMode game_mode)
        {
            if (game_mode >= GameMode::GAMEMODE_CHALLENGE_WAR_AND_PEAS && game_mode <= GameMode::GAMEMODE_CHALLENGE_FINAL_BOSS)
            {
                return 251 + game_mode - GAMEMODE_CHALLENGE_WAR_AND_PEAS;
            }
            if (game_mode >= GameMode::GAMEMODE_SCARY_POTTER_1 && game_mode <= GAMEMODE_SCARY_POTTER_9)
            {
                return 271 + game_mode - GAMEMODE_SCARY_POTTER_1;
            }
            if (game_mode >= GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_1 && game_mode <= GAMEMODE_PUZZLE_I_ZOMBIE_9)
            {
                return 280 + game_mode - GAMEMODE_PUZZLE_I_ZOMBIE_1;
            }
            if (game_mode >= GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1 && game_mode <= GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_5)
            {
                return 289 + game_mode - GAMEMODE_SURVIVAL_NORMAL_STAGE_1;
            }
            if (game_mode >= GameMode::GAMEMODE_SURVIVAL_HARD_STAGE_1 && game_mode <= GameMode::GAMEMODE_SURVIVAL_HARD_STAGE_5)
            {
                return 294 + game_mode - GAMEMODE_SURVIVAL_HARD_STAGE_1;
            }
            return -1;
        }
        
        inline int TileUnlock(int row, int col)
        {
            if (row > 5 || row < 0)
            {
                return -1;
            }
            if (col < 0 || col > 9)
            {
                return -1;
            }
            return 1000 + row * 10 + col;
        }
    }
    
    class SlotData
    {
    public:
        class SlotDataInner;
        SlotData(const SlotData& other);
        
        enum class LevelRandomisation
        {
            Off = 0,
            Vanilla = 1,
            RandomisedOrder = 2,
            Open = 3,
            LevelItems = 4
        };
        
        enum class AdventureModeProgression
        {
            Linear = 0,
            AreaUnlockItems = 1,
            OpenAreaUnlockItems = 2,
            LevelItems = 3
        };
        
        struct LawnlinkChance
        {
            int add_plant;
            int overwrite_plant;
            int remove_plant;
        };
        
        struct SeedStats
        {
            std::optional<int> sun_price;
            std::optional<int> recharge_time;
            std::optional<int> firing_rate;
            std::optional<int> health;
        };
        
        struct ProjectileStats
        {
            std::optional<int> damage;
        };

        struct GoalProgress
        {
            int adventure_levels_goal;
            int adventure_areas_goal;
            int minigame_levels_goal;
            int puzzle_levels_goal;
            int survival_levels_goal;
            int overall_levels_goal;
            int taco_goal;
	
            int adventure_levels_complete;
            int adventure_areas_complete;
            int minigame_levels_complete;
            int puzzle_levels_complete;
            int survival_levels_complete;
            int overall_levels_complete;
            int taco_received;
        };
        
        static SlotData get_slot_data(const nlohmann::json& json);
        
        bool is_valid() const;
        std::string version() const;
        bool requires_replanted() const;
        
        AdventureModeProgression adventure_mode_progression() const;
        
        LevelRandomisation minigame_levels() const;
        LevelRandomisation puzzle_levels() const;
        LevelRandomisation survival_levels() const;
        
        std::map<int, int> minigame_unlocks() const;
        std::map<int, int> survival_unlocks() const;
        std::map<int, int> izombie_unlocks() const;
        std::map<int, int> vasebreaker_unlocks() const;
        
        GoalProgress goal_requirements() const;
        bool fast_goal() const;
        
        std::optional<std::set<ZombieType>> zombies_on_level(int level) const;
        std::optional<std::map<SeedType, int>> conveyor_seeds_for_level(int level) const;
        std::optional<std::vector<SeedType>> conveyor_order_for_level(int level) const;
        std::optional<std::map<ZombieType, int>> zombie_weights_for_level(int level) const;
    
        std::optional<std::map<SeedType, int>> vasebreaker_seeds_for_level(int level, int wave) const;
        std::optional<std::map<ZombieType, int>> vasebreaker_zombies_for_level(int level, int wave) const;
        
        bool lock_conveyor() const;
        bool lock_vasebreaker() const;
        bool lock_izombie() const;
        
        bool easy_upgrade_plants() const;
        bool disable_storm_flashes() const;
        bool imitater_open() const;
        
        int sun_per_upgrade() const;
        bool progressive_sun_capacity_items() const;
        bool individual_tile_unlock_items() const;
        bool is_eligible_for_individual_tile_unlock_items(int level) const;
        
        bool energylink_enabled() const;
        bool ringlink_enabled() const;
        bool lawnlink_enabled() const;
        bool seedlink_enabled() const;
        
        std::optional<LawnlinkChance> lawnlink_chances() const;
        
        std::optional<SeedStats> seed_stats(SeedType seed) const;
        std::optional<ProjectileStats> projectile_stats(ProjectileType projectile) const;
        
    private:
        explicit SlotData(const std::shared_ptr<SlotDataInner>& inner);
        std::shared_ptr<SlotDataInner> inner;
    };
}

#endif
