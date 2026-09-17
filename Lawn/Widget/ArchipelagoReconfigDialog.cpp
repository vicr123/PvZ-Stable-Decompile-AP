#include "ArchipelagoReconfigDialog.h"

#include "../../LawnApp.h"
#include "../../SexyAppFramework/Checkbox.h"
#include "../../Resources.h"

ArchipelagoReconfigDialog::ArchipelagoReconfigDialog(LawnApp* theApp) : LawnDialog(
    theApp, Dialogs::DIALOG_APOPTIONS, true, "Archipelago Reconfiguration",
    _S("If you regret setting any of these YAML options, you can reconfigure them here."), _S("Close"),
    Dialog::BUTTONS_FOOTER)
{
    this->mApp = theApp;
    this->mVerticalCenterText = false;
    
    auto x = 180;
    auto y = 206;
    mDeathlinkCheckbox = MakeNewCheckbox(APOptions_Deathlink, this, false);
    mDeathlinkCheckbox->Resize(x, y, 46, 45);
    y += 45;
    mRinglinkCheckbox = MakeNewCheckbox(APOptions_Ringlink, this, mApp->mSlotData->ringlink_enabled());
    mRinglinkCheckbox->Resize(x, y, 46, 45);
    y += 45;
    mEnergylinkCheckbox = MakeNewCheckbox(APOptions_Energylink, this, mApp->mSlotData->energylink_enabled());
    mEnergylinkCheckbox->Resize(x, y, 46, 45);
    y += 45;
    mSeedlinkCheckbox = MakeNewCheckbox(APOptions_Seedlink, this, mApp->mSlotData->seedlink_enabled());
    mSeedlinkCheckbox->Resize(x, y, 46, 45);
    y += 45;
    mLawnlinkCheckbox = MakeNewCheckbox(APOptions_Lawnlink, this, mApp->mSlotData->lawnlink_enabled());
    mLawnlinkCheckbox->Resize(x, y, 46, 45);
    x = 490;
    y = 206;
    mHarderZombieSpawnsCheckbox = MakeNewCheckbox(APOptions_HarderZombieSpawns, this, mApp->mSlotData->harder_zombie_spawns());
    mHarderZombieSpawnsCheckbox->Resize(x, y, 46, 45);
    y += 45;
    mDisableStormFlashesCheckbox = MakeNewCheckbox(APOptions_DisableStormFlashes, this, mApp->mSlotData->disable_storm_flashes());
    mDisableStormFlashesCheckbox->Resize(x, y, 46, 45);
    y += 45;
    mOpenImitaterCheckbox = MakeNewCheckbox(APOptions_OpenImitater, this, mApp->mSlotData->imitater_open());
    mOpenImitaterCheckbox->Resize(x, y, 46, 45);
    
    CalcSize(0, 300);
}

ArchipelagoReconfigDialog::~ArchipelagoReconfigDialog()
{
    delete mDeathlinkCheckbox;
}

void ArchipelagoReconfigDialog::AddedToManager(Sexy::WidgetManager* theWidgetManager)
{
    LawnDialog::AddedToManager(theWidgetManager);
    AddWidget(mDeathlinkCheckbox);
    AddWidget(mRinglinkCheckbox);
    AddWidget(mEnergylinkCheckbox);
    AddWidget(mSeedlinkCheckbox);
    AddWidget(mLawnlinkCheckbox);
    AddWidget(mHarderZombieSpawnsCheckbox);
    AddWidget(mDisableStormFlashesCheckbox);
    AddWidget(mOpenImitaterCheckbox);
}

void ArchipelagoReconfigDialog::RemovedFromManager(Sexy::WidgetManager* theWidgetManager)
{
    LawnDialog::RemovedFromManager(theWidgetManager);
    RemoveWidget(mDeathlinkCheckbox);
    RemoveWidget(mRinglinkCheckbox);
    RemoveWidget(mEnergylinkCheckbox);
    RemoveWidget(mSeedlinkCheckbox);
    RemoveWidget(mLawnlinkCheckbox);
    RemoveWidget(mHarderZombieSpawnsCheckbox);
    RemoveWidget(mDisableStormFlashesCheckbox);
    RemoveWidget(mOpenImitaterCheckbox);
}

void ArchipelagoReconfigDialog::Draw(Sexy::Graphics* g)
{
    LawnDialog::Draw(g);
    
    auto x = 170;
    auto y = 230;
    Sexy::Color aTextColor(107, 109, 145);
    TodDrawString(g, "Death Link", x, y, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    y += 45;
    TodDrawString(g, "Ring Link", x, y, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    y += 45;
    TodDrawString(g, "Energy Link", x, y, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    y += 45;
    TodDrawString(g, "Seed Link", x, y, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    y += 45;
    TodDrawString(g, "Lawn Link", x, y, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    x = 480;
    y = 230;
    TodDrawString(g, "Harder Zombie Spawns", x, y, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    y += 45;
    TodDrawString(g, "Disable Storm Flashes", x, y, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    y += 45;
    TodDrawString(g, "Open Imitater", x, y, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);

}

void ArchipelagoReconfigDialog::ButtonDepress(int theId)
{
    LawnDialog::ButtonDepress(theId);
    if (theId == 21)
    {
        mApp->KillDialog(DIALOG_APOPTIONS);
    }
}

void ArchipelagoReconfigDialog::CheckboxChecked(int theId, bool theChecked)
{
    mApp->PlaySample(SOUND_BUTTONCLICK);
}