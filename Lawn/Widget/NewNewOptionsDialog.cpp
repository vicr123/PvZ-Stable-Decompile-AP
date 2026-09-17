#include "NewNewOptionsDialog.h"
#include "../../Resources.h"

#include "../../SexyAppFramework/Slider.h"
#include "../../SexyAppFramework/Checkbox.h"
#include "../../LawnApp.h"
#include "../../Sexy.TodLib/TodFoley.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../Board.h"

NewNewOptionsDialog::NewNewOptionsDialog(LawnApp* theApp) : LawnDialog(theApp, Dialogs::DIALOG_NEWNEWOPTIONS, true, "Game Options", _S(""), _S("Close"), Dialog::BUTTONS_FOOTER)
{
    this->mApp = theApp;
    mMusicVolumeSlider = new Slider(IMAGE_OPTIONS_SLIDERSLOT, IMAGE_OPTIONS_SLIDERKNOB2, NewOptionsDialog_MusicVolume, this);
    double aMusicVolume = theApp->GetMusicVolume();
    aMusicVolume = max(0.0, min(1.0, aMusicVolume));
    mMusicVolumeSlider->SetValue(aMusicVolume);

    mSfxVolumeSlider = new Slider(IMAGE_OPTIONS_SLIDERSLOT, IMAGE_OPTIONS_SLIDERKNOB2, NewOptionsDialog_SoundVolume, this);
    mSfxVolumeSlider->SetValue(theApp->GetSfxVolume() / 0.65);

    mFullscreenCheckbox = MakeNewCheckbox(NewOptionsDialog_Fullscreen, this, !theApp->mIsWindowed);
    mHardwareAccelerationCheckbox = MakeNewCheckbox(NewOptionsDialog_HardwareAcceleration, this, theApp->mEnableVsync);
    mPauseOnLostFocusCheckbox = MakeNewCheckbox(NewOptionsDialog_PauseOnLostFocus, this, theApp->mMuteOnLostFocus);
    
    mMusicVolumeSlider->Resize(199, 116, 135, 40);
    mSfxVolumeSlider->Resize(199, 143, 135, 40);
    mHardwareAccelerationCheckbox->Resize(283, 175, 46, 45);
    mFullscreenCheckbox->Resize(284, 206, 46, 45);
    mPauseOnLostFocusCheckbox->Resize(284, 237, 46, 45);
    
    CalcSize(110, 180);
}

NewNewOptionsDialog::~NewNewOptionsDialog()
{
    delete mMusicVolumeSlider;
    delete mSfxVolumeSlider;
    delete mFullscreenCheckbox;
    delete mHardwareAccelerationCheckbox;
    delete mPauseOnLostFocusCheckbox;
}

//0x45C880
int NewNewOptionsDialog::GetPreferredHeight(int theWidth)
{
    return 300;
}

//0x45C890
void NewNewOptionsDialog::AddedToManager(Sexy::WidgetManager* theWidgetManager)
{
    LawnDialog::AddedToManager(theWidgetManager);
    AddWidget(mMusicVolumeSlider);
    AddWidget(mSfxVolumeSlider);
    AddWidget(mHardwareAccelerationCheckbox);
    AddWidget(mFullscreenCheckbox);
    AddWidget(mPauseOnLostFocusCheckbox);
}

//0x45C930
void NewNewOptionsDialog::RemovedFromManager(Sexy::WidgetManager* theWidgetManager)
{
    LawnDialog::RemovedFromManager(theWidgetManager);
    RemoveWidget(mMusicVolumeSlider);
    RemoveWidget(mSfxVolumeSlider);
    RemoveWidget(mHardwareAccelerationCheckbox);
    RemoveWidget(mFullscreenCheckbox);
    RemoveWidget(mPauseOnLostFocusCheckbox);
}

//0x45C9D0
void NewNewOptionsDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
    LawnDialog::Resize(theX, theY, theWidth, theHeight);
}

//0x45CB50
void NewNewOptionsDialog::Draw(Sexy::Graphics* g)
{
	LawnDialog::Draw(g);
    
    int aMusicOffset = 0;
    int aSfxOffset = 0;
    int a3DAccelOffset = 0;
    int aFullScreenOffset = 0;
    int aPauseWhenLostFocusOffset = 0;
    Sexy::Color aTextColor(107, 109, 145);
    {
        TodDrawString(g, TodStringTranslate(_S("[OPTIONS_MUSIC_LABEL]")), 186, 140 + aMusicOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, TodStringTranslate(_S("[OPTIONS_SOUNDFX]")), 186, 167 + aSfxOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, TodStringTranslate(_S("[OPTIONS_3D_ACCELERATION]")), 274, 197 + a3DAccelOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, TodStringTranslate(_S("[OPTIONS_FULL_SCREEN]")), 274, 229 + aFullScreenOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, TodStringTranslate(_S("Auto Pause")), 274, 261 + aPauseWhenLostFocusOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    }
}

//0x45CF50
void NewNewOptionsDialog::SliderVal(int theId, double theVal)
{
    switch (theId)
    {
    case NewOptionsDialog_MusicVolume:
        mApp->SetMusicVolume(theVal);
        mApp->mSoundSystem->RehookupSoundWithMusicVolume();
        /*if (!mMusicVolumeSlider->mDragging)
        {
            mApp->PlaySample(SOUND_BUTTONCLICK);
        }*/
        break;

    case NewOptionsDialog_SoundVolume:
        mApp->SetSfxVolume(theVal * 0.65);
        mApp->mSoundSystem->RehookupSoundWithMusicVolume();
        if (!mSfxVolumeSlider->mDragging)
        {
            mApp->PlaySample(SOUND_BUTTONCLICK);
        }
        break;
    }
}

//0x45CFF0
void NewNewOptionsDialog::CheckboxChecked(int theId, bool checked)
{
    mApp->PlaySample(SOUND_BUTTONCLICK);
}

//0x45D290
void NewNewOptionsDialog::KeyDown(Sexy::KeyCode theKey)
{
    if (mApp->mBoard)
    {
        mApp->mBoard->DoTypingCheck(theKey);
    }

    if (theKey == KeyCode::KEYCODE_SPACE || theKey == KeyCode::KEYCODE_RETURN)
    {
        Dialog::ButtonDepress(Dialog::ID_OK);
    }
    else if (theKey == KeyCode::KEYCODE_ESCAPE)
    {
        Dialog::ButtonDepress(/*Dialog::ID_CANCEL*/Dialog::ID_OK);
    }
}

//0x45D2F0
void NewNewOptionsDialog::ButtonPress(int theId)
{
    mApp->PlaySample(SOUND_GRAVEBUTTON);
    if (theId == 21)
    {
        mApp->KillDialog(DIALOG_NEWNEWOPTIONS);
    }
}
