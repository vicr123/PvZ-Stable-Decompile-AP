#include "NewHintDialog.h"

#include "GameButton.h"
#include "../Admonition.h"
#include "../../LawnApp.h"
#include "../../Resources.h"
#include "../../SexyAppFramework/APWrapper.h"
#include "../../SexyAppFramework/APWrapper.h"
#include "../../SexyAppFramework/WidgetManager.h"

//0x45D610
NewHintDialog::NewHintDialog(LawnApp* theApp) : LawnDialog(
	theApp, 
	Dialogs::DIALOG_REQUEST_NEW_HINT, 
	false, 
	"Request New Hint",
	_S("What do you want to hint for?"),
	"", 
	Dialog::BUTTONS_YES_NO)
{
	mApp = theApp;
	mVerticalCenterText = false;
	mHintEditWidget = CreateEditWidget(0, this, this);
	mHintEditWidget->DisableAutocap();
	mHintEditWidget->SetFont(FONT_PICO129);
	
	mLawnYesButton->SetLabel("Request Hint");
	mLawnNoButton->SetLabel("Cancel");
	
	CalcSize(110, 90);
	
	mAdmonition = nullptr;
	mProcessing = false;
	
	mUserCommandListener = mApp->mAP->AddUserCommandChatMessageListener([this](const std::string& message)
	{
		if (!mProcessing)
		{
			return;
		}
		
		if (mAdmonition)
		{
			delete mAdmonition;
		}
			
		mAdmonition = new Admonition();
		mAdmonition->SetArea(mHintEditWidget->GetRect());
		mAdmonition->SetText(message);
			
		mProcessing = false;
	});
	mDataStorageListener = mApp->mAP->AddDataStorageValueChangeListener([this](const std::string& name, const nlohmann::json& value)
	{
		if (!mProcessing)
		{
			return;
		}
		
		if (name == mApp->mAP->DataStorageSlot(APWrapper::KnownDataStorageKey::Hints))
		{
			mProcessing = false;
			mApp->KillRequestNewHintDialog();
		}
	});
}

//0x45D7E0��0x45D800
NewHintDialog::~NewHintDialog()
{
	if (mAdmonition)
	{
		delete mAdmonition;
	}
	delete mUserCommandListener;
	delete mDataStorageListener;
	delete mHintEditWidget;
}

//0x45D870
void NewHintDialog::AddedToManager(WidgetManager* theWidgetManager)
{
	LawnDialog::AddedToManager(theWidgetManager);
	AddWidget(mHintEditWidget);
	theWidgetManager->SetFocus(mHintEditWidget);
}

//0x45D8E0
void NewHintDialog::RemovedFromManager(WidgetManager* theWidgetManager)
{
	LawnDialog::RemovedFromManager(theWidgetManager);
	RemoveWidget(mHintEditWidget);
}

int NewHintDialog::GetPreferredHeight(int theWidth)
{
	return LawnDialog::GetPreferredHeight(theWidth) + 40;
}

//0x45D910
void NewHintDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
	LawnDialog::Resize(theX, theY, theWidth, theHeight);
	mHintEditWidget->Resize(mContentInsets.mLeft + 12, mHeight - 205, mWidth - mContentInsets.mLeft - mContentInsets.mRight - 24, 28);
}

//0x45D970
void NewHintDialog::Draw(Graphics* g)
{
	LawnDialog::Draw(g);
	DrawEditBox(g, mHintEditWidget);
	
	Sexy::Color aTextColor(107, 109, 145);
	TodDrawString(g, "Available Hint Points: " + std::to_string(mApp->mAP->AvailableHintPoints()), mContentInsets.mLeft + 12, 210, FONT_DWARVENTODCRAFT15, aTextColor, DrawStringJustification::DS_ALIGN_LEFT);
	TodDrawString(g, "Required Hint Points: " + std::to_string(mApp->mAP->RequiredHintPoints()), mContentInsets.mLeft + 12, 230, FONT_DWARVENTODCRAFT15, aTextColor, DrawStringJustification::DS_ALIGN_LEFT);
	
	if (mAdmonition && !mAdmonition->IsDead())
	{
		mAdmonition->Draw(g);
	}
}

//0x45D9D0
void NewHintDialog::EditWidgetText(int theId, const SexyString& theString)
{
	this->ButtonDepress(1000);
}

//0x45D9F0
bool NewHintDialog::AllowChar(int, SexyChar theChar)
{
	return true;
}

void NewHintDialog::ButtonDepress(int theId)
{
	if (mProcessing)
	{
		return;
	}
	
	if (theId == 1000) {
		// Request Hint
		mProcessing = true;
		if (mHintEditWidget->mString.empty())
		{
			mApp->DoDialog(Dialogs::DIALOG_INFO, true, "Hint", "Enter the name of an item to request a hint for", "OK", Dialog::BUTTONS_FOOTER);
			return;
		}
		mApp->mAP->SendAPMessage("!hint " + mHintEditWidget->mString);
		return;
	}
	LawnDialog::ButtonDepress(theId);
}

void NewHintDialog::Update()
{
	LawnDialog::Update();
	
	if (mAdmonition) {
		mAdmonition->Update();
	}
}
