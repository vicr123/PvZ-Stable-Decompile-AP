#include "APDialogueClient.h"

#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <nlohmann/json.hpp>
#include <urlmon.h>
#include <wininet.h>
#include <wrl/client.h>
#include "SexyAppBase.h"

const std::string DIALOGUE_BASE_URL = "http://pvz-ap.vicr123.com/";

struct GameMeta
{
    std::vector<std::string> strings;
    bool matureGame;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameMeta, strings, matureGame)

struct Dialogue
{
    std::string crazyDave;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Dialogue, crazyDave)

struct APDialogueClientPrivate
{
    Sexy::SexyAppBase* app;

    std::map<std::string, GameMeta> gameMeta;
    std::map<std::string, std::map<std::string, Dialogue>> gameDialogue;
    std::vector<std::string> requestedGames;
};


void downloadJsonInner(const std::string& url, std::function<void(std::optional<nlohmann::json>)> result_callback)
{
    DeleteUrlCacheEntry(url.c_str());
    Microsoft::WRL::ComPtr<IStream> stream;
    auto hr = URLOpenBlockingStream(nullptr, url.c_str(), stream.GetAddressOf(), 0, nullptr);
    if (FAILED(hr))
    {
        result_callback(std::nullopt);
        return;
    }

    std::vector<BYTE> data;
    BYTE buf[8192];
    ULONG read = 0;
    while (SUCCEEDED(stream->Read(buf, sizeof(buf), &read)) && read > 0)
    {
        data.insert(data.end(), buf, buf + read);
    }

    auto result = nlohmann::json::parse(data, nullptr, false);
    if (result.is_discarded())
    {
        result_callback(std::nullopt);
        return;
    }

    result_callback(result);
}

void downloadJson(Sexy::SexyAppBase* app, const std::string& url,
                  std::function<void(std::optional<nlohmann::json>)> result_callback)
{
    std::thread(downloadJsonInner, url, [app, result_callback](std::optional<nlohmann::json> result)
    {
        std::lock_guard guard(app->mDeferredTasksMutex);
        app->mDeferredTasks.push_back([result_callback, result]
        {
            result_callback(result);
        });
    }).detach();
}

APDialogueClient::APDialogueClient(Sexy::SexyAppBase* app)
{
    d = new APDialogueClientPrivate;
    d->app = app;
    downloadJson(app, DIALOGUE_BASE_URL + "/item-text/meta.json", [this](std::optional<nlohmann::json> result)
    {
        if (result.has_value())
        {
            auto json_result = result.value();
            this->d->gameMeta = json_result.get<std::map<std::string, GameMeta>>();
        }
        else
        {
            std::cout << "APDialogueClient: Failed to download meta.json\n";
        }
    });
}

APDialogueClient::~APDialogueClient()
{
    delete d;
}

void APDialogueClient::DownloadGameMeta(std::string gameName)
{
    if (d->gameMeta.contains(gameName) && std::ranges::find(d->requestedGames, gameName) == d->requestedGames.end())
    {
        d->requestedGames.push_back(gameName);
        auto gameDef = d->gameMeta[gameName];
        for (auto stringLocation : gameDef.strings)
        {
            downloadJson(d->app, DIALOGUE_BASE_URL + stringLocation,
                         [this, stringLocation, gameName](std::optional<nlohmann::json> result)
                         {
                             if (result.has_value())
                             {
                                 this->d->gameDialogue.insert_or_assign(
                                     gameName, result.value().get<std::map<std::string, Dialogue>>());
                             }
                             else
                             {
                                 std::cout << "APDialogueClient: Failed to download " << stringLocation << "\n";
                             }
                         });
        }
    }
}

void replace(std::string& string, std::string from, std::string to)
{
    for (;;)
    {
        auto start = string.find(from);
        if (start == std::string::npos)
        {
            return;
        }

        string.replace(start, from.length(), to);
    }
}

std::optional<std::string> APDialogueClient::GetCrazyDaveDialogue(std::string gameName, std::string itemName,
                                                                  std::string playerName)
{
    if (!d->gameDialogue.contains(gameName))
    {
        return std::nullopt;
    }

    auto dialogue = d->gameDialogue[gameName];
    if (!dialogue.contains(itemName))
    {
        return std::nullopt;
    }

    auto sourceDialogue = dialogue[itemName].crazyDave;
    replace(sourceDialogue, "{{player}}", playerName);
    replace(sourceDialogue, "{{item}}", itemName);
    replace(sourceDialogue, "{{game}}", gameName);
    return sourceDialogue;
}
