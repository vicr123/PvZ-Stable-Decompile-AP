#pragma once
#include "LawnDialog.h"
#include "../../SexyAppFramework/SliderListener.h"
#include "../../SexyAppFramework/CheckboxListener.h"

namespace Sexy
{
    class Slider;
    class Checkbox;
};

class NewNewOptionsDialog : public LawnDialog, public Sexy::SliderListener, public Sexy::CheckboxListener
{
protected:
    
    enum
    {
        NewOptionsDialog_MusicVolume,
        NewOptionsDialog_SoundVolume,
        NewOptionsDialog_Fullscreen,
        NewOptionsDialog_HardwareAcceleration,
        NewOptionsDialog_VideoGraphics,
        NewOptionsDialog_PauseOnLostFocus,
    };
    
public:
    NewNewOptionsDialog(LawnApp* theApp);
    ~NewNewOptionsDialog();
    
    LawnApp*				mApp;								//+0x158
    Sexy::Slider*			mMusicVolumeSlider;					//+0x15C
    Sexy::Slider*			mSfxVolumeSlider;					//+0x160
    Sexy::Checkbox*			mFullscreenCheckbox;				//+0x164
    Sexy::Checkbox*			mHardwareAccelerationCheckbox;		//+0x168
    Sexy::Checkbox*			mPauseOnLostFocusCheckbox;	 		//+0x168
    
    int						GetPreferredHeight(int theWidth);
    void					AddedToManager(Sexy::WidgetManager* theWidgetManager);
    void					RemovedFromManager(Sexy::WidgetManager* theWidgetManager);
    void					Resize(int theX, int theY, int theWidth, int theHeight);
    void					Draw(Sexy::Graphics* g);
    void					SliderVal(int theId, double theVal);
    void					CheckboxChecked(int theId, bool checked);
    void					ButtonPress(int theId);
    void					KeyDown(Sexy::KeyCode theKey);

};
