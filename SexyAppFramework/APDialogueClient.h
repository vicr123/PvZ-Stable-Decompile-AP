#pragma once
#include <functional>
#include <optional>
#include <stop_token>
#include <string>
#include <vector>

struct DialogueFile;

namespace Sexy
{
    class SexyAppBase;
}

struct APDialogueClientPrivate;
class APDialogueClient
{
public:
    APDialogueClient(Sexy::SexyAppBase* app);
    ~APDialogueClient();
    
    enum class DialogueType : int
    {
        CrazyDave = 0,
        AwardScreen
    };
    
    void DownloadGameMeta(std::string gameName);
    std::vector<std::string> AvailableGames();
    std::optional<std::string> GetDialogue(DialogueType type, std::string gameName, std::string itemName, std::string playerName, std::string playerYourName, std::string playerYouName);
    void DownloadDialogueFileCallback(std::string gameName, std::function<void(std::optional<DialogueFile>)> callback, std::stop_token stop_token);
    
private:
    APDialogueClientPrivate* d;
};
