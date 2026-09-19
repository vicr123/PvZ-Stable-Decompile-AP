#include "APWrapper.h"

#define APCLIENT_DEBUG
#define _WEBSOCKETPP_CPP11_STL_

#define _WIN32_WINNT 0x0600
#include <apclient.hpp>
#include <apuuid.hpp>

#include "APData.h"

const std::string EXPECTED_SLOT_DATA_VERSION = "1.3";

class APWrapper_Private {
public:
    std::map<uint64_t, std::function<void(const std::string&)>> server_chat_listeners;
    std::map<uint64_t, std::function<void(const std::string&)>> countdown_chat_listeners;
    std::map<uint64_t, std::function<void(const APItem&, const int&)>> item_sent_listeners;
    std::map<uint64_t, std::function<void(const std::list<APItem>&)>> item_received_listeners;
    std::map<uint64_t, std::function<void()>> connection_complete_listener;
    std::map<uint64_t, std::function<void()>> disconnection_listener;
    std::map<uint64_t, std::function<void(const std::string&)>> slot_refused_listeners;
    std::map<uint64_t, std::function<void(const std::string&, const std::string&)>> deathlink_listeners;
    std::map<uint64_t, std::function<void(const long&)>> ringlink_listeners;
    std::map<uint64_t, std::function<void(const int&, std::string)>> seedlink_listeners;
    std::map<uint64_t, std::function<void(const APWrapper::LawnLinkData&)>> lawnlink_listeners;
    std::map<uint64_t, std::function<void(const std::string&)>> any_chat_listeners;
    std::map<uint64_t, std::function<void(const std::string&, const nlohmann::json&)>> data_storage_value_change_listeners;
    
    uint64_t next_listener_id = 0;
    
    APClient* mAP = nullptr;
    std::string server_name;
    std::string slot_name;
    std::string password;
    nlohmann::json slot_data;
    
    std::list<APItem> received_items;
    std::map<int64_t, APItem> location_scouts;
    
    std::list<std::string> tags;
    double last_sent_deathlink;
    std::string last_deathlink_source;
    std::string last_deathlink_cause;
    long our_ringlink_source;
    bool processing_lawn_link;
    
    std::list<std::string> chat_messages;
    std::vector<std::string> message_history;
    std::map<std::string, nlohmann::json> data_storage;
    
    bool delete_on_next_poll = false;
};

ListenerHandle::~ListenerHandle()
{
    on_delete();
}

ListenerHandle::ListenerHandle(std::function<void()> on_delete)
    : on_delete(on_delete)
{
}

struct DataStoragePendingOperationPrivate
{
    APWrapper* parentWrapper;
    std::string key;
    nlohmann::json default_value;
    std::list<APClient::DataStorageOperation> ops;
};

DataStoragePendingOperation DataStoragePendingOperation::replace(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"replace", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::de_fault(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"default", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::add(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"add", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::mul(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"mul", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::pow(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"pow", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::mod(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"mod", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::floor(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"floor", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::ceil(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"ceil", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::maximum(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"max", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::minimum(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"min", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::data_and(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"and", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::data_or(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"or", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::data_xor(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"xor", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::left_shift(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"left_shift", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::right_shift(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"right_shift", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::remove(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"remove", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::pop(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"pop", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation DataStoragePendingOperation::update(nlohmann::json value)
{
    d->ops.emplace_back(APClient::DataStorageOperation{"update", value});
    return DataStoragePendingOperation(std::move(d));
}

DataStoragePendingOperation::DataStoragePendingOperation(APWrapper* parent, std::string key,
                                                         nlohmann::json& default_value)
{
    d = std::make_unique<DataStoragePendingOperationPrivate>();
    d->parentWrapper = parent;
    d->key = key;
    d->default_value = default_value;
}

DataStoragePendingOperation::DataStoragePendingOperation(std::unique_ptr<DataStoragePendingOperationPrivate> d)
{
    this->d = std::move(d);
}

DataStoragePendingOperation::~DataStoragePendingOperation()
{
    if (d)
    {
        d->parentWrapper->d->mAP->Set(d->key, d->default_value, true, d->ops);
    }
}

APWrapper::APWrapper() : d(new APWrapper_Private)
{
}

APWrapper::~APWrapper()
{
    delete d->mAP;
    delete d;
}

void APWrapper::Connect(const std::string& server_name, const std::string& slot_name, const std::string& password)
{
    // Already connected
    if (d->mAP)
    {
        return;
    }
    
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, std::numeric_limits<long>::max());
    
    d->server_name = server_name;
    d->slot_name = slot_name;
    d->password = password;
    d->our_ringlink_source = distribution(generator);
    d->tags.clear();
    d->chat_messages.clear();
    d->data_storage.clear();
    d->received_items.clear();
    d->location_scouts.clear();
    d->processing_lawn_link = false;
    
    d->mAP = new APClient(ap_get_uuid("uuid.txt"), "Plants vs. Zombies", server_name);
    d->mAP->set_print_handler([](const std::string& print_line)
    {
        std::cout << "Archipelago: " << print_line << std::endl;
    });
    d->mAP->set_print_json_handler([this](const APClient::PrintJSONArgs& print_line)
    {
        std::string concatenated_message;
        for (const auto& node : print_line.data)
        {
            if (node.type == "player_id")
            {
                concatenated_message.append(PlayerDisplayName(std::stoi(node.text)));
            }
            else if (node.type == "item_id")
            {
                concatenated_message.append(ItemName(std::stoi(node.text), node.player));
            }
            else if (node.type == "location_id")
            {
                auto player_game = PlayerGameName(node.player);
                auto location_name = d->mAP->get_location_name(std::stoi(node.text), player_game);
                concatenated_message.append(location_name);
            }
            else
            {
                concatenated_message.append(node.text);
            }
        }
        
        d->chat_messages.push_back(concatenated_message);
        
        if (print_line.type == "ServerChat")
        {
            for (const auto& server_chat_listener : this->d->server_chat_listeners)
            {
                server_chat_listener.second(concatenated_message);
            }
        }
        else if (print_line.type == "ItemSend")
        {
            APItem item {
                print_line.item->item,
                print_line.item->location,
                print_line.item->player,
                print_line.item->flags,
                print_line.item->index
            };
            for (const auto& item_sent_listener : this->d->item_sent_listeners)
            {
                item_sent_listener.second(item, *print_line.receiving);
            }
        }
        else if (print_line.type == "Countdown")
        {
            auto countdown_remaining = *print_line.countdown == 0 ? "GO!" : std::to_string(*print_line.countdown);
            for (const auto& countdown_chat_listener : this->d->countdown_chat_listeners)
            {
                countdown_chat_listener.second(countdown_remaining);
            }
        }
        
        for (const auto& any_chat_listener : this->d->any_chat_listeners)
        {
            any_chat_listener.second(concatenated_message);
        }
    });
    d->mAP->set_socket_connected_handler([this, slot_name, password]
    {
        d->mAP->ConnectSlot(slot_name, password, 0b111 /* Everything! */, d->tags);
    });
    d->mAP->set_slot_connected_handler([this](const nlohmann::json& slot_data)
    {
        auto checked_data = PVZRAPData::SlotData::get_slot_data(this, slot_data);
        if (!checked_data.is_valid())
        {
            // Call the slot refused listener
            for (const auto& slot_refused_listener : this->d->slot_refused_listeners)
            {
                slot_refused_listener.second("IncompatibleSlotData:" + checked_data.version());
            }
            this->Disconnect();
            return;
        }
        
        this->d->slot_data = slot_data;
        
        // Scout all locations and cache them
        auto scouted_locations = this->d->mAP->get_missing_locations();
        scouted_locations.merge(this->d->mAP->get_checked_locations());
        
        d->mAP->LocationScouts(std::list(scouted_locations.begin(), scouted_locations.end()), 0);
        
        auto data_storage_requested_keys = {
            DataStorageSlotPrefixed("profileGuids"),
            DataStorageSlot(KnownDataStorageKey::ClientStatus),
            DataStorageSlot(KnownDataStorageKey::EnergyLink),
            DataStorageSlot(KnownDataStorageKey::Hints),
            DataStorageSlot(KnownDataStorageKey::DeathLinkEnabled),
            DataStorageSlot(KnownDataStorageKey::RingLinkEnabled),
            DataStorageSlot(KnownDataStorageKey::EnergyLinkEnabled),
            DataStorageSlot(KnownDataStorageKey::SeedLinkEnabled),
            DataStorageSlot(KnownDataStorageKey::LawnLinkEnabled),
        };
        d->mAP->SetNotify(data_storage_requested_keys);
        
        // Default profile GUIDs
        this->WriteDataStorage(DataStorageSlotPrefixed("profileGuids"), nlohmann::json::array()).de_fault(nlohmann::json::array());
        this->WriteDataStorage(DataStorageSlot(KnownDataStorageKey::EnergyLink), 0).de_fault(0);
        
        d->mAP->Get({
            DataStorageSlot(KnownDataStorageKey::ClientStatus),
            DataStorageSlot(KnownDataStorageKey::Hints),
            DataStorageSlot(KnownDataStorageKey::DeathLinkEnabled),
            DataStorageSlot(KnownDataStorageKey::RingLinkEnabled),
            DataStorageSlot(KnownDataStorageKey::EnergyLinkEnabled),
            DataStorageSlot(KnownDataStorageKey::SeedLinkEnabled),
            DataStorageSlot(KnownDataStorageKey::LawnLinkEnabled),
            DataStorageSlot(KnownDataStorageKey::HarderZombieSpawnsEnabled),
            DataStorageSlot(KnownDataStorageKey::DisableStormFlashes),
            DataStorageSlot(KnownDataStorageKey::OpenImitaterEnabled)
        });
        
        for (const auto& connection_complete_listener : this->d->connection_complete_listener)
        {
            connection_complete_listener.second();
        }
    });
    d->mAP->set_slot_refused_handler([this](const std::list<std::string>& errors) {
        auto error = errors.back();
        for (const auto& slot_refused_listener : this->d->slot_refused_listeners)
        {
            slot_refused_listener.second(error);
        }
        this->Disconnect();
    });
    d->mAP->set_socket_error_handler([this](const std::string& error_message)
    {
        // this->Disconnect();
    });
    d->mAP->set_items_received_handler([this](const std::list<APClient::NetworkItem>& items) {
        std::list<APItem> ap_items;
        for (const auto item : items)
        {
            ap_items.push_back(APItem {
                item.item,
                item.location,
                item.player,
                item.flags,
                item.index
            });
        }
        
        auto ap_items_clone = ap_items;
        d->received_items.splice(d->received_items.end(), ap_items_clone);

        for (const auto& item_received_listener : this->d->item_received_listeners)
        {
            item_received_listener.second(ap_items);
        }
    });
    d->mAP->set_socket_disconnected_handler([this]
    {
        this->Disconnect();
    });
    d->mAP->set_bounced_handler([this](const nlohmann::json& bounce_data)
    {
        auto tags = bounce_data["tags"];
        if (tags.size() > 0)
        {
            if (tags[0] == "DeathLink")
            {
                // Handle DeathLink packet
                auto data = bounce_data["data"];
                std::string source = data["source"];
                auto cause = data["cause"]; // string
                double time = data["time"]; // number_float
                
                if (abs(time - d->last_sent_deathlink) > 0.001)
                {
                    d->last_deathlink_source = source;
                    if (cause.is_string())
                    {
                        d->last_deathlink_cause = cause;
                    } else
                    {
                        d->last_deathlink_cause = "";
                    }
                        
                    for (const auto& deathlink_listener : this->d->deathlink_listeners)
                    {
                        deathlink_listener.second(source, d->last_deathlink_cause);
                    }
                }
            }
            
            if (tags[0] == "RingLink")
            {
                // Handle RingLink packet
                auto data = bounce_data["data"];
                auto source_json = data["source"];
                auto amount_json = data["amount"];
                
                if (!source_json.is_number_integer() || !amount_json.is_number_integer())
                {
                    return;
                }
                
                auto source = source_json.get<long>();
                auto amount = amount_json.get<long>();
                
                if (source != d->our_ringlink_source)
                {
                    for (const auto& ringlink_listener : this->d->ringlink_listeners)
                    {
                        ringlink_listener.second(amount);
                    }
                }
            }
            
            if (tags[0] == "SeedLink")
            {
                // Handle SeedLink packet
                auto data = bounce_data["data"];
                auto source_json = data["source"];
                auto team_json = data["team"];
                auto seed_json = data["seed"];
                
                if (team_json.is_number() && team_json.get<int>() != d->mAP->get_team_number())
                {
                    return;
                }
                
                if (!source_json.is_number_integer() || !seed_json.is_number_integer())
                {
                    return;
                }
                
                auto source = source_json.get<long>();
                auto seed = seed_json.get<int>();
                
                if (source != d->mAP->get_player_number())
                {
                    auto player = d->mAP->get_player_alias(source);
                    for (const auto& seedlink_listener : this->d->seedlink_listeners)
                    {
                        seedlink_listener.second(seed, player);
                    }
                }
            }
            
            if (tags[0] == "LawnLink")
            {
                // Handle LawnLink packet
                auto data = bounce_data["data"];
                auto source_json = data["source"];
                auto team_json = data["team"];
                auto action_json = data["action"];
                auto row_json = data["row"];
                auto column_json = data["column"];
                auto seed_json = data["seed"];
                auto conveyor_json = data["conveyor"];
                
                if (team_json.is_number() && team_json.get<int>() != d->mAP->get_team_number())
                {
                    return;
                }
                
                if (!source_json.is_number_integer() || !action_json.is_number_integer() || !seed_json.is_number_integer() || !row_json.is_number_integer() || !column_json.is_number_integer() || !conveyor_json.is_boolean())
                {
                    return;
                }
                
                auto source = source_json.get<long>();
                auto action = static_cast<LawnLinkAction>(action_json.get<int>());
                auto row = row_json.get<int>();
                auto column = column_json.get<int>();
                auto seed = seed_json.get<int>();
                auto conveyor = conveyor_json.get<bool>();
                
                if (source != d->mAP->get_player_number())
                {
                    auto player = d->mAP->get_player_alias(source);
                    d->processing_lawn_link = true;
                    LawnLinkData lawnlink_data {
                        action,
                        row,
                        column,
                        seed,
                        conveyor,
                        player
                    };
                    
                    for (const auto& lawnlink_listener : this->d->lawnlink_listeners)
                    {
                        lawnlink_listener.second(lawnlink_data);
                    }
                    d->processing_lawn_link = false;
                }
            }
        }
    });
    d->mAP->set_location_info_handler([this](const std::list<APClient::NetworkItem>& location_info)
    {
        for (const auto& location : location_info)
        {
            d->location_scouts[location.location] = APItem {
                location.item,
                location.location,
                location.player,
                location.flags,
                location.index
            };
        }
    });
    d->mAP->set_set_reply_handler([this](const std::string& key, const nlohmann::json& value, const nlohmann::json& original_value)
    {
        d->data_storage.insert_or_assign(key, value);
        for (auto data_storage_value_change_listener : d->data_storage_value_change_listeners)
        {
            data_storage_value_change_listener.second(key, value);
        }
    });
    d->mAP->set_retrieved_handler([this](const std::map<std::string, nlohmann::json>& keys, const nlohmann::json& message)
    {
        for (const auto& [key, value] : keys)
        {
            d->data_storage.insert_or_assign(key, value);     
            for (auto data_storage_value_change_listener : d->data_storage_value_change_listeners)
            {
                data_storage_value_change_listener.second(key, value);
            }
        }
    });
}

void APWrapper::Disconnect() const
{
    d->delete_on_next_poll = true;
    for (const auto& disconnection_listener : this->d->disconnection_listener)
    {
        disconnection_listener.second();
    }
}

void APWrapper::DisconnectNow() const
{
    delete d->mAP;
    d->mAP = nullptr;
}

std::string APWrapper::ServerName() const
{
    return d->server_name;
}

std::string APWrapper::SlotName() const
{
    return d->slot_name;
}

std::string APWrapper::Password() const
{
    return d->password;
}

enum APWrapper::ConnectionStatus APWrapper::ConnectionStatus() const
{
    if (!d->mAP || d->delete_on_next_poll) return ConnectionStatus::Disconnected;

    switch (d->mAP->get_state())
    {
    case APClient::State::SLOT_CONNECTED:
        return ConnectionStatus::Connected;
    case APClient::State::DISCONNECTED:
    case APClient::State::SOCKET_CONNECTING:
    case APClient::State::SOCKET_CONNECTED:
    case APClient::State::ROOM_INFO:
        return ConnectionStatus::Connecting;
    }
    
    return ConnectionStatus::Disconnected;
}

nlohmann::json APWrapper::SlotData()
{
    return d->slot_data;
}

void APWrapper::SendAPMessage(const std::string& message) const
{
    if (!d->mAP) return;
    d->mAP->Say(message);
}

void APWrapper::PushMessageHistory(const std::string& message) const
{
    d->message_history.push_back(message);
}

std::string APWrapper::HistoryItem(int index) const
{
    return d->message_history[index];
}

int APWrapper::HistoryLength() const
{
    return d->message_history.size();
}

std::string APWrapper::DataStorageSlotPrefixed(std::string key) const
{
    return "Slot:" + std::to_string(d->mAP->get_player_number()) + ":" + key;
}

std::string APWrapper::DataStorageSlot(KnownDataStorageKey key) const
{
    switch (key)
    {
    case KnownDataStorageKey::EnergyLink:
        return std::format("EnergyLink{}", d->mAP->get_team_number());
    case KnownDataStorageKey::ClientStatus:
        return std::string("_read_client_status_") + std::to_string(d->mAP->get_team_number()) + "_" + std::to_string(d->mAP->get_player_number());
    case KnownDataStorageKey::Hints:
        return std::string("_read_hints_") + std::to_string(d->mAP->get_team_number()) + "_" + std::to_string(d->mAP->get_player_number());
    case KnownDataStorageKey::DeathLinkEnabled:
        return DataStorageSlotPrefixed("deathLinkEnabled");
    case KnownDataStorageKey::RingLinkEnabled:
        return DataStorageSlotPrefixed("ringLinkEnabled");
    case KnownDataStorageKey::EnergyLinkEnabled:
        return DataStorageSlotPrefixed("energyLinkEnabled");
    case KnownDataStorageKey::SeedLinkEnabled:
        return DataStorageSlotPrefixed("seedLinkEnabled");
    case KnownDataStorageKey::LawnLinkEnabled:
        return DataStorageSlotPrefixed("lawnLinkEnabled");
    case KnownDataStorageKey::HarderZombieSpawnsEnabled:
        return DataStorageSlotPrefixed("harderZombieSpawnsEnabled");
    case KnownDataStorageKey::DisableStormFlashes:
        return DataStorageSlotPrefixed("disableStormFlashes");
    case KnownDataStorageKey::OpenImitaterEnabled:
        return DataStorageSlotPrefixed("imitaterOpen");
    }
    
    return "";
}

nlohmann::json APWrapper::ReadDataStorage(std::string key) const
{
    return d->data_storage[key];
}

DataStoragePendingOperation APWrapper::WriteDataStorage(std::string key, nlohmann::json value)
{
    return DataStoragePendingOperation(this, key, value);
}

int64_t APWrapper::MySlot() const
{
    if (!d->mAP) return -1;
    return d->mAP->get_player_number();
}

bool APWrapper::IsGoalReached() const
{
    if (!d->mAP) return false;
    auto status = ReadDataStorage(DataStorageSlot(KnownDataStorageKey::ClientStatus));
    if (status.is_null())
    {
        return false;
    }
    return static_cast<APClient::ClientStatus>(status.get<int>()) == APClient::ClientStatus::GOAL;
}

bool APWrapper::CanReleaseItems() const
{
    if (!d->mAP) return false;
    auto release_permission = d->mAP->get_permissions().find("release");
    switch (release_permission->second)
    {
    case APClient::Permission::DISABLED:
        return false;
    case APClient::Permission::ENABLED:
    case APClient::Permission::AUTO_ENABLED:
        return true;
    case APClient::Permission::GOAL:
    case APClient::Permission::AUTO:
        return this->IsGoalReached();
    }
    
    return false;
}

void APWrapper::ReleaseItems() const
{
    this->SendAPMessage("!release");
}

std::string APWrapper::PlayerDisplayName(int slot) const
{
    if (!d->mAP) return "";
    
    if (slot == 0) return "Archipelago";
    
    auto players = d->mAP->get_players();
    for (const auto& player : d->mAP->get_players())
    {
        if (player.slot != slot) continue;
        if (player.alias.empty() || player.alias == player.name)
        {
            return player.name;
        }
        return player.name + " (" + player.alias + ")";
    }
    return "";
}

std::string APWrapper::PlayerGameName(int slot) const
{
    if (!d->mAP) return "";
    
    if (slot == 0) return "Archipelago";
    
    return d->mAP->get_player_game(slot);
}

bool APWrapper::IsPlayerPlayingPVZ(int slot) const
{
    if (!d->mAP) return false;
    
    return d->mAP->get_player_game(slot) == d->mAP->get_game();
}

std::string APWrapper::ItemName(const APItem& item) const
{
    const auto player_game = d->mAP->get_player_game(item.player);
    return d->mAP->get_item_name(item.item, player_game);
}

std::string APWrapper::ItemName(const int64_t item, const int64_t slot) const
{
    const auto player_game = d->mAP->get_player_game(slot);
    return d->mAP->get_item_name(item, player_game);
}

int64_t APWrapper::ReceivedItemCount(int item) const
{
    auto count = 0;
    for (const auto& received_item : d->received_items)
    {
        if (received_item.item == item) count++;
    }
    return count;
}

std::list<APItem> APWrapper::ReceivedItems() const
{
    std::list<APItem> items = d->received_items;
    items.sort([](const APItem& a, const APItem& b) { return a.index < b.index; });
    return items;
}

void APWrapper::Poll() const
{
    if (d->delete_on_next_poll)
    {
        delete d->mAP;
        d->mAP = nullptr;
        d->delete_on_next_poll = false;
    }
    
    if (!d->mAP) return;
    this->d->mAP->poll();
}

void APWrapper::CheckLocations(const std::list<int64_t>& location_ids) const
{
    if (!d->mAP) return;
    this->d->mAP->LocationChecks(location_ids);
}

bool APWrapper::IsLocationChecked(const int64_t& location) const
{
    if (!d->mAP) return false;
    auto checked_locations = this->d->mAP->get_checked_locations();
    return checked_locations.find(location) != checked_locations.end();
}

bool APWrapper::IsLocationPresent(const int64_t& location) const
{
    if (!d->mAP) return false;
    if (location == -1) return false;
    auto locations = this->d->mAP->get_missing_locations();
    locations.merge(this->d->mAP->get_checked_locations());
    return locations.find(location) != locations.end();
}

std::set<int64_t> APWrapper::UncheckedLocations() const
{
    return this->d->mAP->get_missing_locations();
}

bool APWrapper::AllLocationsChecked() const
{
    if (!d->mAP)
    {
        return false;
    }
    return this->d->mAP->get_missing_locations().size() == 0;
}

APItem APWrapper::ItemAtLocation(int64_t location) const
{
    return d->location_scouts[location];
}

void APWrapper::SetGoal() const
{
    if (!d->mAP) return;
    this->d->mAP->StatusUpdate(APClient::ClientStatus::GOAL);
}

std::string APWrapper::LocationName(const int64_t location, std::string game_name)
{
    return d->mAP->get_location_name(location, game_name);
}

std::list<Hint> APWrapper::Hints()
{
    auto hint_data_storage = ReadDataStorage(DataStorageSlot(KnownDataStorageKey::Hints));
    std::list<Hint> hints;
    for (auto hint_json : hint_data_storage)
    {
        Hint hint;
        hint.receiving_player = hint_json["receiving_player"];
        hint.finding_player = hint_json["finding_player"];
        hint.location = hint_json["location"];
        hint.item = hint_json["item"];
        hint.found = hint_json["found"];
        hint.entrance = hint_json["entrance"];
        hint.item_flags = hint_json["item_flags"];
        hint.status = hint_json["status"];
        hints.push_back(hint);
    }
    
    return hints;
}

void APWrapper::HintLocations(std::list<int64_t> locations) const
{
    d->mAP->CreateHints(locations, -1, APClient::HintStatus::HINT_UNSPECIFIED);
}

void APWrapper::EnableDeathLink(bool enable) const
{
    if (!enable)
    {
        d->tags.remove_if([](std::string tag)
        {
            return tag == "DeathLink";
        });
    }
    else
    {
        if (std::ranges::find(d->tags, "DeathLink") == d->tags.end())
        {
            d->tags.emplace_back("DeathLink");
        }
    }
    
    this->UpdateConnectionInformation();
}

void APWrapper::SendDeathLink(const std::string& reason) const
{
    // Ensure DeathLink is on
    for (const auto& tag : d->tags)
    {
        if (tag == "DeathLink")
        {
            auto time = d->mAP->get_server_time();
            d->last_sent_deathlink = time;
            d->mAP->Bounce({
                {"time", time},
                {"source", this->PlayerDisplayName(this->MySlot())},
                {"cause", reason}
            }, {}, {}, {"DeathLink"});
            return;
        }
    }
}

void APWrapper::ClearLastDeathLink() const
{
    d->last_deathlink_cause = "";
    d->last_deathlink_source = "";
}

std::string APWrapper::LastDeathLinkSource() const
{
    return d->last_deathlink_source;
}

std::string APWrapper::LastDeathLinkCause() const
{
    return d->last_deathlink_cause;
}

void APWrapper::EnableRingLink(bool enable) const
{
    if (!enable)
    {
        d->tags.remove_if([](std::string tag)
        {
            return tag == "RingLink";
        });
    }
    else
    {
        if (std::ranges::find(d->tags, "RingLink") == d->tags.end())
        {
            d->tags.emplace_back("RingLink");
        }
    }
    
    this->UpdateConnectionInformation();
}

void APWrapper::SendRingLink(long delta) const
{
    // Ensure RingLink is on
    for (const auto& tag : d->tags)
    {
        if (tag == "RingLink")
        {
            auto time = d->mAP->get_server_time();
            d->mAP->Bounce({
                {"time", time},
                {"source", d->our_ringlink_source},
                {"amount", delta}
            }, {}, {}, {"RingLink"});
            return;
        }
    }
}

void APWrapper::EnableSeedLink(bool enable) const
{
    if (!enable)
    {
        d->tags.remove_if([](std::string tag)
        {
            return tag == "SeedLink";
        });
    }
    else
    {
        if (std::ranges::find(d->tags, "SeedLink") == d->tags.end())
        {
            d->tags.emplace_back("SeedLink");
        }
    }
    
    this->UpdateConnectionInformation();
}

void APWrapper::SendSeedLink(int seed) const
{
    // Ensure SeedLink is on
    for (const auto& tag : d->tags)
    {
        if (tag == "SeedLink")
        {
            auto time = d->mAP->get_server_time();
            d->mAP->Bounce({
                {"time", time},
                {"source", d->mAP->get_player_number()},
                {"team", d->mAP->get_team_number()},
                {"seed", seed}
            }, {}, {}, {"SeedLink"});
            return;
        }
    }
}

void APWrapper::EnableLawnLink(bool enable) const
{
    if (!enable)
    {
        d->tags.remove_if([](std::string tag)
        {
            return tag == "LawnLink";
        });
    }
    else
    {
        if (std::ranges::find(d->tags, "LawnLink") == d->tags.end())
        {
            d->tags.emplace_back("LawnLink");
        }
    }
    
    this->UpdateConnectionInformation();
}

void APWrapper::SendLawnLink(const LawnLinkData& data) const
{
    // Ensure LawnLink is on and we aren't processing a LawnLink
    if (d->processing_lawn_link) return;
    
    for (const auto& tag : d->tags)
    {
        if (tag == "LawnLink")
        {
            auto time = d->mAP->get_server_time();
            d->mAP->Bounce({
                {"time", time},
                {"source", d->mAP->get_player_number()},
                {"team", d->mAP->get_team_number()},
                {"action", static_cast<int>(data.action)},
                {"row", data.row},
                {"column", data.column},
                {"seed", data.seed},
                {"conveyor", data.conveyor}
            }, {}, {}, {"LawnLink"});
            return;
        }
    }
}

std::list<std::string> APWrapper::ChatMessages() const
{
    return d->chat_messages;
}

ListenerHandle* APWrapper::AddServerChatMessageListener(std::function<void(const std::string&)> listener) const
{
    auto id = d->next_listener_id++;
    this->d->server_chat_listeners.insert_or_assign(id, listener);
    
    return new ListenerHandle([this, id] { this->d->server_chat_listeners.erase(id); });
}

ListenerHandle* APWrapper::AddCountdownChatMessageListener(std::function<void(const std::string&)> listener) const
{
    auto id = d->next_listener_id++;
    this->d->countdown_chat_listeners.insert_or_assign(id, listener);
    
    return new ListenerHandle([this, id] { this->d->countdown_chat_listeners.erase(id); });
}

ListenerHandle* APWrapper::AddItemsSentListener(std::function<void(const APItem&, const int&)> listener) const
{
    auto id = d->next_listener_id++;
    this->d->item_sent_listeners.insert_or_assign(id, listener);
    
    return new ListenerHandle([this, id] { this->d->item_sent_listeners.erase(id); });
}

ListenerHandle* APWrapper::AddItemsReceivedListener(std::function<void(const std::list<APItem>&)> listener) const
{
    auto id = d->next_listener_id++;
    this->d->item_received_listeners.insert_or_assign(id, listener);
    
    return new ListenerHandle([this, id] { this->d->item_received_listeners.erase(id); });
}

ListenerHandle* APWrapper::AddConnectionCompleteListener(std::function<void()> listener) const
{
    auto id = d->next_listener_id++;
    this->d->connection_complete_listener.insert_or_assign(id, listener);
    
    return new ListenerHandle([this, id] { this->d->connection_complete_listener.erase(id); });
}

ListenerHandle* APWrapper::AddSlotRefusedListener(std::function<void(const std::string&)> listener) const
{
    auto id = d->next_listener_id++;
    this->d->slot_refused_listeners.insert_or_assign(id, listener);
    
    return new ListenerHandle([this, id] { this->d->slot_refused_listeners.erase(id); });
}

ListenerHandle* APWrapper::AddDisconnectionListener(std::function<void()> listener) const
{
    auto id = d->next_listener_id++;
    this->d->disconnection_listener.insert_or_assign(id, listener);
    
    return new ListenerHandle([this, id] { this->d->disconnection_listener.erase(id); });
}

ListenerHandle* APWrapper::AddDeathLinkListener(std::function<void(const std::string&, const std::string&)> listener) const
{
    auto id = d->next_listener_id++;
    this->d->deathlink_listeners.insert_or_assign(id, listener);
    
    return new ListenerHandle([this, id] { this->d->deathlink_listeners.erase(id); });
}

ListenerHandle* APWrapper::AddRingLinkListener(std::function<void(const long&)> listener) const
{
    auto id = d->next_listener_id++;
    this->d->ringlink_listeners.insert_or_assign(id, listener);
    
    return new ListenerHandle([this, id] { this->d->ringlink_listeners.erase(id); });
}

ListenerHandle* APWrapper::AddSeedLinkListener(std::function<void(const int&, std::string)> listener) const
{
    auto id = d->next_listener_id++;
    this->d->seedlink_listeners.insert_or_assign(id, listener);
    
    return new ListenerHandle([this, id] { this->d->seedlink_listeners.erase(id); });
}

ListenerHandle* APWrapper::AddLawnLinkListener(std::function<void(const LawnLinkData&)> listener) const
{
    auto id = d->next_listener_id++;
    this->d->lawnlink_listeners.insert_or_assign(id, listener);
    
    return new ListenerHandle([this, id] { this->d->lawnlink_listeners.erase(id); });
}

ListenerHandle* APWrapper::AddAnyChatMessageListener(std::function<void(const std::string&)> listener) const
{
    auto id = d->next_listener_id++;
    this->d->any_chat_listeners.insert_or_assign(id, listener);
    
    return new ListenerHandle([this, id] { this->d->any_chat_listeners.erase(id); });
}

ListenerHandle* APWrapper::AddDataStorageValueChangeListener(
    std::function<void(const std::string&, const nlohmann::json&)> listener) const
{
    auto id = d->next_listener_id++;
    this->d->data_storage_value_change_listeners.insert_or_assign(id, listener);
    
    return new ListenerHandle([this, id] { this->d->data_storage_value_change_listeners.erase(id); });
}

void APWrapper::UpdateConnectionInformation() const
{
    if (!d->mAP) return;
    
    d->mAP->ConnectUpdate(0b111, d->tags);
}
