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
