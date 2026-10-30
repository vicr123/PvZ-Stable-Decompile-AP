#include "HintClientDialog.h"

#include <format>

#include "AlmanacDialog.h"
#include "GameButton.h"
#include "../Board.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "../System/Music.h"
#include "../../Resources.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../SexyAppFramework/APWrapper.h"
#include "../../Lawn/ToolTipWidget.h"

const auto x_sender = 20;
const auto x_receiver = 100;
const auto x_item = 220;
const auto x_location = 330;
const auto x_status = 580;

struct HintClientDialogPrivate
{
    std::vector<Hint> hints;
    int hoveredHint = -1;
    int hoveredSection = -1;
    MusicTune music;
};

void DrawStringBetween(Graphics* g, const SexyString& theText, int thePosX, int thePosXLimit, int thePosY, Font* theFont, const Color& theColor)
{
    auto string = theText;
    auto max_width = thePosXLimit - thePosX - 10;
    auto width = theFont->StringWidth(theText);
    if (width > max_width)
    {
        while (width > max_width)
        {
            string = string.substr(0, string.length() - 1);
            width = theFont->StringWidth(string + "...");
        }
        TodDrawString(g, string + "...", thePosX, thePosY, theFont, theColor, DrawStringJustification::DS_ALIGN_LEFT);
    }
    else
    {
        TodDrawString(g, string, thePosX, thePosY, theFont, theColor, DrawStringJustification::DS_ALIGN_LEFT);
    }
}

HintClientDialog::HintClientDialog(LawnApp* theApp) : LawnDialog(theApp, DIALOG_ALMANAC, true, _S("Hints"),
                                                                 _S(""), _S(""), BUTTONS_NONE)
{
    d = new HintClientDialogPrivate();
    
    mApp = (LawnApp*)gSexyAppBase;
    mDrawStandardBack = false;
    TodLoadResources("DelayLoad_Almanac");
	TodLoadResources("DelayLoad_ChallengeScreen");
    LawnDialog::Resize(0, 0, BOARD_WIDTH, BOARD_HEIGHT);

    mCloseButton = new GameButton(HintClientDialog::ALMANAC_BUTTON_CLOSE);
    mCloseButton->SetLabel(_S("[CLOSE_BUTTON]"));
    mCloseButton->mButtonImage = Sexy::IMAGE_ALMANAC_CLOSEBUTTON;
    mCloseButton->mOverImage = Sexy::IMAGE_ALMANAC_CLOSEBUTTONHIGHLIGHT;
    mCloseButton->mDownImage = nullptr;
    mCloseButton->SetFont(Sexy::FONT_BRIANNETOD12);
    Color aColor = Color(42, 42, 90);
    mCloseButton->mColors[ButtonWidget::COLOR_LABEL] = aColor;
    mCloseButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = aColor;
    mCloseButton->Resize(676, 567, 89, 26);
    mCloseButton->mParentWidget = this;
    mCloseButton->mTextOffsetX = -8;
    mCloseButton->mTextOffsetY = 1;
    
    mNewHintButton = new GameButton(HintClientDialog::ALMANAC_BUTTON_INDEX);
    mNewHintButton->SetLabel("REQUEST HINT");
    mNewHintButton->mButtonImage = Sexy::IMAGE_ALMANAC_INDEXBUTTON;
    mNewHintButton->mOverImage = Sexy::IMAGE_ALMANAC_INDEXBUTTONHIGHLIGHT;
    mNewHintButton->mDownImage = nullptr;
    mNewHintButton->SetFont(Sexy::FONT_BRIANNETOD12);
    mNewHintButton->mColors[ButtonWidget::COLOR_LABEL] = aColor;
    mNewHintButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = aColor;
    mNewHintButton->Resize(32, 567, 164, 26);
    mNewHintButton->mParentWidget = this;
    mNewHintButton->mTextOffsetX = 8;
    mNewHintButton->mTextOffsetY = 1;
    
    mCopyButton = new GameButton(HintClientDialog::ALMANAC_BUTTON_COPY);
    mCopyButton->SetLabel(_S("COPY"));
    mCopyButton->mButtonImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON2;
    mCopyButton->mOverImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW;
    mCopyButton->mDownImage = nullptr;
    mCopyButton->SetFont(Sexy::FONT_BRIANNETOD12);
    mCopyButton->mColors[0] = Color(42, 42, 90);
    mCopyButton->mColors[1] = Color(42, 42, 90);
    mCopyButton->Resize(BOARD_WIDTH - 30 - IMAGE_SEEDCHOOSER_BUTTON2->GetWidth(), 561, IMAGE_SEEDCHOOSER_BUTTON2->GetWidth(), IMAGE_SEEDCHOOSER_BUTTON2_GLOW->GetHeight());
    mCopyButton->mParentWidget = this;
    mCopyButton->mTextOffsetY = 1;
    mCopyButton->mBtnNoDraw = true;

    mScrollbar = new ScrollbarWidget(mApp, ScrollbarMode::VERTICAL);
    mScrollbar->mScrollRange = 100;
    mScrollbar->Resize(BOARD_WIDTH - 28, 110, 8, BOARD_HEIGHT - 150);
    mScrollbar->mViewport = Rect(20, 110, BOARD_WIDTH - 40, BOARD_HEIGHT - 150);
    
    mToolTip = new ToolTipWidget();

    d->music = mApp->mMusic->mCurMusicTune;
    if (!mApp->mBoard || !mApp->mBoard->mPaused)
        mApp->mMusic->MakeSureMusicIsPlaying(MUSIC_TUNE_CHOOSE_YOUR_SEEDS);
    
    mScrollAmount = 0;
    
    mAPDataStorageValueChangedListener = mApp->mAP->AddDataStorageValueChangeListener([this](const std::string& key, const nlohmann::json& value)
    {
        this->UpdateHints();
        this->Update();
    });
    this->UpdateHints();
}

HintClientDialog::~HintClientDialog()
{
    if (d->music != MusicTune::MUSIC_TUNE_NONE)
    {
        mApp->mMusic->MakeSureMusicIsPlaying(d->music);
    }
    
    if (mCloseButton) delete mCloseButton;
	if (mNewHintButton)	delete mNewHintButton;
    if (mCopyButton) delete mCopyButton;
	if (mScrollbar)		delete mScrollbar;
    delete mToolTip;
    delete mAPDataStorageValueChangedListener;
    delete d;
}

void HintClientDialog::Update()
{
    mCloseButton->Update();
	mNewHintButton->Update();
    mCopyButton->Update();
    mScrollbar->Update();
    
    auto cursor = CURSOR_POINTER;

    if (mApp->mWidgetManager->mOverWidget)
    {
        if (mCloseButton->IsMouseOver() || mNewHintButton->IsMouseOver() || mCopyButton->IsMouseOver())
        {
            cursor = CURSOR_HAND;
        }
    }

    Widget::Update();

    if (!mScrollbar->mDisabled)
    {
    	float aScrollSpeed = 1 + abs(mScrollAmount) * 0.1f;
    	mScrollbar->mScrollValue = ClampFloat(mScrollbar->mScrollValue += mScrollAmount * aScrollSpeed, 0, mScrollbar->mScrollRange);
    	mScrollAmount *= 0.9f;
    }
    
    if (d->hoveredHint != -1)
    {
        auto hint = d->hints[d->hoveredHint];
        if (hint.receiving_player == mApp->mAP->MySlot() && !hint.found && d->hoveredSection == 4)
        {
            cursor = CURSOR_HAND;
        }
    }

    mApp->SetCursor(cursor);
    MarkDirty();
}

void HintClientDialog::AddedToManager(WidgetManager* theWidgetManager)
{
    LawnDialog::AddedToManager(theWidgetManager);
    this->AddWidget(mScrollbar);
}

void HintClientDialog::RemovedFromManager(WidgetManager* theWidgetManager)
{
    LawnDialog::RemovedFromManager(theWidgetManager);
    this->RemoveWidget(mScrollbar);
}

void HintClientDialog::Draw(Graphics* g)
{
    g->SetLinearBlend(true);

    g->DrawImage(Sexy::IMAGE_CHALLENGE_BACKGROUND, 0, 0);
    TodDrawString(g, "Hints", BOARD_WIDTH / 2, 60, Sexy::FONT_HOUSEOFTERROR28, Color(220, 220, 220),
                  DrawStringJustification::DS_ALIGN_CENTER);


    mCloseButton->Draw(g);
    mCopyButton->Draw(g);
    mNewHintButton->Draw(g);
    
    auto titles = Color(40, 50, 90);
    auto black = Color(0, 0, 0);
    auto black_gray = Color(0, 0, 0, 100);
    
    auto y = 110;
    auto central_viewport = Rect(20, 115, BOARD_WIDTH - 40, BOARD_HEIGHT - 150);
    
    if (d->hints.empty())
    {
        TodDrawString(g, "No hints have been requested from the server", central_viewport.mX + central_viewport.mWidth / 2, central_viewport.mY + central_viewport.mHeight / 2, Sexy::FONT_BRIANNETOD16, titles, DrawStringJustification::DS_ALIGN_CENTER);
    }
    else
    {
        TodDrawString(g, "Sender", x_sender, y, Sexy::FONT_BRIANNETOD12, titles, DrawStringJustification::DS_ALIGN_LEFT);
        TodDrawString(g, "Receiver", x_receiver, y, Sexy::FONT_BRIANNETOD12, titles, DrawStringJustification::DS_ALIGN_LEFT);
        TodDrawString(g, "Item", x_item, y, Sexy::FONT_BRIANNETOD12, titles, DrawStringJustification::DS_ALIGN_LEFT);
        TodDrawString(g, "Location", x_location, y, Sexy::FONT_BRIANNETOD12, titles, DrawStringJustification::DS_ALIGN_LEFT);
        TodDrawString(g, "Status", x_status, y, Sexy::FONT_BRIANNETOD12, titles, DrawStringJustification::DS_ALIGN_LEFT);
        
        y += 18;
        y -= mScrollbar->mScrollValue;
        
        g->PushState();
        g->SetClipRect(central_viewport);
        for (auto hint : d->hints)
        {
            auto sender = mApp->mAP->PlayerDisplayName(hint.finding_player);
            auto receiver = mApp->mAP->PlayerDisplayName(hint.receiving_player);
            auto item = mApp->mAP->ItemName(hint.item, hint.receiving_player);
            auto location = mApp->mAP->LocationName(hint.location, mApp->mAP->PlayerGameName(hint.finding_player));
            
            std::string status = "Unspecified";
            if (hint.status == 10)
            {
                status = "No Priority";
            }
            else if (hint.status == 20)
            {
                status = "Avoid";
            }
            else if (hint.status == 30)
            {
                status = "Priority";
            }
            else if (hint.status == 40)
            {
                status = "Found";
            }
            
            
            auto fg = black;
            if (hint.found)
            {
                fg = black_gray;
            }
            
            
            DrawStringBetween(g, sender, x_sender, x_receiver, y, Sexy::FONT_PICO129, fg);
            DrawStringBetween(g, receiver, x_receiver, x_item, y, Sexy::FONT_PICO129, fg);
            DrawStringBetween(g, item, x_item, x_location, y, Sexy::FONT_PICO129, fg);
            DrawStringBetween(g, location, x_location, x_status, y, Sexy::FONT_PICO129, fg);
            DrawStringBetween(g, status, x_status, BOARD_WIDTH - 20, y, Sexy::FONT_PICO129, fg);

            y += 20;
        }
        g->PopState();
    }

    if (!mScrollbar->mDisabled)
    {
    	mScrollbar->mButtonNoDraw = false;
    	g->PushState();
    	g->mTransX = g->mTransX + mScrollbar->mX;
    	g->mTransY = g->mTransY + mScrollbar->mY;
    	mScrollbar->Draw(g);
    	mScrollbar->mButtonNoDraw = true;
    	g->PopState();
    }
    
    mToolTip->Draw(g);
}

void HintClientDialog::MouseDown(int x, int y, int theClickCount)
{
    if (!this->mHasFocus)
    {
        mWidgetManager->SetFocus(this);
    }
    
    if (mCloseButton->IsMouseOver() || mNewHintButton->IsMouseOver() || mCopyButton->IsMouseOver())
        mApp->PlaySample(Sexy::SOUND_TAP);
    
    if(mScrollbar->isThumbOver())
    {
    	mScrollbar->MouseDown(x, y, theClickCount);
    }
}

void HintClientDialog::MouseMove(int x, int y)
{
    LawnDialog::MouseMove(x, y);
    
    d->hoveredHint = -1;
    mToolTip->mVisible = false;
    
    mCopyButton->mBtnNoDraw = true;
    
    // Hit test the table
    if (Rect(20, 115, BOARD_WIDTH - 40, BOARD_HEIGHT - 150).Contains(x, y))
    {
        // Find out which hint we are hovering over
        auto firstY = 113 - mScrollbar->mScrollValue;
        auto itemHeight = 20;
        
        auto yOffset = y - firstY;
        auto index = static_cast<int>(yOffset / itemHeight);
        if (index < 0) index = -1;
        if (index >= d->hints.size()) index = -1;
        
        if (index != -1)
        {
            d->hoveredHint = index;
            
            mCopyButton->mY = firstY + index * itemHeight;
            mCopyButton->mBtnNoDraw = false;
        }
        
        if (x > mCopyButton->mX)
        {
            d->hoveredSection = -1;
        }
        else if (x > x_status)
        {
            d->hoveredSection = 4;
        }
        else if (x > x_location)
        {
            d->hoveredSection = 3;
        }
        else if (x > x_item)
        {
            d->hoveredSection = 2;
        }
        else if (x > x_receiver)
        {
            d->hoveredSection = 1;
        }
        else
        {
            d->hoveredSection = 0;
        }
        
        if (d->hoveredHint != -1)
        {
            auto hint = d->hints[d->hoveredHint];
            if (hint.receiving_player == mApp->mAP->MySlot() && d->hoveredSection == 4 && !hint.found)
            {
                mToolTip->SetLabel("Click to change status");
                mToolTip->mVisible = true;
            }
            else if (d->hoveredSection == 3)
            {
                mToolTip->SetLabel(mApp->mAP->LocationName(hint.location, mApp->mAP->PlayerGameName(hint.finding_player)));
                mToolTip->mVisible = FONT_PICO129->StringWidth(mToolTip->mLabel) > x_status - x_location;
            }
            else if (d->hoveredSection == 2)
            {
                mToolTip->SetLabel(mApp->mAP->ItemName(hint.item, hint.receiving_player));
                mToolTip->mVisible = FONT_PICO129->StringWidth(mToolTip->mLabel) > x_location - x_item;
            }
            else if (d->hoveredSection == 1)
            {
                mToolTip->SetLabel(mApp->mAP->PlayerDisplayName(hint.receiving_player));
                mToolTip->mVisible = FONT_PICO129->StringWidth(mToolTip->mLabel) > x_item - x_receiver;
            }
            else if (d->hoveredSection == 0)
            {
                mToolTip->SetLabel(mApp->mAP->PlayerDisplayName(hint.finding_player));
                mToolTip->mVisible = FONT_PICO129->StringWidth(mToolTip->mLabel) > x_receiver - x_sender;
            }
        }
    }
    
    mToolTip->SetPosition(x + 10, y + 10);
}

void HintClientDialog::MouseUp(int x, int y, int theClickCount)
{
    if (mCloseButton->IsMouseOver()) mApp->KillHintClient();
    if (mNewHintButton->IsMouseOver())
    {
        mApp->DoRequestNewHintDialog();
    }
    if (d->hoveredHint != -1)
    {
        if (mCopyButton->IsMouseOver() && !mCopyButton->mBtnNoDraw)
        {
            auto hint = d->hints[d->hoveredHint];
            
            auto sender = mApp->mAP->PlayerDisplayName(hint.finding_player);
            auto receiver = mApp->mAP->PlayerDisplayName(hint.receiving_player);
            auto item = mApp->mAP->ItemName(hint.item, hint.receiving_player);
            auto location = mApp->mAP->LocationName(hint.location, mApp->mAP->PlayerGameName(hint.finding_player));
            
            auto copy_string = std::format("[Hint]: {}'s {} is at {} in {}'s World.", receiver, item, location, sender);
            mWidgetManager->mApp->CopyToClipboard(SexyStringToString(copy_string));
        }
        else if (d->hoveredSection == 4)
        {
            auto hint = &d->hints[d->hoveredHint];
            if (hint->receiving_player == mApp->mAP->MySlot() && !hint->found)
            {
                if (hint->status == 30) // Priority
                {
                    mApp->mAP->SetNoPriorityHint(*hint);
                    hint->status = 10;
                    mApp->PlaySample(Sexy::SOUND_TAP);
                
                }
                else if (hint->status == 10) // No Priority
                {
                    mApp->mAP->SetAvoidHint(*hint);
                    hint->status = 20;
                    mApp->PlaySample(Sexy::SOUND_TAP);
                }
                else
                {
                    mApp->mAP->SetPriorityHint(*hint);
                    hint->status = 30;
                    mApp->PlaySample(Sexy::SOUND_TAP);
                }
            }
        }
    }
}

void HintClientDialog::MouseWheel(int theDelta)
{
    Widget::MouseWheel(theDelta);

    if (mScrollbar->mDisabled) return;

    mScrollAmount -= theDelta;
    mScrollAmount -= mScrollAmount * 0.1f;
}

void HintClientDialog::UpdateHints()
{
    d->hints = mApp->mAP->Hints();
    ranges::stable_sort(d->hints, [](const Hint& a, const Hint& b)
    {
        return b.found;
    });
    
    auto total_hint_height = static_cast<long long>(d->hints.size()) * 20;
    auto viewport_height = BOARD_HEIGHT - 150;
    auto required_scroll = total_hint_height - viewport_height;
    if (required_scroll < 0) required_scroll = 0;
    mScrollbar->mScrollRange = required_scroll;
    mScrollbar->SetDisabled(required_scroll == 0);
}