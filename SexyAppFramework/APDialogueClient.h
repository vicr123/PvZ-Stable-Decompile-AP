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
    
    enum class DialogueType : int
    {
        CrazyDave = 0,
        AwardScreen
    };
    
    void DownloadGameMeta(std::string gameName);
    std::optional<std::string> GetDialogue(DialogueType type, std::string gameName, std::string itemName, std::string playerName, std::string playerYourName, std::string playerYouName);
    
private:
    APDialogueClientPrivate* d;
};
