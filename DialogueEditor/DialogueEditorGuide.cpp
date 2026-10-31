#include "DialogueEditorGuide.h"

#include "imgui.h"

ImVec2 minRect;
ImVec2 maxRect;

namespace DialogueEditorComponents
{
    void DialogueEditorGuide(bool* isOpen)
    {
        if (*isOpen)
        {
            ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_Once);
            if (ImGui::Begin("Guide", isOpen, ImGuiWindowFlags_None))
            {
                ImGui::TextWrapped("Welcome to the Dialogue Editor! Here, you can contribute to the dialogue that is shown in Crazy Dave's shop in Plants vs. Zombies.");
                
                ImGui::GetWindowDrawList()->AddRectFilled(minRect, maxRect, IM_COL32(255, 200, 0, 255), 0);

                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
                ImGui::TextWrapped("IMPORTANT - PLEASE READ!");
                minRect = ImGui::GetItemRectMin();
                ImGui::TextWrapped("If you just want to start writing, please make sure you read the Style Guide at the very least. It contains information about expectations and requirements that submitted text is expected to uphold. You can always return to this guide by selecting Help > Show Guide.");
                maxRect = ImGui::GetItemRectMax();
                ImGui::PopStyleColor();
                
                ImGui::TextWrapped("This guide is broken up into two parts:");
                ImGui::Indent();
                ImGui::TextWrapped("- The Style Guide outlines the requirements for the text that you submit");
                ImGui::TextWrapped("- The Dialogue Editor contains information about how to use the Dialogue Editor.");
                ImGui::TextWrapped("- Submitting Dialogue contains instructions on what to do once you have finished.");
                ImGui::Unindent();
                
                ImGui::Separator();
                
                ImGui::Text("S T Y L E  G U I D E");
                ImGui::Text("");
                ImGui::TextWrapped("The required style for the text boils down to a few simple points:");
                ImGui::Indent();
                ImGui::TextWrapped("- KEEP IT G RATED. Plants vs. Zombies is a game that is designed for everyone to play, no matter their age.");
                ImGui::Indent();
                ImGui::TextWrapped(R"(~ Avoid profanity. "Fuck" and "Shit" are examples of unacceptable language.)");
                ImGui::TextWrapped("~ Avoid references to drugs and sex.");
                ImGui::TextWrapped("~ Exception: Games that are inherently for mature-age players have some leniency.");
                ImGui::TextWrapped("~ Exception: Item names can be written verbatim.");
                ImGui::Unindent();
                ImGui::TextWrapped("- SPELLING AND GRAMMAR ARE IMPORTANT. Double check your text to ensure that you have spelled everything correctly, and ensure that the sentences make sense.");
                ImGui::TextWrapped("- TEST YOUR TEXT. Pay attention to the preview as you are writing your dialogue. Avoid obscuring any elements of the background art - this includes buttons that are calculated based on the length of the text.");
                ImGui::TextWrapped("- INSIDE JOKES which any player of the game is expected to understand are permissible - and recommended!");
                ImGui::TextWrapped("- DON'T DECEIVE THE PLAYER. For example:");
                ImGui::Indent();
                ImGui::TextWrapped("~ Don't write dialogue implying that a different item will be sent.");
                ImGui::TextWrapped(R"(~ Exception: Traps designed to trick the player into sending them (for example, "Time Plece" in A Hat in Time) are exempt from this rule.)");
                ImGui::Unindent();
                ImGui::TextWrapped("- USE COMMON SENSE.");
                ImGui::Unindent();
                ImGui::TextWrapped("Additionally, some recommendations which are not strictly necessary, but you should consider following:");
                ImGui::Indent();
                ImGui::TextWrapped("- MAINTAIN THE TONE OF THE CHARACTER. Crazy Dave has an esoteric way of communication, and the submitted text should take this into account and emulate his tone and mannerisms.");
                ImGui::Unindent();
                ImGui::TextWrapped("In the event of a violation of the style guide, you may be contacted to provide an alternative string, or the submitted dialogue may be edited to conform to the style guide.");
                
                ImGui::Separator();
                
                ImGui::Text("D I A L O G U E   E D I T O R");
                ImGui::Text("");
                ImGui::TextWrapped("Dialogue for a game can be defined for each item, and each scene. Each different scenario where dialogue text for an item can appear is called a scene. The available scenes are");
                ImGui::Indent();
                ImGui::TextWrapped("- Crazy Dave");
                ImGui::TextWrapped("- Award Screen");
                ImGui::Unindent();
                ImGui::TextWrapped("To create dialogue for a game that doesn't have any dialogue yet, generate an Archipelago multiworld and connect to it, following the instructions in File > Create File from Archipelago.");
                ImGui::TextWrapped("Once the available items have been discovered, you will be asked to save the file somewhere. Pick a place to save the file, and then continue.");
                ImGui::TextWrapped("You can also open a file from your disk by selecting File > Open Local File.");
                ImGui::Text("");
                ImGui::Text("AVAILABLE ITEMS DIALOG");
                ImGui::TextWrapped("Once you have opened (or created) a file, the Available Items dialog will appear. This is where you can pick an item to provide dialogue for. Each item has a status indicator on the left, indicating for which scenes dialogue text exists.");
                ImGui::TextWrapped("In the event that text does not exist, standard fallback text will be used instead.");
                ImGui::Text("");
                ImGui::Text("TEXT EDITOR DIALOG");
                ImGui::TextWrapped("After selecting an item to edit, the Text Editor dialog will appear. This is where you can edit the dialogue text for the selected item and scene.");
                ImGui::TextWrapped("After entering dialogue text, check the preview and ensure the text renders correctly.");
                ImGui::TextWrapped("Placeholders are available to substitute in specific strings of text. The following placeholders are available:");
                ImGui::Indent();
                ImGui::TextWrapped("- {{player}} will be replaced with the name of the player the item is being sent to.");
                ImGui::TextWrapped("- {{item}} will be replaced with the name of the item that is being sent.");
                ImGui::TextWrapped("- {{game}} will be replaced with the name of the game that the item is being sent to.");
                ImGui::Unindent();
                ImGui::TextWrapped(R"(To use a placeholder, enter its name, surrounded by two braces. It will disappear from the "Remaining Placeholders" list, and an example will be reflected in the preview. You are not required to use every placeholder.)");
                ImGui::TextWrapped("While you are editing dialogue, you can use some keyboard shortcuts to make editing faster:");
                ImGui::Indent();
                ImGui::TextWrapped("- CTRL+ENTER  Go to the next dialogue in sequence.");
                ImGui::TextWrapped("- ALT+LEFT    Go to the previous scene.");
                ImGui::TextWrapped("- ALT+RIGHT   Go to the next scene.");
                ImGui::TextWrapped("- ALT+UP      Go to the previous item.");
                ImGui::TextWrapped("- ALT+DOWN    Go to the next item.");
                ImGui::Unindent();
                
                ImGui::Separator();
                
                ImGui::Text("S U B M I T T I N G   D I A L O G U E");
                ImGui::Text("");
                ImGui::TextWrapped("Once you're ready to submit your dialogue, ping @vicr123 in the #plants-vs-zombies-goty text channel in the Archipelago Discord server, making sure to upload the saved file.");
                ImGui::TextWrapped("Once your dialogue is submitted, you may be contacted for follow-up questions if required.");
                ImGui::TextWrapped("Thank you for your contributions!");
                
                if (ImGui::Button("Close"))
                {
                    *isOpen = false;
                }
            }
            ImGui::End();
        }
    }
}
