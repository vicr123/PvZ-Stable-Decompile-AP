#pragma once
#include "LawnDialog.h"
#include "../../SexyAppFramework/CheckboxListener.h"

class ArchipelagoReconfigDialog : public LawnDialog, public CheckboxListener
{
protected:
    enum
    {
        APOptions_Deathlink,
        APOptions_Ringlink,
        APOptions_Energylink,
        APOptions_Seedlink,
        APOptions_Lawnlink,
        APOptions_HarderZombieSpawns,
        APOptions_DisableStormFlashes,
        APOptions_OpenImitater
    };
    
public:

    ArchipelagoReconfigDialog(LawnApp* theApp);
    ~ArchipelagoReconfigDialog();
    
    LawnApp* mApp;
    Sexy::Checkbox* mDeathlinkCheckbox;
    Sexy::Checkbox* mRinglinkCheckbox;
    Sexy::Checkbox* mEnergylinkCheckbox;
    Sexy::Checkbox* mSeedlinkCheckbox;
    Sexy::Checkbox* mLawnlinkCheckbox;
    Sexy::Checkbox* mHarderZombieSpawnsCheckbox;
    Sexy::Checkbox* mDisableStormFlashesCheckbox;
    Sexy::Checkbox* mOpenImitaterCheckbox;
    
    void AddedToManager(Sexy::WidgetManager* theWidgetManager);
    void RemovedFromManager(Sexy::WidgetManager* theWidgetManager);
    void Draw(Sexy::Graphics* g);
    void ButtonDepress(int theId) override;
    void CheckboxChecked(int theId, bool theChecked) override;
};
