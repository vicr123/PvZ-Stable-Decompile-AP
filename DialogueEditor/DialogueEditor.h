#pragma once

#include "../../SexyAppFramework/Widget.h"

class LawnApp;
struct DialogueEditorPrivate;
class DialogueEditor : public Sexy::Widget
{
public:
    DialogueEditor(LawnApp* theApp);
    ~DialogueEditor();
    
    void ImGuiDraw();
    void MenuBar();
    void Draw(Sexy::Graphics* g) override;
    void Update() override;
    
    void GoUp();
    void GoDown();
    void GoLeft();
    void GoRight();
    
    void TriggerOpenAction();

private:
    DialogueEditorPrivate* d;
};
