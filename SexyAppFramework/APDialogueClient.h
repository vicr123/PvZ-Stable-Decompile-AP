#pragma once
#include <optional>
#include <string>

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
    
    void DownloadGameMeta(std::string gameName);
    std::optional<std::string> GetCrazyDaveDialogue(std::string gameName, std::string itemName, std::string playerName);
    
private:
    APDialogueClientPrivate* d;
};
