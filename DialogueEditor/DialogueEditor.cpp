#define APCLIENT_DEBUG
#define _WEBSOCKETPP_CPP11_STL_

#define _WIN32_WINNT 0x0600

#include <apclient.hpp>

#include "DialogueEditor.h"

#include <fstream>
#include <imgui.h>
#include <tinyfiledialogs.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <nlohmann/json.hpp>

#include "DialogueEditorGuide.h"
#include "../LawnApp.h"
#include "../Lawn/Widget/StoreScreen.h"
#include "../SexyAppFramework/APDialogueClientInternalTypes.h"
#include "../SexyAppFramework/APWrapper.h"


struct DialogueEditorPrivate
{
    LawnApp* mApp;
    StoreScreen* storeScreen;
    std::string current_file;

    DialogueFile mDialogueFile;
    std::vector<string> availableDialogue;
    std::string selected_item;
    
    std::string apHostPort;
    std::string apSlot;
    std::string apPassword;
    std::string apError;
    APClient* mAP = nullptr;
    bool deleteAP = false;

    bool showBadFilePopup = false;
    bool showArchipelagoPopup = false;
    bool showGuide = true;
    
    void populateAvailableDialogue()
    {
        availableDialogue.clear();
        for (auto dialogue : mDialogueFile.dialogue)
        {
            availableDialogue.push_back(dialogue.first);
        }
    }

    bool loadJson(nlohmann::json& json)
    {
        try
        {
            mDialogueFile = json.get<DialogueFile>();
            populateAvailableDialogue();
            return true;
        }
        catch (nlohmann::json::parse_error& ex)
        {
            return false;
        }
    }

    void saveJson()
    {
        std::ofstream f(current_file);
        nlohmann::json json(mDialogueFile);
        f << std::setw(4) << json;
    }
};

void ImguiEditText(std::string* source, const char* label)
{
    static char text_buffer[128];
    strncpy(text_buffer, source->c_str(), 128);
            
    if (ImGui::InputText(label, text_buffer, 128))
    {
        *source = text_buffer;
    }
}

DialogueEditor::DialogueEditor(LawnApp* theApp)
{
    d = new DialogueEditorPrivate();
    d->mApp = theApp;

    TodLoadResources("DelayLoad_Zombatar");

    d->storeScreen = new StoreScreen(theApp);
    d->storeScreen->mInert = true;
}

DialogueEditor::~DialogueEditor()
{
    delete d;
}

void DialogueEditor::ImGuiDraw()
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    MenuBar();

    if (!d->availableDialogue.empty())
    {
        ImGui::Begin("Available Items", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        
        ImGui::Text(d->mDialogueFile.gameName.c_str());
        
        ImGui::PushItemWidth(600);

        int selected_item_index = std::distance(d->availableDialogue.begin(),
                                                std::ranges::find(d->availableDialogue, d->selected_item));
        if (selected_item_index >= d->availableDialogue.size()) selected_item_index = -1;
        
        std::vector<std::string> listItemText;
        for (auto itemName : d->availableDialogue)
        {
            std::string status = "[X] ";
            if (d->mDialogueFile.dialogue[itemName].crazyDave.empty())
            {
                status = "[ ] ";
            }
            listItemText.push_back(status + itemName);
        }
        
        ImGui::ListBox("Available Items##List", &selected_item_index, [](void* data, int idx, const char** out_text)
        {
            auto vec = static_cast<std::vector<std::string>*>(data);
            *out_text = vec->at(idx).c_str();
            return true;
        }, &listItemText, d->availableDialogue.size(), 10);
        if (selected_item_index >= 0)
        {
            d->selected_item = d->availableDialogue[selected_item_index];
        }
        
        if (d->mDialogueFile.dialogue.contains(d->selected_item))
        {
            auto item = &d->mDialogueFile.dialogue[d->selected_item];
            static char text_buffer[128];
            strncpy(text_buffer, item->crazyDave.c_str(), 128);
            
            if (ImGui::InputText("Crazy Dave Text", text_buffer, 128))
            {
                item->crazyDave = text_buffer;
                d->saveJson();
            }
        }
        
        ImGui::PopItemWidth();
        ImGui::End();
    }

    if (d->showBadFilePopup)
    {
        ImGui::OpenPopup("Bad File");
    }

    if (ImGui::BeginPopupModal("Bad File", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Unable to open the file. It may be in the wrong format or corrupt.");
        if (ImGui::Button("OK"))
        {
            d->showBadFilePopup = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    
    if (d->showArchipelagoPopup)
    {
        ImGui::OpenPopup("Archipelago");
    }
    
    if (ImGui::BeginPopupModal("Archipelago", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (d->mAP)
        {
            ImGui::Text("One moment please...");
            if (ImGui::Button("Cancel"))
            {
                d->deleteAP = true;
            }
        }
        else
        {
            ImGui::TextWrapped("Please create a multiworld with the game that you want to write dialogue for, and then enter the connection details for that slot below.");
            ImGui::TextWrapped("The game options are not important - all items that the game can possibly send will be imported for editing.");
            ImguiEditText(&d->apHostPort, "Archipelago Host & Port");
            ImguiEditText(&d->apSlot, "Archipelago Slot");
            ImguiEditText(&d->apPassword, "Archipelago Password");
            if (!d->apError.empty())
            {
                ImGui::TextWrapped(d->apError.c_str());
            }
            if (ImGui::Button("Cancel"))
            {
                d->showArchipelagoPopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Download Game Information and Save")) {
                d->apError.clear();
                d->mAP = new APClient(GetAPUuid(), "", d->apHostPort);
                d->mAP->set_socket_connected_handler([this]
                {
                    d->mAP->ConnectSlot(d->apSlot, d->apPassword, 0, {"TextOnly"});
                });
                d->mAP->set_slot_connected_handler([this](const nlohmann::json&)
                {
                    d->mAP->GetDataPackage({d->mAP->get_player_game(d->mAP->get_player_number())});
                });
                d->mAP->set_slot_refused_handler([this](std::list<std::string> errors)
                {
                    if (std::ranges::find(errors, "InvalidSlot") != std::end(errors))
                    {
                        d->apError = "Check the slot name and try again.";
                    }
                    else
                    {
                        d->apError = "Unable to connect to Archipelago. Check your parameters and try again.";
                    }
                    d->deleteAP = true;
                });
                d->mAP->set_data_package_changed_handler([this](const nlohmann::json& json)
                {
                    auto gameName = d->mAP->get_player_game(d->mAP->get_player_number());
                    
                    const char* filterPatterns[] = {"*.json"};
                    const char* file_path = tinyfd_saveFileDialog(
                        "Save Dialogue File",
                        (gameName + ".json").c_str(),
                        1,
                        filterPatterns,
                        "JSON file"
                    );

                    if (file_path)
                    {
                        d->mDialogueFile = DialogueFile();
                        d->mDialogueFile.gameName = gameName;
                        
                        auto gameDataPackage = json["games"][gameName];
                        auto itemNameToId = gameDataPackage["item_name_to_id"];
                        
                        for (auto& [key, value] : itemNameToId.items())
                        {
                            d->mDialogueFile.dialogue.insert_or_assign(key, Dialogue());
                        }
                        d->current_file = file_path;
                        d->saveJson();
                        d->populateAvailableDialogue();
                        
                        d->showArchipelagoPopup = false;
                        ImGui::CloseCurrentPopup();
                    }
                    
                    d->deleteAP = true;
                });
            }
        }
        ImGui::EndPopup();
    }
    
    DialogueEditorComponents::DialogueEditorGuide(&d->showGuide);
    
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Enter) || ImGui::IsKeyChordPressed(ImGuiMod_Alt | ImGuiKey_DownArrow))
    {
        int selected_item_index = std::distance(d->availableDialogue.begin(),
                                                std::ranges::find(d->availableDialogue, d->selected_item));
        if (selected_item_index >= d->availableDialogue.size()) selected_item_index = -1;
        selected_item_index++;
        if (selected_item_index < d->availableDialogue.size())
        {
            d->selected_item = d->availableDialogue[selected_item_index];
        }
    }
    else if (ImGui::IsKeyChordPressed(ImGuiMod_Alt | ImGuiKey_UpArrow))
    {
        int selected_item_index = std::distance(d->availableDialogue.begin(),
                                                std::ranges::find(d->availableDialogue, d->selected_item));
        selected_item_index--;
        if (selected_item_index < d->availableDialogue.size())
        {
            d->selected_item = d->availableDialogue[selected_item_index];
        }
    }
    else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_O))
    {
        TriggerOpenAction();
    }

    ImGui::Render();
}

void DialogueEditor::MenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Create File from Archipelago"))
            {
                d->showArchipelagoPopup = true;
            }
            if (ImGui::MenuItem("Open Local File", "Ctrl+O"))
            {
                TriggerOpenAction();
            }
            // if (ImGui::MenuItem("Open Network File"))
            // {
            // }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit"))
            {
                SendMessage(d->mApp->mHWnd, WM_CLOSE, NULL, NULL);
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Show Guide", nullptr, d->showGuide))
            {
                d->showGuide = !d->showGuide;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void DialogueEditor::Draw(Sexy::Graphics* g)
{
    Widget::Draw(g);
    d->storeScreen->Draw(g);
}

void DialogueEditor::Update()
{
    Widget::Update();
    
    if (d->deleteAP)
    {
        delete d->mAP;
        d->mAP = nullptr;
        d->deleteAP = false;
    }
    if (d->mAP)
    {
        d->mAP->poll();
    }
    
    d->storeScreen->Update();

    if (d->mApp->mCrazyDaveState != CrazyDaveState::CRAZY_DAVE_ENTERING && d->mApp->mCrazyDaveState !=
        CrazyDaveState::CRAZY_DAVE_OFF)
    {
        if (d->mDialogueFile.dialogue.contains(d->selected_item))
        {
            auto item = d->mDialogueFile.dialogue[d->selected_item];
            d->storeScreen->SetBubbleText(item.crazyDave + "{NO_CLICK}", 1, false);
        }
        else
        {
            d->storeScreen->SetBubbleText("Welcome to the Dialogue Editor!{NO_CLICK}", 1, false);
        }
    }

    this->MarkDirty();
}

void DialogueEditor::TriggerOpenAction()
{
    const char* filterPatterns[] = {"*.json"};

    const char* file_path = tinyfd_openFileDialog(
        "Open Dialogue File",
        NULL,
        1,
        filterPatterns,
        "JSON file",
        0
    );

    if (file_path)
    {
        std::ifstream f(file_path);
        auto json = nlohmann::json::parse(f, nullptr, false);
        if (json.is_discarded())
        {
            d->showBadFilePopup = true;
        }
        else
        {
            if (d->loadJson(json))
            {
                d->current_file = file_path;
            }
            else
            {
                d->showBadFilePopup = true;
            }
        }
    }
}
