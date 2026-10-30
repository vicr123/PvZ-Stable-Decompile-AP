#pragma once

struct GameMeta
{
    std::vector<std::string> strings;
    bool matureGame;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameMeta, strings, matureGame)

struct Dialogue
{
    std::string crazyDave;
    std::string awardScreen;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Dialogue, crazyDave, awardScreen)
};

struct DialogueFile
{
    std::string gameName;
    std::map<std::string, Dialogue> dialogue;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DialogueFile, gameName, dialogue)