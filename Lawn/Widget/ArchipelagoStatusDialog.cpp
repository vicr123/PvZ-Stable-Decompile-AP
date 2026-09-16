#include "ArchipelagoStatusDialog.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "GameButton.h"
#include "../../LawnApp.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "../../Resources.h"
#include "../../SexyAppFramework/APWrapper.h"
#include "../Board.h"

ArchipelagoStatusDialog::ArchipelagoStatusDialog(LawnApp* theApp) : LawnDialog(
    	theApp,
    	Dialogs::DIALOG_ARCHIPELAGO_STATUS,
    	true,
    	"Archipelago",
    	"Connect to Archipelago by entering\nthe connection details below",
    	theApp->mBoard ? "Main Menu" : "Close",
    	Dialog::BUTTONS_NONE
)
{
    mApp = theApp;
    mVerticalCenterText = false;
    mHostEditWidget = CreateEditWidget(0, this, this);
	mHostEditWidget->mMaxChars = 30;
	mHostEditWidget->DisableAutocap();
	mHostEditWidget->SetFont(FONT_PICO129);
	mSlotEditWidget = CreateEditWidget(1, this, this);
	mSlotEditWidget->mMaxChars = 30;
	mSlotEditWidget->DisableAutocap();
	mSlotEditWidget->SetFont(FONT_PICO129);
	mPasswordEditWidget = CreateEditWidget(2, this, this);
	mPasswordEditWidget->mMaxChars = 30;
	mPasswordEditWidget->DisableAutocap();
	mPasswordEditWidget->SetFont(FONT_PICO129);

    mConnectButton = MakeButton(20, this, "Connect to Archipelago");
    mCloseButton = MakeButton(21, this, theApp->mBoard ? "Main Menu" : "Close");
	
    CalcSize(110, 300);
	
	mConnectionListener = mApp->mAP->AddConnectionCompleteListener([this] { UpdateArchipelagoStatus(); });
	mDisconnectionListener = mApp->mAP->AddDisconnectionListener([this] { UpdateArchipelagoStatus(); });
	
	std::ifstream ap_connection_file("ap-connection.json");
    auto connection = nlohmann::json::parse(ap_connection_file, nullptr, false);
	if (!connection.is_discarded())
	{
		mHostEditWidget->mString = connection["host"];
		mSlotEditWidget->mString = connection["slot"];
		mPasswordEditWidget->mString = connection["password"];
	}
	
	UpdateArchipelagoStatus();
}

ArchipelagoStatusDialog::~ArchipelagoStatusDialog()
{
	delete mConnectionListener;
	delete mDisconnectionListener;
	
	delete mPasswordEditWidget;
	delete mSlotEditWidget;
	delete mHostEditWidget;
	
	delete mConnectButton;
	delete mCloseButton;
}

void ArchipelagoStatusDialog::AddedToManager(WidgetManager* theWidgetManager)
{
	LawnDialog::AddedToManager(theWidgetManager);
	AddWidget(mHostEditWidget);
	AddWidget(mSlotEditWidget);
	AddWidget(mPasswordEditWidget);
	AddWidget(mConnectButton);
	AddWidget(mCloseButton);
	theWidgetManager->SetFocus(mHostEditWidget);
}

void ArchipelagoStatusDialog::RemovedFromManager(WidgetManager* theWidgetManager)
{
	LawnDialog::RemovedFromManager(theWidgetManager);
	RemoveWidget(mHostEditWidget);
	RemoveWidget(mSlotEditWidget);
	RemoveWidget(mPasswordEditWidget);
	RemoveWidget(mConnectButton);
	RemoveWidget(mCloseButton);
}

int ArchipelagoStatusDialog::GetPreferredHeight(int theWidth)
{
	return LawnDialog::GetPreferredHeight(theWidth) + 40;
}

void ArchipelagoStatusDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
	LawnDialog::Resize(theX, theY, theWidth, theHeight);
	mHostEditWidget->Resize(mContentInsets.mLeft + 12, 220, mWidth - mContentInsets.mLeft - mContentInsets.mRight - 24, 28);
	mSlotEditWidget->Resize(mContentInsets.mLeft + 12, mHostEditWidget->Bottom() + 35, mWidth - mContentInsets.mLeft - mContentInsets.mRight - 24, 28);
	mPasswordEditWidget->Resize(mContentInsets.mLeft + 12, mSlotEditWidget->Bottom() + 35, mWidth - mContentInsets.mLeft - mContentInsets.mRight - 24, 28);
	mConnectButton->Resize(this->Width() / 2 - 200, mPasswordEditWidget->Bottom() + 30, 400, 46);
	mCloseButton->Resize(this->Width() / 2 - 200, this->Height() - 46 - 40, 400, 46);
}

void ArchipelagoStatusDialog::Draw(Graphics* g)
{
	LawnDialog::Draw(g);
	DrawEditBox(g, mHostEditWidget);
	DrawEditBox(g, mSlotEditWidget);
	DrawEditBox(g, mPasswordEditWidget);
	
	Sexy::Color aTextColor(107, 109, 145);
	TodDrawString(g, "Archipelago Host and Port", mContentInsets.mLeft + 12, mHostEditWidget->Top() - 15, FONT_DWARVENTODCRAFT15, aTextColor, DrawStringJustification::DS_ALIGN_LEFT);
	TodDrawString(g, "Archipelago Slot Name", mContentInsets.mLeft + 12, mSlotEditWidget->Top() - 15, FONT_DWARVENTODCRAFT15, aTextColor, DrawStringJustification::DS_ALIGN_LEFT);
	TodDrawString(g, "Archipelago Password (optional)", mContentInsets.mLeft + 12, mPasswordEditWidget->Top() - 15, FONT_DWARVENTODCRAFT15, aTextColor, DrawStringJustification::DS_ALIGN_LEFT);
	
	if (mHostEditWidget->mVisible)
	{
		if (mHostEditWidget->mString.empty())
		{
			Sexy::Color aTextColor(150, 150, 150);
			TodDrawString(g, "archipelago.gg:12345", mContentInsets.mLeft + 16, mHostEditWidget->Top() + 18, FONT_PICO129, aTextColor, DrawStringJustification::DS_ALIGN_LEFT);
		}
	}
	else
	{
		Sexy::Color aTextColor(255, 255, 255);
		TodDrawString(g, mApp->mAP->ServerName(), mContentInsets.mLeft + 16, mHostEditWidget->Top() + 18, FONT_PICO129, aTextColor, DrawStringJustification::DS_ALIGN_LEFT);
		TodDrawString(g, mApp->mAP->SlotName(), mContentInsets.mLeft + 16, mHostEditWidget->Top() + 18, FONT_PICO129, aTextColor, DrawStringJustification::DS_ALIGN_LEFT);
		TodDrawString(g, mApp->mAP->Password(), mContentInsets.mLeft + 16, mHostEditWidget->Top() + 18, FONT_PICO129, aTextColor, DrawStringJustification::DS_ALIGN_LEFT);
	}
}

void ArchipelagoStatusDialog::EditWidgetText(int theId, const SexyString& theString)
{
	if (mApp->mAP->ConnectionStatus() == APWrapper::ConnectionStatus::Disconnected)
	{
		// Connect to AP
		this->ButtonDepress(20);
	}
	else
	{
		// Close the dialog box
		mApp->ButtonDepress(mId + 2000);
	}
}

bool ArchipelagoStatusDialog::AllowChar(int, SexyChar theChar)
{
	return true;
}

void ArchipelagoStatusDialog::ButtonDepress(int theId)
{
	LawnDialog::ButtonDepress(theId);
	switch (theId)
	{
	case 21:
		if (mApp->mBoard)
		{
			if (mApp->mBoard->NeedSaveGame())
			{
				mApp->DoConfirmBackToMain();
			} else
			{
				mApp->DoBackToMain();
			}
		}
		else
		{
			mApp->KillDialog(DIALOG_ARCHIPELAGO_STATUS);
		}
		break;
	case 20:
		// Connect button
		if (mApp->mAP->ConnectionStatus() == APWrapper::ConnectionStatus::Disconnected)
		{
			// Clear the current profile
			if (!mApp->mBoard)
			{
				mApp->mPlayerInfo = nullptr;
			}
			
			// Save the connection information
			nlohmann::json connection = {
				{"host", mHostEditWidget->mString},
				{"slot", mSlotEditWidget->mString},
				{"password", mPasswordEditWidget->mString}
			};
			std::ofstream ap_connection_file("ap-connection.json");
			ap_connection_file << connection;
			
			mApp->mAP->Connect(mHostEditWidget->mString, mSlotEditWidget->mString, mPasswordEditWidget->mString);
			mApp->ShowAPConnectingDialog();
		}
		else
		{
			mApp->mAP->DisconnectNow();
			UpdateArchipelagoStatus();
		}
		break;
	}
}

void ArchipelagoStatusDialog::UpdateArchipelagoStatus()
{
	if (mApp->mAP->ConnectionStatus() == APWrapper::ConnectionStatus::Disconnected)
	{
		mConnectButton->SetLabel( "Connect to Archipelago");
		mHostEditWidget->SetVisible(true);
		mSlotEditWidget->SetVisible(true);
		mPasswordEditWidget->SetVisible(true);
		
		if (mApp->mBoard)
		{
			mDialogLines = "The connection to Archipelago was severed.\nReconnect to continue.";
		}
		else
		{
			mDialogLines = "Connect to Archipelago by entering\nthe connection details below";
		}
	}
	else
	{
		mConnectButton->SetLabel("Disconnect from Archipelago");
		mHostEditWidget->SetVisible(false);
		mSlotEditWidget->SetVisible(false);
		mPasswordEditWidget->SetVisible(false);
		
		mDialogLines = "Connected to Archipelago";
	}
}

void ArchipelagoStatusDialog::Tab()
{
	if (mWidgetManager->mFocusWidget == mHostEditWidget)
	{
		mWidgetManager->SetFocus(mSlotEditWidget);
	}
	else if (mWidgetManager->mFocusWidget == mSlotEditWidget)
	{
		mWidgetManager->SetFocus(mPasswordEditWidget);
	}
}

void ArchipelagoStatusDialog::BackTab()
{
	if (mWidgetManager->mFocusWidget == mSlotEditWidget)
	{
		mWidgetManager->SetFocus(mHostEditWidget);
	}
	else if (mWidgetManager->mFocusWidget == mPasswordEditWidget)
	{
		mWidgetManager->SetFocus(mSlotEditWidget);
	}
}
