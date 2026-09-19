#include "APData.h"

#include <nlohmann/json.hpp>

#include "APWrapper.h"
#include "APSlotData/SlotData1_10.h"

class SlotDataInvalid : public PVZRAPData::SlotData::SlotDataInner
{
public:
    explicit SlotDataInvalid(std::string version) : _version(version) {}
    std::string _version;
    
    bool is_valid() override
    {
        return false;
    }
    
    std::string version() override
    {
        return _version;
    }
    
    bool requires_replanted() override
    {
        return {};
    }

    PVZRAPData::SlotData::AdventureModeProgression adventure_mode_progression() override
    {
        return {};
    }

    PVZRAPData::SlotData::LevelRandomisation minigame_levels() override
    {
        return {};
    }
    
    PVZRAPData::SlotData::LevelRandomisation puzzle_levels() override
    {
        return {};
    }
    
    PVZRAPData::SlotData::LevelRandomisation survival_levels() override
    {
        return {};
    }

    std::map<int, int> minigame_unlocks() override
    {
        return {};
    }
    
    std::map<int, int> survival_unlocks() override
    {
        return {};
    }
    
    std::map<int, int> izombie_unlocks() override
    {
        return {};
    }
    
    std::map<int, int> vasebreaker_unlocks() override
    {
        return {};
    }
    
    PVZRAPData::SlotData::GoalProgress goal_requirements() override
    {
        return {};
    }
    
    bool fast_goal() override
    {
        return {};
    }
    
    std::optional<std::set<ZombieType>> zombies_on_level(int level) override
    {
        return {};
    }
    
    std::optional<std::map<SeedType, int>> conveyor_seeds_for_level(int level) override
    {
        return {};
    }

    std::optional<std::vector<SeedType>> conveyor_order_for_level(int level) override
    {
        return {};        
    }
    
    std::optional<std::map<ZombieType, int>> zombie_weights_for_level(int level) override
    {
        return {};
    }
    
    std::optional<std::vector<SeedType>> banned_plants_for_level(int level) override
    {
        return {};
    }
    
    bool lock_conveyor() override
    {
        return {};
    }
    
    bool lock_vasebreaker() override
    {
        return {};
    }
    
    bool lock_izombie() override
    {
        return {};
    }

    bool easy_upgrade_plants() override
    {
        return {};
    }
    
    bool disable_storm_flashes() override
    {
        return {};
    }
    
    bool imitater_open() override
    {
        return {};
    }
    
    bool deathlink_enabled() override
    {
        return {};
    }
    
    bool energylink_enabled() override
    {
        return {};
    }
    
    bool ringlink_enabled() override
    {
        return {};
    }
    
    bool lawnlink_enabled() override
    {
        return {};
    }
    
    bool seedlink_enabled() override
    {
        return {};
    }
    
    bool harder_zombie_spawns() override
    {
        return {};
    }
    
    std::optional<PVZRAPData::SlotData::LawnlinkChance> lawnlink_chances() override
    {
        return {};
    }
    
    bool progressive_sun_capacity_items() override
    {
        return {};
    }
    
    bool individual_tile_unlock_items() override
    {
        return {};
    }
    
    bool is_eligible_for_individual_tile_unlock_items(int level) override
    {
        return {};
    }
    
    bool is_eligible_for_harder_zombie_spawns(int level) override
    {
        return {};
    }
    
    std::optional<PVZRAPData::SlotData::SeedStats> seed_stats(SeedType seed) override
    {
        return {};
    }
    
    std::optional<PVZRAPData::SlotData::ProjectileStats> projectile_stats(const ProjectileType projectile) override
    {
        return {};
    }
    
    int sun_per_upgrade() override
    {
        return {};
    }
    
    std::optional<std::map<SeedType, int>> vasebreaker_seeds_for_level(int level, int wave) override
    {
        return {};
    }
    
    std::optional<std::map<ZombieType, int>> vasebreaker_zombies_for_level(int level, int wave) override
    {
        return {};
    }
};


PVZRAPData::SlotData::SlotData(const SlotData& other)
{
    this->inner = other.inner;
    this->wrapper = other.wrapper;
}

PVZRAPData::SlotData PVZRAPData::SlotData::get_slot_data(APWrapper* wrapper, const nlohmann::json& slot_data)
{
    auto gen_version = slot_data["gen_version"];
    std::string gen_version_string;
    if (gen_version.is_number_float())
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << gen_version;
        gen_version_string = ss.str();
    }
    else
    {
        gen_version_string = gen_version.get<std::string>();
    }

    if (gen_version_string == "1.7")
    {
        return SlotData(wrapper, std::make_shared<SlotData1_7>(slot_data));
    }

    if (gen_version_string == "1.8")
    {
        return SlotData(wrapper, std::make_shared<SlotData1_8>(slot_data));
    }

    if (gen_version_string == "1.9")
    {
        return SlotData(wrapper, std::make_shared<SlotData1_9>(slot_data));
    }
    
    if (gen_version_string == "1.10")
    {
        return SlotData(wrapper, std::make_shared<SlotData1_10>(slot_data));
    }

    return SlotData(wrapper, std::make_shared<SlotDataInvalid>(gen_version_string));
}

bool PVZRAPData::SlotData::is_valid() const
{
    return inner->is_valid();
}

std::string PVZRAPData::SlotData::version() const
{
    return inner->version();
}

bool PVZRAPData::SlotData::requires_replanted() const
{
    return inner->requires_replanted();
}

PVZRAPData::SlotData::AdventureModeProgression PVZRAPData::SlotData::adventure_mode_progression() const
{
    return inner->adventure_mode_progression();
}

PVZRAPData::SlotData::LevelRandomisation PVZRAPData::SlotData::minigame_levels() const
{
    return inner->minigame_levels();
}

PVZRAPData::SlotData::LevelRandomisation PVZRAPData::SlotData::puzzle_levels() const
{
    return inner->puzzle_levels();
}

PVZRAPData::SlotData::LevelRandomisation PVZRAPData::SlotData::survival_levels() const
{
    return inner->survival_levels();
}

std::map<int, int> PVZRAPData::SlotData::minigame_unlocks() const
{
    return inner->minigame_unlocks();
}

std::map<int, int> PVZRAPData::SlotData::survival_unlocks() const
{
    return inner->survival_unlocks();
}

std::map<int, int> PVZRAPData::SlotData::izombie_unlocks() const
{
    return inner->izombie_unlocks();
}

std::map<int, int> PVZRAPData::SlotData::vasebreaker_unlocks() const
{
    return inner->vasebreaker_unlocks();
}

PVZRAPData::SlotData::GoalProgress PVZRAPData::SlotData::goal_requirements() const
{
    return inner->goal_requirements();
}

bool PVZRAPData::SlotData::fast_goal() const
{
    return inner->fast_goal();
}

std::optional<std::set<ZombieType>> PVZRAPData::SlotData::zombies_on_level(int level) const
{
    return inner->zombies_on_level(level);
}

std::optional<std::map<SeedType, int>> PVZRAPData::SlotData::conveyor_seeds_for_level(int level) const
{
    return inner->conveyor_seeds_for_level(level);
}

std::optional<std::vector<SeedType>> PVZRAPData::SlotData::conveyor_order_for_level(int level) const
{
    return inner->conveyor_order_for_level(level);
}

std::optional<std::map<ZombieType, int>> PVZRAPData::SlotData::zombie_weights_for_level(int level) const
{
    return inner->zombie_weights_for_level(level);
}

std::optional<std::vector<SeedType>> PVZRAPData::SlotData::banned_plants_for_level(int level) const
{
    return inner->banned_plants_for_level(level);
}

std::optional<std::map<SeedType, int>> PVZRAPData::SlotData::vasebreaker_seeds_for_level(int level, int wave) const
{
    return inner->vasebreaker_seeds_for_level(level, wave);
}

std::optional<std::map<ZombieType, int>> PVZRAPData::SlotData::vasebreaker_zombies_for_level(int level, int wave) const
{
    return inner->vasebreaker_zombies_for_level(level, wave);
}

bool PVZRAPData::SlotData::lock_conveyor() const
{
    return inner->lock_conveyor();
}

bool PVZRAPData::SlotData::lock_vasebreaker() const
{
    return inner->lock_vasebreaker();
}

bool PVZRAPData::SlotData::lock_izombie() const
{
    return inner->lock_izombie();
}

bool PVZRAPData::SlotData::easy_upgrade_plants() const
{
    return inner->easy_upgrade_plants();
}

bool PVZRAPData::SlotData::disable_storm_flashes() const
{
    auto slot_data = this->wrapper->ReadDataStorage(this->wrapper->DataStorageSlot(APWrapper::KnownDataStorageKey::DisableStormFlashes));
    if (slot_data.is_boolean())
    {
        return slot_data.get<bool>();
    }
    return inner->disable_storm_flashes();
}

bool PVZRAPData::SlotData::imitater_open() const
{
    auto slot_data = this->wrapper->ReadDataStorage(this->wrapper->DataStorageSlot(APWrapper::KnownDataStorageKey::OpenImitaterEnabled));
    if (slot_data.is_boolean())
    {
        return slot_data.get<bool>();
    }
    return inner->imitater_open();
}

int PVZRAPData::SlotData::sun_per_upgrade() const
{
    return inner->sun_per_upgrade();
}

bool PVZRAPData::SlotData::progressive_sun_capacity_items() const
{
    return inner->progressive_sun_capacity_items();
}

bool PVZRAPData::SlotData::individual_tile_unlock_items() const
{
    return inner->individual_tile_unlock_items();
}

bool PVZRAPData::SlotData::is_eligible_for_individual_tile_unlock_items(int level) const
{
    return inner->is_eligible_for_individual_tile_unlock_items(level);
}

bool PVZRAPData::SlotData::is_eligible_for_harder_zombie_spawns(int level) const
{
    return inner->is_eligible_for_harder_zombie_spawns(level);
}

bool PVZRAPData::SlotData::deathlink_enabled() const
{
    auto slot_data = this->wrapper->ReadDataStorage(this->wrapper->DataStorageSlot(APWrapper::KnownDataStorageKey::DeathLinkEnabled));
    if (slot_data.is_boolean())
    {
        return slot_data.get<bool>();
    }
    return inner->deathlink_enabled();
}

bool PVZRAPData::SlotData::energylink_enabled() const
{
    auto slot_data = this->wrapper->ReadDataStorage(this->wrapper->DataStorageSlot(APWrapper::KnownDataStorageKey::EnergyLinkEnabled));
    if (slot_data.is_boolean())
    {
        return slot_data.get<bool>();
    }
    return inner->energylink_enabled();
}

bool PVZRAPData::SlotData::ringlink_enabled() const
{
    auto slot_data = this->wrapper->ReadDataStorage(this->wrapper->DataStorageSlot(APWrapper::KnownDataStorageKey::RingLinkEnabled));
    if (slot_data.is_boolean())
    {
        return slot_data.get<bool>();
    }
    return inner->ringlink_enabled();
}

bool PVZRAPData::SlotData::lawnlink_enabled() const
{
    auto slot_data = this->wrapper->ReadDataStorage(this->wrapper->DataStorageSlot(APWrapper::KnownDataStorageKey::LawnLinkEnabled));
    if (slot_data.is_boolean())
    {
        return slot_data.get<bool>();
    }
    return inner->lawnlink_enabled();
}

bool PVZRAPData::SlotData::seedlink_enabled() const
{
    auto slot_data = this->wrapper->ReadDataStorage(this->wrapper->DataStorageSlot(APWrapper::KnownDataStorageKey::SeedLinkEnabled));
    if (slot_data.is_boolean())
    {
        return slot_data.get<bool>();
    }
    return inner->seedlink_enabled();
}

bool PVZRAPData::SlotData::harder_zombie_spawns() const
{
    auto slot_data = this->wrapper->ReadDataStorage(this->wrapper->DataStorageSlot(APWrapper::KnownDataStorageKey::HarderZombieSpawnsEnabled));
    if (slot_data.is_boolean())
    {
        return slot_data.get<bool>();
    }
    return inner->harder_zombie_spawns();
}

std::optional<PVZRAPData::SlotData::LawnlinkChance> PVZRAPData::SlotData::lawnlink_chances() const
{
    return inner->lawnlink_chances();
}

std::optional<PVZRAPData::SlotData::SeedStats> PVZRAPData::SlotData::seed_stats(const SeedType seed) const
{
    return inner->seed_stats(seed);
}

std::optional<PVZRAPData::SlotData::ProjectileStats> PVZRAPData::SlotData::projectile_stats(const ProjectileType projectile) const
{
    return inner->projectile_stats(projectile);
}

PVZRAPData::SlotData::SlotData(APWrapper* wrapper, const std::shared_ptr<SlotDataInner>& inner) : inner(inner)
{
    this->wrapper = wrapper;
}
