#ifndef AP_WRAPPER_H
#define AP_WRAPPER_H

#include <functional>
#include <set>
#include <string>
#include <nlohmann/json_fwd.hpp>

class APWrapper;
class APWrapper_Private;

class ListenerHandle
{
public:
    ~ListenerHandle();
    
protected:
    friend APWrapper;
    explicit ListenerHandle(std::function<void()> on_delete);
    
private:
    std::function<void()> on_delete;
};

// Keep up to date with APClient::NetworkItem
struct APItem
{
    int64_t item;
    int64_t location;
    int player;
    unsigned flags;
    int index = -1; // to sync items, not actually part of NetworkItem
    
    static constexpr unsigned ITEM_FLAG_PROGRESSION = 0b001;
    static constexpr unsigned ITEM_FLAG_USEFUL = 0b010;
    static constexpr unsigned ITEM_FLAG_TRAP = 0b100;
};
    
struct Hint
{
    int receiving_player;
    int finding_player;
    int location;
    int item;
    bool found;
    std::string entrance;
    int item_flags;
    int status;
};

struct DataStoragePendingOperationPrivate;

class DataStoragePendingOperation
{
public:
    ~DataStoragePendingOperation();
    
    DataStoragePendingOperation replace(nlohmann::json value);
    DataStoragePendingOperation de_fault(nlohmann::json value);
    DataStoragePendingOperation add(nlohmann::json value);
    DataStoragePendingOperation mul(nlohmann::json value);
    DataStoragePendingOperation pow(nlohmann::json value);
    DataStoragePendingOperation mod(nlohmann::json value);
    DataStoragePendingOperation floor(nlohmann::json value);
    DataStoragePendingOperation ceil(nlohmann::json value);
    DataStoragePendingOperation maximum(nlohmann::json value);
    DataStoragePendingOperation minimum(nlohmann::json value);
    DataStoragePendingOperation data_and(nlohmann::json value);
    DataStoragePendingOperation data_or(nlohmann::json value);
    DataStoragePendingOperation data_xor(nlohmann::json value);
    DataStoragePendingOperation left_shift(nlohmann::json value);
    DataStoragePendingOperation right_shift(nlohmann::json value);
    DataStoragePendingOperation remove(nlohmann::json value);
    DataStoragePendingOperation pop(nlohmann::json value);
    DataStoragePendingOperation update(nlohmann::json value);
    
protected:
    friend APWrapper;
    explicit DataStoragePendingOperation(APWrapper* parent, std::string key, nlohmann::json& default_value);
    explicit DataStoragePendingOperation(std::unique_ptr<DataStoragePendingOperationPrivate> d);
    
private:
    std::unique_ptr<DataStoragePendingOperationPrivate> d;
};

class APWrapper
{
public:
    APWrapper();
    ~APWrapper();
    
    enum class ConnectionStatus
    {
        Disconnected,
        Connecting,
        Connected
    };
    
    enum class KnownDataStorageKey
    {
        EnergyLink,
        ClientStatus,
        Hints,
        
        DeathLinkEnabled,
        RingLinkEnabled,
        EnergyLinkEnabled,
        SeedLinkEnabled,
        LawnLinkEnabled,
        HarderZombieSpawnsEnabled,
        DisableStormFlashes,
        OpenImitaterEnabled
    };
    
    enum class LawnLinkAction
    {
        PlantAdded = 0,
        PlantRemoved = 1,
    };
    
    struct LawnLinkData
    {
        LawnLinkAction action;
        int row;
        int column;
        int seed;
        bool conveyor;
        std::string player;
    };
    
    void Connect(const std::string& server_name, const std::string& slot_name, const std::string& password = "");
    void Disconnect() const;
    void DisconnectNow() const;
    std::string ServerName() const;
    std::string SlotName() const;
    std::string Password() const;
    ConnectionStatus ConnectionStatus() const;
    
    nlohmann::json SlotData();
    int64_t MySlot() const;
    bool IsGoalReached() const;
    bool CanReleaseItems() const;
    void ReleaseItems() const;
    
    std::string PlayerDisplayName(int slot) const;
    std::string PlayerGameName(int slot) const;
    bool IsPlayerPlayingPVZ(int slot) const;
    
    std::string ItemName(const APItem& item) const;
    std::string ItemName(int64_t item, int64_t slot) const;
    int64_t ReceivedItemCount(int item) const;
    std::list<APItem> ReceivedItems() const;

    void Poll() const;
    
    void CheckLocations(const std::list<int64_t>& location_ids) const;
    bool IsLocationChecked(const int64_t& location) const;
    bool IsLocationPresent(const int64_t& location) const;
    std::set<int64_t> UncheckedLocations() const;
    bool AllLocationsChecked() const;
    APItem ItemAtLocation(int64_t location) const;
    void SetGoal() const;
    std::string LocationName(const int64_t location, std::string game_name);
    
    std::list<Hint> Hints();
    void HintLocations(std::list<int64_t> locations) const;
    
    void EnableDeathLink(bool enable) const;
    void SendDeathLink(const std::string& reason) const;
    void ClearLastDeathLink() const;
    std::string LastDeathLinkSource() const;
    std::string LastDeathLinkCause() const;
    
    void EnableRingLink(bool enable) const;
    void SendRingLink(long delta) const;
    
    void EnableSeedLink(bool enable) const;
    void SendSeedLink(int seed) const;
    
    void EnableLawnLink(bool enable) const;
    void SendLawnLink(const LawnLinkData& data) const;
    
    std::list<std::string> ChatMessages() const;
    void SendAPMessage(const std::string& message) const;
    void PushMessageHistory(const std::string& message) const;
    std::string HistoryItem(int index) const;
    int HistoryLength() const;
    
    std::string DataStorageSlotPrefixed(std::string key) const;
    std::string DataStorageSlot(KnownDataStorageKey key) const;
    nlohmann::json ReadDataStorage(std::string key) const;
    DataStoragePendingOperation WriteDataStorage(std::string key, nlohmann::json default_value);
    
    ListenerHandle* AddServerChatMessageListener(std::function<void(const std::string&)>) const;
    ListenerHandle* AddCountdownChatMessageListener(std::function<void(const std::string&)>) const;
    ListenerHandle* AddItemsSentListener(std::function<void(const APItem&, const int&)>) const;
    ListenerHandle* AddItemsReceivedListener(std::function<void(const std::list<APItem>&)>) const;
    ListenerHandle* AddConnectionCompleteListener(std::function<void()>) const;
    ListenerHandle* AddSlotRefusedListener(std::function<void(const std::string&)>) const;
    ListenerHandle* AddDisconnectionListener(std::function<void()>) const;
    ListenerHandle* AddDeathLinkListener(std::function<void(const std::string&, const std::string&)>) const;
    ListenerHandle* AddRingLinkListener(std::function<void(const long&)>) const;
    ListenerHandle* AddSeedLinkListener(std::function<void(const int&, std::string)>) const;
    ListenerHandle* AddLawnLinkListener(std::function<void(const LawnLinkData&)>) const;
    ListenerHandle* AddAnyChatMessageListener(std::function<void(const std::string&)>) const;
    ListenerHandle* AddDataStorageValueChangeListener(std::function<void(const std::string&, const nlohmann::json&)>) const;

protected:
    friend DataStoragePendingOperation;
    APWrapper_Private* d;
    
    void UpdateConnectionInformation() const;
};

#endif
