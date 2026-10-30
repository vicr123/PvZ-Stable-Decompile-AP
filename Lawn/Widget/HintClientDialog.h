#pragma once
#include "LawnDialog.h"

class ToolTipWidget;
class ListenerHandle;
class ScrollbarWidget;
class GameButton;

struct HintClientDialogPrivate;
class HintClientDialog : public LawnDialog
{
private:
    enum
    {
        ALMANAC_BUTTON_CLOSE = 0,
        ALMANAC_BUTTON_PLANT = 1,
        ALMANAC_BUTTON_ZOMBIE = 2,
        ALMANAC_BUTTON_INDEX = 3,
        ALMANAC_BUTTON_COPY = 4
    };
    
    HintClientDialogPrivate* d;

public:
    LawnApp*					mApp;
    GameButton*					mCloseButton;
	GameButton*					mNewHintButton;
    GameButton*                 mCopyButton;
	ScrollbarWidget*			mScrollbar;
	float						mScrollAmount;
    ListenerHandle* mAPDataStorageValueChangedListener;
    ToolTipWidget* mToolTip;
    
    
    HintClientDialog(LawnApp* theApp);
    virtual ~HintClientDialog();

    void Update() override;
    void AddedToManager(WidgetManager* theWidgetManager) override;
    void RemovedFromManager(WidgetManager* theWidgetManager) override;
    void Draw(Graphics* g) override;
    void MouseDown(int x, int y, int theClickCount) override;
    void MouseMove(int x, int y) override;
    void MouseUp(int x, int y, int theClickCount) override;
    void MouseWheel(int theDelta) override;
    
    void UpdateHints();
};
