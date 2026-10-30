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

private:
    DialogueEditorPrivate* d;
};
