#include <time.h>
#include "../Coin.h"
#include "../Board.h"
#include "../Plant.h"
#include "LawnDialog.h"
#include "GameButton.h"
#include "StoreScreen.h"

#include <nlohmann/json.hpp>

#include "../ZenGarden.h"
#include "../SeedPacket.h"
#include "../../LawnApp.h"
#include "../../Resources.h"
#include "../System/Music.h"
#include "SeedChooserScreen.h"
#include "../../GameConstants.h"
#include "../System/PopDRMComm.h"
#include "../../Sexy.TodLib/TodFoley.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../Sexy.TodLib/Reanimator.h"
#include "../../SexyAppFramework/Debug.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../SexyAppFramework/ImageFont.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "AchievementsScreen.h"
#include "../../SexyAppFramework/APData.h"
#include "../../SexyAppFramework/APDialogueClient.h"
#include "../../SexyAppFramework/APWrapper.h"
#include "../Admonition.h"

constexpr uint64_t EnergyLinkExchangeWithdraw = 100000000;
constexpr uint64_t EnergyLinkExchangeDeposit = EnergyLinkExchangeWithdraw * 3 / 4;

static StoreItem gStoreItemSpots[NUM_STORE_PAGES][MAX_PAGE_SPOTS] =
{
    {
        STORE_ITEM_PACKET_UPGRADE, STORE_ITEM_POOL_CLEANER, STORE_ITEM_RAKE, STORE_ITEM_ROOF_CLEANER,
        STORE_ITEM_PLANT_GATLINGPEA, STORE_ITEM_PLANT_TWINSUNFLOWER, STORE_ITEM_PLANT_GLOOMSHROOM,
        STORE_ITEM_PLANT_CATTAIL
    },
    {
        STORE_ITEM_PLANT_SPIKEROCK, STORE_ITEM_PLANT_GOLD_MAGNET, STORE_ITEM_PLANT_WINTERMELON,
        STORE_ITEM_PLANT_COBCANNON,
        STORE_ITEM_PLANT_IMITATER, STORE_ITEM_FIRSTAID, STORE_ITEM_INVALID, STORE_ITEM_INVALID
    },
    {
        STORE_ITEM_POTTED_MARIGOLD_1, STORE_ITEM_POTTED_MARIGOLD_2, STORE_ITEM_POTTED_MARIGOLD_3,
        STORE_ITEM_GOLD_WATERINGCAN,
        STORE_ITEM_FERTILIZER, STORE_ITEM_BUG_SPRAY, STORE_ITEM_PHONOGRAPH, STORE_ITEM_GARDENING_GLOVE
    },
    {
        STORE_ITEM_MUSHROOM_GARDEN, STORE_ITEM_AQUARIUM_GARDEN, STORE_ITEM_WHEEL_BARROW, STORE_ITEM_STINKY_THE_SNAIL,
        STORE_ITEM_TREE_OF_WISDOM, STORE_ITEM_TREE_FOOD, STORE_ITEM_INVALID, STORE_ITEM_INVALID
    }
};

static const std::string progression_bank[4] = {
    "It's CRAZY important!",
    "You'd have to be CRAZY to pass this one up!",
    "I think of this store as quite a progressive place!",
    "It's as essential to progressing as hot sauce is to making a good taco."
};
static const std::string useful_bank[4] = {
    "It's darn handy. Handier than a staple gun.",
    "At a price like that, how can you say no?",
    "Buy this and you'll be A-O-GOOD.",
    "To some, this is worthless. To others, it's worth the arbitrary price I decided upon."
};
static const std::string trap_bank[4] = {
    "Something smells fishy and I don't think it's just my lunch.",
    "I think a zombie put this here.",
    "No refunds for damage caused by cross-multiworld delivery!",
    "I wouldn't touch it without gloves on."
};
static const std::string standard_bank[4] = {
    "It probably won't amount to much, but that's what they said about me!",
    "It tastes like dirt! It's around the same price, too.",
    "It's called filler, but I still feel hungry!",
    "It's about as useful as a Wall-nut is against a Zomboni.",
};

StoreScreenOverlay::StoreScreenOverlay(StoreScreen* theParent)
{
    mParent = theParent;
    mMouseVisible = false;
    mHasAlpha = true;
}

void StoreScreenOverlay::Draw(Graphics* g)
{
    mParent->DrawOverlay(g);
}

const int BackButtonOffset = 366;
const int PrevButtonOffset = 252;
const int NextButtonOffset = 596;
const int EnergyButtonOffset = 669;
const int EnergyBackButtonOffset = 260;

//0x489DA0
StoreScreen::StoreScreen(LawnApp* theApp) : Dialog(nullptr, nullptr, DIALOG_STORE, true, _S("Store"), _S(""), _S(""),
                                                   BUTTONS_NONE)
{
    mApp = theApp;
    mClip = false;
    mStoreTime = 0;
    mBubbleCountDown = 0;
    mBubbleClickToContinue = false;
    mAmbientSpeechCountDown = 200;
    mPreviousAmbientSpeechIndex = -1;
    mPage = 0;
    mMouseOverItem = -1;
    mHatchTimer = 0;
    mShakeX = 0;
    mShakeY = 0;
    mStartDialog = -1;
    mHatchOpen = true;
    mEasyBuyingCheat = false;
    mWaitForDialog = false;
    mInert = false;
    mCoins.DataArrayInitialize(1024U, "coins");
    TodLoadResources("DelayLoad_Store");
    Resize(0, 0, BOARD_WIDTH * 2, BOARD_HEIGHT);
    mPottedPlantSpecs.InitializePottedPlant(SEED_MARIGOLD);
    mPottedPlantSpecs.mDrawVariation = (DrawVariation)RandRangeInt(VARIATION_MARIGOLD_WHITE,
                                                                   VARIATION_MARIGOLD_LIGHT_GREEN);
    mCrazyDaveLastTalkIndex = -1;

    mBackButton = new NewLawnButton(nullptr, StoreScreen::StoreScreen_Back, this);
    mBackButton->mDoFinger = true;
    mBackButton->SetLabel(_S("[STORE_MAIN_MENU_BUTTON]"));
    Image* aMenuImage = Sexy::IMAGE_STORE_MAINMENUBUTTON;
    mBackButton->mButtonImage = aMenuImage;
    mBackButton->mOverImage = Sexy::IMAGE_STORE_MAINMENUBUTTONHIGHLIGHT;
    mBackButton->mDownImage = Sexy::IMAGE_STORE_MAINMENUBUTTONDOWN;
    mBackButton->SetFont(Sexy::FONT_HOUSEOFTERROR20);
    mBackButton->mColors[ButtonWidget::COLOR_LABEL] = Color(98, 153, 235);
    mBackButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(167, 192, 235);
    mBackButton->Resize(BackButtonOffset, 512, aMenuImage->mWidth, aMenuImage->mHeight);
    mBackButton->mTextOffsetX = -7;
    mBackButton->mTextOffsetY = 1;
    mBackButton->mTextDownOffsetX = 2;
    mBackButton->mTextDownOffsetY = 1;
    mBackButton->mBtnNoDraw = true;

    mPrevButton = new NewLawnButton(nullptr, StoreScreen::StoreScreen_Prev, this);
    mPrevButton->mDoFinger = true;
    mPrevButton->SetLabel(_S(""));
    Image* aPrevImage = Sexy::IMAGE_STORE_PREVBUTTON;
    mPrevButton->mButtonImage = aPrevImage;
    mPrevButton->mOverImage = Sexy::IMAGE_STORE_PREVBUTTONHIGHLIGHT;
    mPrevButton->mDownImage = Sexy::IMAGE_STORE_PREVBUTTONHIGHLIGHT;
    mPrevButton->mColors[ButtonWidget::COLOR_LABEL] = Color(255, 240, 0);
    mPrevButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(200, 200, 255);
    mPrevButton->Resize(PrevButtonOffset, 402, aPrevImage->mWidth, aPrevImage->mHeight);
    mPrevButton->mBtnNoDraw = true;

    mNextButton = new NewLawnButton(nullptr, StoreScreen::StoreScreen_Next, this);
    mNextButton->mDoFinger = true;
    mNextButton->SetLabel(_S(""));
    Image* aNextImage = Sexy::IMAGE_STORE_NEXTBUTTON;
    mNextButton->mButtonImage = aNextImage;
    mNextButton->mOverImage = Sexy::IMAGE_STORE_NEXTBUTTONHIGHLIGHT;
    mNextButton->mDownImage = Sexy::IMAGE_STORE_NEXTBUTTONHIGHLIGHT;
    mNextButton->mColors[ButtonWidget::COLOR_LABEL] = Color(255, 240, 0);
    mNextButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(200, 200, 255);
    mNextButton->Resize(NextButtonOffset, 402, aNextImage->mWidth, aNextImage->mHeight);
    mNextButton->mBtnNoDraw = true;

    mEnergyButton = MakeNewButton(StoreScreen::StoreScreen_Energy, this, _S("EnergyLink"), nullptr,
                                  Sexy::IMAGE_SEEDCHOOSER_BUTTON2,
                                  Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW, Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW);
    mEnergyButton->mTextDownOffsetX = 1;
    mEnergyButton->mTextDownOffsetY = 1;
    mEnergyButton->mColors[ButtonWidget::COLOR_LABEL] = Color(42, 42, 90);
    mEnergyButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(42, 42, 90);
    mEnergyButton->Resize(EnergyButtonOffset, 518, 111, 26);
    mEnergyButton->mBtnNoDraw = true;

    mEnergyBackButton = MakeNewButton(StoreScreen::StoreScreen_EnergyBack, this, _S("Back to Shop"), nullptr,
                                      Sexy::IMAGE_SEEDCHOOSER_BUTTON2,
                                      Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW, Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW);
    mEnergyBackButton->mTextDownOffsetX = 1;
    mEnergyBackButton->mTextDownOffsetY = 1;
    mEnergyBackButton->mColors[ButtonWidget::COLOR_LABEL] = Color(42, 42, 90);
    mEnergyBackButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(42, 42, 90);
    mEnergyBackButton->Resize(mApp->mWidth + EnergyBackButtonOffset, 518, 111, 26);
    mEnergyBackButton->mBtnNoDraw = true;

    mDepositButton = MakeNewButton(StoreScreen::StoreScreen_EnergyDeposit, this, _S("Deposit"), nullptr,
                                   Sexy::IMAGE_SEEDCHOOSER_BUTTON2,
                                   Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW, Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW);
    mDepositButton->mTextDownOffsetX = 1;
    mDepositButton->mTextDownOffsetY = 1;
    mDepositButton->mColors[ButtonWidget::COLOR_LABEL] = Color(42, 42, 90);
    mDepositButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(42, 42, 90);
    mDepositButton->Resize(mApp->mWidth + 440, 300, 111, 26);
    mDepositButton->mBtnNoDraw = true;

    mWithdrawButton = MakeNewButton(StoreScreen::StoreScreen_EnergyWithdraw, this, _S("Withdraw"), nullptr,
                                    Sexy::IMAGE_SEEDCHOOSER_BUTTON2,
                                    Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW, Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW);
    mWithdrawButton->mTextDownOffsetX = 1;
    mWithdrawButton->mTextDownOffsetY = 1;
    mWithdrawButton->mColors[ButtonWidget::COLOR_LABEL] = Color(42, 42, 90);
    mWithdrawButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(42, 42, 90);
    mWithdrawButton->Resize(mApp->mWidth + 440, 350, 111, 26);
    mWithdrawButton->mBtnNoDraw = true;

    mEnergyAtmBackButton = MakeNewButton(StoreScreen::StoreScreen_EnergyAtmBack, this, _S("Back to Menu"), nullptr,
                                         Sexy::IMAGE_SEEDCHOOSER_BUTTON2,
                                         Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW, Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW);
    mEnergyAtmBackButton->mTextDownOffsetX = 1;
    mEnergyAtmBackButton->mTextDownOffsetY = 1;
    mEnergyAtmBackButton->mColors[ButtonWidget::COLOR_LABEL] = Color(42, 42, 90);
    mEnergyAtmBackButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(42, 42, 90);
    mEnergyAtmBackButton->Resize(mApp->mWidth + 420, 270, 111, 26);
    mEnergyAtmBackButton->mBtnNoDraw = true;

    mEnergyLessButton = MakeNewButton(StoreScreen::StoreScreen_EnergyLess, this, _S(""), nullptr,
                                      Sexy::IMAGE_ZOMBATAR_PREV_BUTTON,
                                      Sexy::IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT,
                                      Sexy::IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT);
    mEnergyLessButton->mTextDownOffsetX = 1;
    mEnergyLessButton->mTextDownOffsetY = 1;
    mEnergyLessButton->mColors[ButtonWidget::COLOR_LABEL] = Color(42, 42, 90);
    mEnergyLessButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(42, 42, 90);
    mEnergyLessButton->Resize(mApp->mWidth + 390, 300, 33, 38);
    mEnergyLessButton->mBtnNoDraw = true;

    mEnergyMoreButton = MakeNewButton(StoreScreen::StoreScreen_EnergyMore, this, _S(""), nullptr,
                                      Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON,
                                      Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT,
                                      Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT);
    mEnergyMoreButton->mTextDownOffsetX = 1;
    mEnergyMoreButton->mTextDownOffsetY = 1;
    mEnergyMoreButton->mColors[ButtonWidget::COLOR_LABEL] = Color(42, 42, 90);
    mEnergyMoreButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(42, 42, 90);
    mEnergyMoreButton->Resize(mApp->mWidth + 570, 300, 33, 38);
    mEnergyMoreButton->mBtnNoDraw = true;

    mEnergyGoButton = MakeNewButton(StoreScreen::StoreScreen_EnergyGo, this, _S("Exchange"), nullptr,
                                    Sexy::IMAGE_SEEDCHOOSER_BUTTON2,
                                    Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW, Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW);
    mEnergyGoButton->mTextDownOffsetX = 1;
    mEnergyGoButton->mTextDownOffsetY = 1;
    mEnergyGoButton->mColors[ButtonWidget::COLOR_LABEL] = Color(42, 42, 90);
    mEnergyGoButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color(42, 42, 90);
    mEnergyGoButton->Resize(mApp->mWidth + 440, 380, 111, 26);
    mEnergyGoButton->mBtnNoDraw = true;

    mOverlayWidget = new StoreScreenOverlay(this);
    mOverlayWidget->Resize(0, 0, BOARD_WIDTH * 2, BOARD_HEIGHT);
    
    mAdmonition = new Admonition();

    mSlideCounter = 0;
    mStartX = 0;
    mStartY = 0;
    mDestX = 0;
    mDestY = 0;
    mCrazyDaveOffset = 0;
    mCurrentScreen = CarScreen;
    mEnergyMode = Idle;
    mZapTickCounter = 0;

    if (!IsPageShown(0))
    {
        mPage = 1;
    }
    
    if (!IsPageShown(mPage + 1))
    {
        mPrevButton->mDisabledImage = Sexy::IMAGE_STORE_PREVBUTTONDISABLED;
        mPrevButton->SetDisabled(true);
        mNextButton->mDisabledImage = Sexy::IMAGE_STORE_NEXTBUTTONDISABLED;
        mNextButton->SetDisabled(true);
    }
    mDrawnOnce = false;
    mGoToTreeNow = false;
    mPurchasedFullVersion = false;
    mTrialLockedWhenStoreOpened = mApp->IsTrialStageLocked();

    mItemHandler = mApp->mAP->AddItemsReceivedListener([this](const std::list<APItem>&)
    {
        EnableButtons(true);
    });
    
    ScoutPage();
}

//0x48A610、0x48A630
StoreScreen::~StoreScreen()
{
    delete mItemHandler;
    mCoins.DataArrayDispose();
    if (mBackButton) delete mBackButton;
    if (mPrevButton) delete mPrevButton;
    if (mNextButton) delete mNextButton;
    if (mEnergyButton) delete mEnergyButton;
    if (mEnergyBackButton) delete mEnergyBackButton;
    if (mDepositButton) delete mDepositButton;
    if (mWithdrawButton) delete mWithdrawButton;
    if (mEnergyAtmBackButton) delete mEnergyAtmBackButton;
    if (mEnergyLessButton) delete mEnergyLessButton;
    if (mEnergyMoreButton) delete mEnergyMoreButton;
    if (mEnergyGoButton) delete mEnergyGoButton;
    if (mOverlayWidget) delete mOverlayWidget;
}

//0x48A760
StoreItem StoreScreen::GetStoreItemType(int theSpotIndex)
{
    // 这个函数原版是穷举判断的，这里优化一下……
    if (mPage == 0)
    {
        switch (theSpotIndex)
        {
        case 0:
            return STORE_ITEM_FERTILIZER;
        case 1:
            return STORE_ITEM_BUG_SPRAY;
        case 2:
            return STORE_ITEM_TREE_FOOD;
        default:
            return STORE_ITEM_INVALID;
        }
    }

    if (mApp->mAP->IsLocationPresent(PVZRAPData::Locations::Twiddydinkie(theSpotIndex + (mPage - 1) * 8)))
    {
        return STORE_ITEM_AP;
    }
    else
    {
        return STORE_ITEM_INVALID;
    }

    if (mPage < NUM_STORE_PAGES && theSpotIndex < MAX_PAGE_SPOTS)
    {
        if (mPage == STORE_PAGE_SLOT_UPGRADES && theSpotIndex == 6 && mApp->IsTrialStageLocked())
        {
            return STORE_ITEM_PVZ;
        }
        return gStoreItemSpots[mPage][theSpotIndex];
    }

    TOD_ASSERT();
    return STORE_ITEM_INVALID;
}

//0x48A8D0
bool StoreScreen::IsFullVersionOnly(StoreItem theStoreItem)
{
    if (!mApp->IsTrialStageLocked())
        return false;

    if (theStoreItem == STORE_ITEM_PACKET_UPGRADE && mApp->mPlayerInfo->mPurchases[STORE_ITEM_PACKET_UPGRADE] >= 2)
        return true;

    return theStoreItem == STORE_ITEM_PLANT_TWINSUNFLOWER;
}

bool StoreScreen::IsPottedPlant(StoreItem theStoreItem)
{
    return theStoreItem == STORE_ITEM_POTTED_MARIGOLD_1 || theStoreItem == STORE_ITEM_POTTED_MARIGOLD_2 || theStoreItem
        == STORE_ITEM_POTTED_MARIGOLD_3;
}

//0x48A940
bool StoreScreen::IsComingSoon(StoreItem theStoreItem)
{
    return false;

    if (IsFullVersionOnly(theStoreItem))
        return true;
    else if (theStoreItem == STORE_ITEM_WHEEL_BARROW)
        return !mApp->mPlayerInfo->mPurchases[STORE_ITEM_MUSHROOM_GARDEN] && !mApp->mPlayerInfo->mPurchases[
            STORE_ITEM_AQUARIUM_GARDEN];
    else if (IsPottedPlant(theStoreItem))
        return !mApp->HasFinishedAdventure();
    else if (theStoreItem == STORE_ITEM_TREE_FOOD)
        return !mApp->mPlayerInfo->mPurchases[STORE_ITEM_TREE_OF_WISDOM] || mApp->mPlayerInfo->mPurchases[
            STORE_ITEM_TREE_FOOD] < PURCHASE_COUNT_OFFSET;
    return false;
}

//0x48A9D0
bool StoreScreen::IsItemSoldOut(int theIndex)
{
    if (mPage == 0)
    {
        return false;
    }
    
    return mApp->mAP->IsLocationChecked(PVZRAPData::Locations::Twiddydinkie(theIndex + (mPage - 1) * 8));
}

//0x48AAD0
bool StoreScreen::IsItemUnavailable(StoreItem theStoreItem)
{
    return false;

    if (mEasyBuyingCheat)
        return false;

    /*
    if (mApp->HasFinishedAdventure())
        return true;

    bool aTrialStageLocked = mApp->IsTrialStageLocked();
    int aCurrentLevel = mApp->mPlayerInfo->mLevel;
    if (theStoreItem == STORE_ITEM_ROOF_CLEANER)
    {
        return aTrialStageLocked || aCurrentLevel < 42;
    }
    else if (theStoreItem == STORE_ITEM_PLANT_GLOOMSHROOM || theStoreItem == STORE_ITEM_PLANT_CATTAIL)
    {
        return aTrialStageLocked || aCurrentLevel < 35;
    }
    else if (theStoreItem == STORE_ITEM_PLANT_SPIKEROCK || theStoreItem == STORE_ITEM_PLANT_GOLD_MAGNET)
    {
        return aCurrentLevel < 41;
    }

    return 
        theStoreItem != STORE_ITEM_PLANT_WINTERMELON && 
        theStoreItem != STORE_ITEM_PLANT_COBCANNON &&
        theStoreItem != STORE_ITEM_PLANT_IMITATER && 
        theStoreItem != STORE_ITEM_FIRSTAID;
    */

    if (theStoreItem == STORE_ITEM_ROOF_CLEANER)
    {
        return mApp->IsTrialStageLocked() || (!mApp->HasFinishedAdventure() && mApp->mPlayerInfo->GetLevel() < 42);
    }
    if (theStoreItem == STORE_ITEM_PLANT_GLOOMSHROOM)
    {
        return mApp->IsTrialStageLocked() || (!mApp->HasFinishedAdventure() && mApp->mPlayerInfo->GetLevel() < 35);
    }
    if (theStoreItem == STORE_ITEM_PLANT_CATTAIL)
    {
        return mApp->IsTrialStageLocked() || (!mApp->HasFinishedAdventure() && mApp->mPlayerInfo->GetLevel() < 35);
    }
    if (theStoreItem == STORE_ITEM_PLANT_SPIKEROCK)
    {
        return !mApp->HasFinishedAdventure() && mApp->mPlayerInfo->GetLevel() < 41;
    }
    if (theStoreItem == STORE_ITEM_PLANT_GOLD_MAGNET)
    {
        return !mApp->HasFinishedAdventure() && mApp->mPlayerInfo->GetLevel() < 41;
    }
    if (theStoreItem == STORE_ITEM_PLANT_WINTERMELON || theStoreItem == STORE_ITEM_PLANT_COBCANNON ||
        theStoreItem == STORE_ITEM_PLANT_IMITATER || theStoreItem == STORE_ITEM_FIRSTAID)
    {
        return !mApp->HasFinishedAdventure();
    }
    return false;
}

void StoreScreen::GetStorePosition(int theSpotIndex, int& thePosX, int& thePosY)
{
    if (theSpotIndex <= 3)
    {
        thePosX = STORESCREEN_ITEMOFFSET_1_X + STORESCREEN_ITEMSIZE * theSpotIndex;
        thePosY = STORESCREEN_ITEMOFFSET_1_Y;
    }
    else
    {
        thePosX = STORESCREEN_ITEMOFFSET_2_X + STORESCREEN_ITEMSIZE * (theSpotIndex - 4);
        thePosY = STORESCREEN_ITEMOFFSET_2_Y;
    }
}

//0x48AC50
void StoreScreen::DrawItemIcon(Graphics* g, int theItemPosition, StoreItem theItemType, bool theIsForHighlight)
{
    if (theIsForHighlight)
    {
        g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
        g->SetColor(Color(255, 255, 255, 96));
        g->SetColorizeImages(true);
    }

    int aPosX, aPosY;
    GetStorePosition(theItemPosition, aPosX, aPosY);

    if (mPage != 0)
    {
        DrawSeedPacket(g, aPosX, aPosY, SEED_AP_OFFWORLD_ITEM, SEED_NONE, 0, 255, false, false, mApp);
    } 
    else
    {
        if (theItemType == STORE_ITEM_PACKET_UPGRADE)
        {
            g->SetColor(Color(255, 255, 255, 32));
            g->DrawImage(Sexy::IMAGE_STORE_PACKETUPGRADE, aPosX - 7, aPosY + 7);
            if (theIsForHighlight)
            {
                g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
                g->SetColorizeImages(false);
            }
        
            SexyString aSlotText = TodReplaceNumberString(_S("[STORE_UPGRADE_SLOTS]"), _S("{SLOTS}"), min(mApp->mPlayerInfo->mPurchases[STORE_ITEM_PACKET_UPGRADE] + 7, 10));
            Rect aRect(aPosX, aPosY + 6, 55, 70);
            TodDrawStringWrapped(g, aSlotText, aRect, Sexy::FONT_HOUSEOFTERROR16, Color::White, DS_ALIGN_CENTER_VERTICAL_MIDDLE);
        }
        else if (theItemType == STORE_ITEM_POOL_CLEANER)
        {
            g->DrawImage(Sexy::IMAGE_ICON_POOLCLEANER, aPosX + 1, aPosY + 7);
        }
        else if (theItemType == STORE_ITEM_RAKE)
        {
            g->DrawImage(Sexy::IMAGE_ICON_RAKE, aPosX - 5, aPosY + 10);
        }
        else if (theItemType == STORE_ITEM_ROOF_CLEANER)
        {
            g->DrawImage(Sexy::IMAGE_ICON_ROOFCLEANER, aPosX, aPosY + 28);
        }
        else if (theItemType == STORE_ITEM_PLANT_IMITATER)
        {
            g->DrawImage(Sexy::IMAGE_IMITATERSEED, aPosX, aPosY);
        }
        else if (theItemType == STORE_ITEM_MUSHROOM_GARDEN)
        {
            g->DrawImage(Sexy::IMAGE_STORE_MUSHROOMGARDENICON, aPosX - 8, aPosY + 2);
        }
        else if (theItemType == STORE_ITEM_AQUARIUM_GARDEN)
        {
            g->DrawImage(Sexy::IMAGE_STORE_AQUARIUMGARDENICON, aPosX - 8, aPosY + 2);
        }
        else if (theItemType == STORE_ITEM_TREE_OF_WISDOM)
        {
            g->DrawImage(Sexy::IMAGE_STORE_TREEOFWISDOMICON, aPosX - 8, aPosY + 2);
        }
        else if (theItemType == STORE_ITEM_FIRSTAID)
        {
            g->DrawImage(Sexy::IMAGE_STORE_FIRSTAIDWALLNUTICON, aPosX - 1, aPosY + 13);
        }
        else if (theItemType == STORE_ITEM_PVZ)
        {
            g->DrawImage(Sexy::IMAGE_STORE_PVZICON, aPosX, aPosY - 9);
        }
        else if (theItemType == STORE_ITEM_TREE_FOOD)
        {
            g->DrawImage(Sexy::IMAGE_TREEFOOD, aPosX - 8, aPosY - 2);
        }
        else if (theItemType == STORE_ITEM_STINKY_THE_SNAIL)
        {
            g->DrawImage(Sexy::IMAGE_REANIM_STINKY_TURN3, aPosX - 24, aPosY + 14);
        }
        else if (theItemType == STORE_ITEM_GOLD_WATERINGCAN)
        {
            g->DrawImage(Sexy::IMAGE_WATERINGCANGOLD, aPosX - 14, aPosY - 4);
        }
        else if (theItemType == STORE_ITEM_FERTILIZER)
        {
            g->DrawImage(Sexy::IMAGE_FERTILIZER, aPosX - 11, aPosY - 2);
            TodDrawString(g, _S("x5"), aPosX + 56, aPosY + 62, Sexy::FONT_HOUSEOFTERROR16, Color::White, DS_ALIGN_RIGHT);
        }
        else if (theItemType == STORE_ITEM_PHONOGRAPH)
        {
            g->DrawImage(Sexy::IMAGE_PHONOGRAPH, aPosX - 12, aPosY + 3);
        }
        else if (theItemType == STORE_ITEM_BUG_SPRAY)
        {
            g->DrawImage(Sexy::IMAGE_BUG_SPRAY, aPosX - 12, aPosY + 3);
            TodDrawString(g, _S("x5"), aPosX + 56, aPosY + 62, Sexy::FONT_HOUSEOFTERROR16, Color::White, DS_ALIGN_RIGHT);
        }
        else if (theItemType == STORE_ITEM_GARDENING_GLOVE)
        {
            g->DrawImage(Sexy::IMAGE_ZEN_GARDENGLOVE, aPosX - 12, aPosY + 3);
        }
        else if (theItemType == STORE_ITEM_WHEEL_BARROW)
        {
            g->DrawImage(Sexy::IMAGE_ZEN_WHEELBARROW, aPosX - 12, aPosY + 3);
        }
        else if (IsPottedPlant(theItemType))
        {
            mApp->mZenGarden->DrawPottedPlantIcon(g, aPosX, aPosY, &mPottedPlantSpecs);
        }
        else
        {
            DrawSeedPacket(g, aPosX, aPosY, (SeedType)(theItemType + 40), SEED_NONE, 0, 255, false, false, mApp);
        }
    }

    g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
    g->SetColorizeImages(false);
}

//0x48B170
void StoreScreen::DrawItem(Graphics* g, int theItemPosition, StoreItem theItemType)
{
    if (IsItemUnavailable(theItemType))
        return;

    DrawItemIcon(g, theItemPosition, theItemType, false);

    int aPosX, aPosY;
    GetStorePosition(theItemPosition, aPosX, aPosY);
    if (theItemType != STORE_ITEM_PVZ)
    {
        g->DrawImage(Sexy::IMAGE_STORE_PRICETAG, aPosX - 3, aPosY + 70);
        SexyString aCostString = LawnApp::GetMoneyString(GetItemCost(theItemPosition));
        TodDrawString(g, aCostString, aPosX + 23, aPosY + 85, Sexy::FONT_BRIANNETOD12, Color::Black, DS_ALIGN_CENTER);
    }
    if (IsComingSoon(theItemType))
    {
        Rect aRect(aPosX, aPosY, 60, 70);
        if (theItemType == STORE_ITEM_PLANT_TWINSUNFLOWER || theItemType == STORE_ITEM_PACKET_UPGRADE)
        {
            aRect.mX -= 4;
        }
        TodDrawStringWrapped(g, _S("[COMING_SOON]"), aRect, Sexy::FONT_HOUSEOFTERROR16, Color(255, 0, 0),
                             DS_ALIGN_CENTER_VERTICAL_MIDDLE);
    }
    else if (IsItemSoldOut(theItemPosition))
    {
        Rect aRect(aPosX, aPosY, 50, 70);
        TodDrawStringWrapped(g, _S("[SOLD_OUT]"), aRect, Sexy::FONT_HOUSEOFTERROR16, Color(255, 0, 0),
                             DS_ALIGN_CENTER_VERTICAL_MIDDLE);
    }
    else if (mMouseOverItem == theItemPosition)
    {
        if (theItemType >= 0 && theItemType <= 8)
        {
            g->DrawImage(Sexy::IMAGE_SEEDPACKETFLASH, aPosX, aPosY);
        }
        else
        {
            DrawItemIcon(g, theItemPosition, theItemType, true);
        }
    }
}

//0x48B4C0
void StoreScreen::Draw(Graphics* g)
{
    g->SetLinearBlend(true);
    mDrawnOnce = true;

    int aStoreSignPosY = TodAnimateCurve(50, 110, mStoreTime, -150, 0, CURVE_EASE_IN_OUT);
    if (mApp->IsNight())
    {
        g->DrawImage(Sexy::IMAGE_STORE_BACKGROUNDNIGHT, 0, 0);
        g->DrawImage(Sexy::IMAGE_STORE_BACKGROUNDNIGHT, BOARD_WIDTH, 0);
    }
    else
    {
        g->DrawImage(Sexy::IMAGE_STORE_BACKGROUND, 0, 0);
        g->DrawImage(Sexy::IMAGE_STORE_BACKGROUND, BOARD_WIDTH, 0);
    }

    if (!mHatchTimer && mHatchOpen)
    {
        g->DrawImage(Sexy::IMAGE_STORE_CAR, mShakeX + 196, mShakeY + 138);
        g->DrawImage(Sexy::IMAGE_STORE_HATCHBACKOPEN, mShakeX + 299, mShakeY);
        if (mApp->IsNight())
        {
            g->DrawImage(Sexy::IMAGE_STORE_CAR_NIGHT, mShakeX + 688 - 20, mShakeY + 193);
        }
    }
    else
    {
        g->DrawImage(Sexy::IMAGE_STORE_CARCLOSED, mShakeX + 196, mShakeY + 138);
        if (mApp->IsNight())
        {
            g->DrawImage(Sexy::IMAGE_STORE_CAR_NIGHT, mShakeX + 688 - 20, mShakeY + 193);
            g->DrawImage(Sexy::IMAGE_STORE_CARCLOSED_NIGHT, mShakeX + 337, mShakeY + 187);
        }
    }
    g->DrawImage(Sexy::IMAGE_STORE_SIGN, 285, aStoreSignPosY);

    auto energy_shake_x = 0;
    auto energy_shake_y = 0;
    if (mZapTickCounter > 490)
    {
        energy_shake_x = -10;
        energy_shake_y = -10;
    }
    else if (mZapTickCounter > 480)
    {
        energy_shake_x = -5;
        energy_shake_y = 5;
    }
    else if (mZapTickCounter > 470)
    {
        energy_shake_x = 10;
        energy_shake_y = -5;
    }
    else if (mZapTickCounter > 460)
    {
        energy_shake_x = 5;
        energy_shake_y = 10;
    }

    // Draw the e-ATM
    g->DrawImage(mZapTickCounter > 0 ? Sexy::IMAGE_E_ATM_ZAP : Sexy::IMAGE_E_ATM, BOARD_WIDTH + 330 + energy_shake_x,
                 -170 + energy_shake_y);

    Graphics gBackButton(*g);
    gBackButton.mTransX = mBackButton->mX + mApp->mDDInterface->mWideScreenOffsetX;
    gBackButton.mTransY = mBackButton->mY + mApp->mDDInterface->mWideScreenOffsetY;
    mBackButton->Render(&gBackButton);

    Graphics gPrevButton(*g);
    gPrevButton.mTransX = mPrevButton->mX + mApp->mDDInterface->mWideScreenOffsetX;
    gPrevButton.mTransY = mPrevButton->mY + mApp->mDDInterface->mWideScreenOffsetY;
    mPrevButton->Render(&gPrevButton);

    Graphics gNextButton(*g);
    gNextButton.mTransX = mNextButton->mX + mApp->mDDInterface->mWideScreenOffsetX;
    gNextButton.mTransY = mNextButton->mY + mApp->mDDInterface->mWideScreenOffsetY;
    mNextButton->Render(&gNextButton);

    if (mApp->mSlotData && mApp->mSlotData->energylink_enabled())
    {
        Graphics gEnergyButton(*g);
        gEnergyButton.mTransX = mEnergyButton->mX + mApp->mDDInterface->mWideScreenOffsetX;
        gEnergyButton.mTransY = mEnergyButton->mY + mApp->mDDInterface->mWideScreenOffsetY;
        mEnergyButton->Render(&gEnergyButton);
    }

    Graphics gEnergyBackButton(*g);
    gEnergyBackButton.mTransX = mEnergyBackButton->mX + mApp->mDDInterface->mWideScreenOffsetX;
    gEnergyBackButton.mTransY = mEnergyBackButton->mY + mApp->mDDInterface->mWideScreenOffsetY;
    mEnergyBackButton->Render(&gEnergyBackButton);

    switch (mEnergyMode)
    {
    case Idle:
        {
            Graphics gDepositButton(*g);
            gDepositButton.mTransX = mDepositButton->mX + mApp->mDDInterface->mWideScreenOffsetX + energy_shake_x;
            gDepositButton.mTransY = mDepositButton->mY + mApp->mDDInterface->mWideScreenOffsetY + energy_shake_y;
            mDepositButton->Render(&gDepositButton);

            Graphics gWithdrawButton(*g);
            gWithdrawButton.mTransX = mWithdrawButton->mX + mApp->mDDInterface->mWideScreenOffsetX + energy_shake_x;
            gWithdrawButton.mTransY = mWithdrawButton->mY + mApp->mDDInterface->mWideScreenOffsetY + energy_shake_y;
            mWithdrawButton->Render(&gWithdrawButton);
            break;
        }
    case Deposit:
    case Withdraw:
        {
            Graphics gBackButton(*g);
            gBackButton.mTransX = mEnergyAtmBackButton->mX + mApp->mDDInterface->mWideScreenOffsetX + energy_shake_x;
            gBackButton.mTransY = mEnergyAtmBackButton->mY + mApp->mDDInterface->mWideScreenOffsetY + energy_shake_y;
            mEnergyAtmBackButton->Render(&gBackButton);

            Graphics gLessButton(*g);
            gLessButton.mTransX = mEnergyLessButton->mX + mApp->mDDInterface->mWideScreenOffsetX + energy_shake_x;
            gLessButton.mTransY = mEnergyLessButton->mY + mApp->mDDInterface->mWideScreenOffsetY + energy_shake_y;
            mEnergyLessButton->Render(&gLessButton);

            Graphics gMoreButton(*g);
            gMoreButton.mTransX = mEnergyMoreButton->mX + mApp->mDDInterface->mWideScreenOffsetX + energy_shake_x;
            gMoreButton.mTransY = mEnergyMoreButton->mY + mApp->mDDInterface->mWideScreenOffsetY + energy_shake_y;
            mEnergyMoreButton->Render(&gMoreButton);

            Graphics gGoButton(*g);
            gGoButton.mTransX = mEnergyGoButton->mX + mApp->mDDInterface->mWideScreenOffsetX + energy_shake_x;
            gGoButton.mTransY = mEnergyGoButton->mY + mApp->mDDInterface->mWideScreenOffsetY + energy_shake_y;
            mEnergyGoButton->Render(&gGoButton);

            auto coinY = 300;
            auto nrgY = 345;
            SexyString aCoinLabel = "$$$";
            SexyString aEnergyLabel = "EEE";

            aCoinLabel = mApp->GetMoneyString(mPendingEnergyTransaction);

            if (mEnergyMode == Deposit)
            {
                aEnergyLabel = mApp->GetEnergyString(mPendingEnergyTransaction * EnergyLinkExchangeDeposit);
            }
            else
            {
                aEnergyLabel = "-" + mApp->GetEnergyString(mPendingEnergyTransaction * EnergyLinkExchangeWithdraw);
            }

            g->DrawImage(Sexy::IMAGE_COINBANK, BOARD_WIDTH + 430 + energy_shake_x, coinY + energy_shake_y);
            g->SetColor(Color(180, 255, 90));
            g->SetFont(Sexy::FONT_CONTINUUMBOLD14);
            g->DrawString(
                aCoinLabel,
                BOARD_WIDTH + 430 + 7 + 111 - Sexy::FONT_CONTINUUMBOLD14->StringWidth(aCoinLabel) + energy_shake_x,
                coinY + 24 + energy_shake_y);

            g->DrawImage(Sexy::IMAGE_NRG_BANK, BOARD_WIDTH + 430 + energy_shake_x, nrgY + energy_shake_y);
            g->SetColor(Color(180, 255, 90));
            g->SetFont(Sexy::FONT_CONTINUUMBOLD14);
            g->DrawString(aEnergyLabel,
                          BOARD_WIDTH + 430 + 7 + 111 - Sexy::FONT_CONTINUUMBOLD14->StringWidth(aEnergyLabel) +
                          energy_shake_x, nrgY + 24 + energy_shake_y);

            break;
        }
    }

    if (!mHatchTimer && mHatchOpen)
    {
        for (int i = 0; i < MAX_PAGE_SPOTS; i++)
        {
            StoreItem aStoreItem = GetStoreItemType(i);
            if (aStoreItem != STORE_ITEM_INVALID)
            {
                DrawItem(g, i, aStoreItem);
            }
        }
    }

    g->DrawImage(Sexy::IMAGE_COINBANK, STORESCREEN_COINBANK_X - mX, STORESCREEN_COINBANK_Y);
    g->SetColor(Color(180, 255, 90));
    g->SetFont(Sexy::FONT_CONTINUUMBOLD14);
    SexyString aCoinLabel = mApp->GetMoneyString(mApp->mPlayerInfo->mCoins);
    g->DrawString(
        aCoinLabel, STORESCREEN_COINBANK_X - mX + 7 + 111 - Sexy::FONT_CONTINUUMBOLD14->StringWidth(aCoinLabel),
        STORESCREEN_COINBANK_Y + 24);

    if (mApp->mAP->ConnectionStatus() == APWrapper::ConnectionStatus::Connected)
    {
        auto elBalance = mApp->mAP->ReadDataStorage(
            mApp->mAP->DataStorageSlot(APWrapper::KnownDataStorageKey::EnergyLink)).get<uint64_t>();
        // Draw EnergyLink label
        g->DrawImage(Sexy::IMAGE_NRG_BANK, STORESCREEN_COINBANK_X + BOARD_WIDTH, STORESCREEN_COINBANK_Y - 30);
        g->SetColor(Color(180, 255, 90));
        g->SetFont(Sexy::FONT_CONTINUUMBOLD14);
        SexyString aEnergyLinkBalance = mApp->GetEnergyString(elBalance);
        g->DrawString(aEnergyLinkBalance,
                      STORESCREEN_COINBANK_X + BOARD_WIDTH + 7 + 111 - Sexy::FONT_CONTINUUMBOLD14->
                      StringWidth(aEnergyLinkBalance), STORESCREEN_COINBANK_Y + 24 - 30);
    }

    if (!mPrevButton->mDisabled)
    {
        // int aNumPages = 0;
        // for (StorePages aPage = STORE_PAGE_SLOT_UPGRADES; aPage < NUM_STORE_PAGES; aPage = (StorePages)(aPage + 1))
        // {
        //     if (IsPageShown(aPage))
        //     {
        //         aNumPages++;
        //     }
        // }
        int aNumPages = this->AvailableRestocks();
        int mPageOffset = -1;
        if (mApp->mAP->ReceivedItemCount(PVZRAPData::Items::ZEN_GARDEN) > 0)
        {
            aNumPages++;
            mPageOffset = 0;
        }

        SexyString aPageString = TodReplaceNumberString(
            TodReplaceNumberString(_S("[STORE_PAGE]"), _S("{PAGE}"), mPage + 1 + mPageOffset), _S("{NUM_PAGES}"), aNumPages);
        TodDrawString(g, aPageString, STORESCREEN_PAGESTRING_X, STORESCREEN_COINBANK_Y - 60, Sexy::FONT_BRIANNETOD12,
                      Color(125, 125, 125), DS_ALIGN_CENTER);
    }

    Graphics gCrazyDave = Graphics(*g);
    gCrazyDave.mTransX -= 42.0f - mCrazyDaveOffset;
    gCrazyDave.mTransY += 68.0f;
    mApp->DrawCrazyDave(&gCrazyDave);
    
    if (!mAdmonition->IsDead())
    {
        mAdmonition->Draw(g);
    }
}

//0x48BA30
void StoreScreen::DrawOverlay(Graphics* g)
{
    Coin* aCoin = nullptr;
    while (mCoins.IterateNext(aCoin))
    {
        if (!aCoin->mDead)
        {
            aCoin->Draw(g);
        }
    }
}

//0x48BAA0
// GOTY @Patoke: 0x4578F0
void StoreScreen::SetBubbleText(int theCrazyDaveMessage, int theTime, bool theClickToContinue)
{
    mApp->CrazyDaveTalkIndex(theCrazyDaveMessage);
    mBubbleCountDown = theTime;
    mBubbleClickToContinue = theClickToContinue;
}

void StoreScreen::SetBubbleText(std::string message, int theTime, bool theClickToContinue)
{
    mApp->CrazyDaveTalkMessage(message);
    mBubbleCountDown = theTime;
    mBubbleClickToContinue = theClickToContinue;
}

//0x48BAD0
void StoreScreen::UpdateMouse()
{
    auto oldMouseOverItem = mMouseOverItem;
    mMouseOverItem = -1;
    mAdmonition->Die();
    if (mStoreTime < 120 || mBubbleClickToContinue || mHatchTimer > 0 || mWaitForDialog || mCrazyDaveLastTalkIndex != -
        1) return;
    int aMouseX = mApp->mWidgetManager->mLastMouseX - mX, aMouseY = mApp->mWidgetManager->mLastMouseY - mY;
    bool aShowFinger = false;
    for (int aItemPos = 0; aItemPos < MAX_PAGE_SPOTS; aItemPos++)
    {
        StoreItem aItemType = GetStoreItemType(aItemPos);
        if (aItemType != STORE_ITEM_INVALID && !IsItemUnavailable(aItemType))
        {
            int aItemX, aItemY;
            GetStorePosition(aItemPos, aItemX, aItemY);
            auto itemRect = Rect(aItemX, aItemY, 50, 87);
            if (itemRect.Contains(aMouseX, aMouseY))
            {
                mMouseOverItem = aItemPos;
                int aMessageIndex = -1;
                std::string message;
                
                switch (aItemType)
                {
                case STORE_ITEM_PLANT_GATLINGPEA:       aMessageIndex = 2000;                           break;
                case STORE_ITEM_PLANT_TWINSUNFLOWER:    aMessageIndex = 2001;                           break;
                case STORE_ITEM_PLANT_GLOOMSHROOM:      aMessageIndex = 2002;                           break;
                case STORE_ITEM_PLANT_CATTAIL:          aMessageIndex = 2003;                           break;
                case STORE_ITEM_PLANT_WINTERMELON:      aMessageIndex = 2004;                           break;
                case STORE_ITEM_PLANT_GOLD_MAGNET:      aMessageIndex = 2005;                           break;
                case STORE_ITEM_PLANT_SPIKEROCK:        aMessageIndex = 2006;                           break;
                case STORE_ITEM_PLANT_COBCANNON:        aMessageIndex = 2007;                           break;
                case STORE_ITEM_PLANT_IMITATER:         aMessageIndex = 2008;                           break;
                case STORE_ITEM_BONUS_LAWN_MOWER:       aMessageIndex = 2009;                           break;
                case STORE_ITEM_POTTED_MARIGOLD_1:
                case STORE_ITEM_POTTED_MARIGOLD_2:
                case STORE_ITEM_POTTED_MARIGOLD_3:      aMessageIndex = 2010;                           break;
                case STORE_ITEM_GOLD_WATERINGCAN:       aMessageIndex = 2019;                           break;
                case STORE_ITEM_FERTILIZER:             aMessageIndex = 2020;                           break;
                case STORE_ITEM_BUG_SPRAY:              aMessageIndex = 2022;                           break;
                case STORE_ITEM_PHONOGRAPH:             aMessageIndex = 2021;                           break;
                case STORE_ITEM_GARDENING_GLOVE:        aMessageIndex = 2023;                           break;
                case STORE_ITEM_MUSHROOM_GARDEN:        aMessageIndex = 2032;                           break;
                case STORE_ITEM_WHEEL_BARROW:           aMessageIndex = 2024;                           break;
                case STORE_ITEM_STINKY_THE_SNAIL:       aMessageIndex = 2025;                           break;
                case STORE_ITEM_PACKET_UPGRADE:
                    if (mApp->mPlayerInfo->mPurchases[STORE_ITEM_PACKET_UPGRADE] < 4)
                        aMessageIndex = mApp->mPlayerInfo->mPurchases[STORE_ITEM_PACKET_UPGRADE] + 2011;
                    else
                        aMessageIndex = 2014;                                                           break;
                case STORE_ITEM_POOL_CLEANER:           aMessageIndex = 2026;                           break;
                case STORE_ITEM_ROOF_CLEANER:           aMessageIndex = 2027;                           break;
                case STORE_ITEM_RAKE:                   aMessageIndex = 2028;                           break;
                case STORE_ITEM_AQUARIUM_GARDEN:        aMessageIndex = 2029;                           break;
                case STORE_ITEM_CHOCOLATE:                                                              break;
                case STORE_ITEM_TREE_OF_WISDOM:         aMessageIndex = 2030;                           break;
                case STORE_ITEM_TREE_FOOD:              aMessageIndex = 2031;                           break;
                case STORE_ITEM_FIRSTAID:               aMessageIndex = 2033;                           break;
                case STORE_ITEM_PVZ:                    aMessageIndex = 2034;
                case STORE_ITEM_AP:
                    {
                        auto item = mApp->mAP->ItemAtLocation(PVZRAPData::Locations::Twiddydinkie(aItemPos + (mPage - 1) * 8));
                        
                        const std::string* string_bank;
                        std::string item_class;
                        if (item.flags & APItem::ITEM_FLAG_PROGRESSION)
                        {
                            string_bank = progression_bank;
                            item_class = "Progression";
                        }
                        else if (item.flags & APItem::ITEM_FLAG_USEFUL)
                        {
                            string_bank = useful_bank;
                            item_class = "Useful";
                        }
                        else if (item.flags & APItem::ITEM_FLAG_TRAP)
                        {
                            string_bank = trap_bank;
                            item_class = "Trap";
                        }
                        else
                        {
                            string_bank = standard_bank;
                            item_class = "Filler";
                        }
                        
                        std::string crazyDaveDialogue;
                        auto playerName = mApp->mAP->PlayerDisplayName(item.player);
                        auto gameName = mApp->mAP->PlayerGameName(item.player);
                        auto itemName = mApp->mAP->ItemName(item);
                        auto customDialogue = mApp->mAPDialogueClient->GetCrazyDaveDialogue(gameName, itemName, playerName);
                        if (customDialogue.has_value())
                        {
                            crazyDaveDialogue = customDialogue.value();
                        }
                        else
                        {
                            crazyDaveDialogue = string_bank[Rand(4)];
                        }


                        message = crazyDaveDialogue + "{NO_CLICK}";
                        mAdmonition->SetText(itemName + " for " + playerName + " (" + item_class + ")");
                        mAdmonition->SetArea(itemRect);
                        mAdmonition->timer = 1;
                        
                        break;
                    }
                }

                if (oldMouseOverItem != aItemPos)
                    if (aMessageIndex == -1)
                    {
                        SetBubbleText(message, 100, false);
                    }
                    else
                    {
                        SetBubbleText(aMessageIndex, 100, false);
                    }
                else mBubbleCountDown = 100;
                if (IsFullVersionOnly(aItemType) || (!IsItemSoldOut(aItemType) && !IsItemUnavailable(aItemType) && !
                    IsComingSoon(aItemType)))
                    aShowFinger = true;
            }
        }
    }

    if (mApp->mWidgetManager->mOverWidget && !mInert)
        mApp->SetCursor(
            mBackButton->mIsOver || mPrevButton->mIsOver || mNextButton->mIsOver || mEnergyButton->mIsOver ||
            mEnergyBackButton->mIsOver ||
            (mEnergyMode == Idle && (mWithdrawButton->mIsOver || mDepositButton->mIsOver)) ||
            (mEnergyMode != Idle && (mEnergyAtmBackButton->mIsOver || mEnergyLessButton->mIsOver || mEnergyMoreButton->
                mIsOver || mEnergyGoButton->mIsOver)) || aShowFinger
                ? CURSOR_HAND
                : CURSOR_POINTER);
}

//0x48BE30
void StoreScreen::StorePreload()
{
    ReanimatorEnsureDefinitionLoaded(REANIM_CRAZY_DAVE, true);
    ReanimatorEnsureDefinitionLoaded(REANIM_ZENGARDEN_FERTILIZER, true);
    mApp->CrazyDaveEnter();

    Plant::PreloadPlantResources(SeedType::SEED_GATLINGPEA); // Garlic this is so random LOL
    Plant::PreloadPlantResources(SeedType::SEED_TWINSUNFLOWER);

    if (mApp->HasFinishedAdventure())
    {
        Plant::PreloadPlantResources(SeedType::SEED_GLOOMSHROOM);
        Plant::PreloadPlantResources(SeedType::SEED_CATTAIL);
        Plant::PreloadPlantResources(SeedType::SEED_WINTERMELON);
        Plant::PreloadPlantResources(SeedType::SEED_GOLD_MAGNET);
        Plant::PreloadPlantResources(SeedType::SEED_SPIKEROCK);
        Plant::PreloadPlantResources(SeedType::SEED_COBCANNON);
        Plant::PreloadPlantResources(SeedType::SEED_IMITATER);
    }
}

bool StoreScreen::CanInteractWithButtons()
{
    return mStoreTime >= 120 && !mBubbleClickToContinue && mHatchTimer <= 0 && !mWaitForDialog;
}

//0x48BF60
void StoreScreen::Update()
{
    // @Patoke: implemented this
    if (mSlideCounter > 0)
    {
        int aNewX = TodAnimateCurve(75, 0, mSlideCounter, mStartX, mDestX, TodCurves::CURVE_EASE_IN_OUT);
        int aNewY = TodAnimateCurve(75, 0, mSlideCounter, mStartY, mDestY, TodCurves::CURVE_EASE_IN_OUT);
        Move(aNewX, aNewY);

        // @Patoke: not from the original binaries but fixes bugs
        mOverlayWidget->Move(aNewX, aNewY);
        mBackButton->Move(aNewX + BackButtonOffset, mBackButton->mY);
        mPrevButton->Move(aNewX + PrevButtonOffset, mPrevButton->mY);
        mNextButton->Move(aNewX + NextButtonOffset, mNextButton->mY);
        mEnergyButton->Move(aNewX + EnergyButtonOffset, mEnergyButton->mY);
        mEnergyBackButton->SetOffset(aNewX, aNewY);
        mDepositButton->SetOffset(aNewX, aNewY);
        mWithdrawButton->SetOffset(aNewX, aNewY);
        mEnergyAtmBackButton->SetOffset(aNewX, aNewY);
        mEnergyLessButton->SetOffset(aNewX, aNewY);
        mEnergyMoreButton->SetOffset(aNewX, aNewY);
        mEnergyGoButton->SetOffset(aNewX, aNewY);
        mCrazyDaveOffset = -aNewX;
        // TODO: mark stuff dirty

        mSlideCounter--;

        if (mSlideCounter == 0 && mCurrentScreen == EnergyScreen)
        {
            SetBubbleText("{SHOW_AP_OFFWORLD}I can sell you energy drinks to put in the energy machine!", 100, false);
        }
    }

    if (mZapTickCounter > 0)
    {
        mZapTickCounter--;
    }

    // For some reason we stop polling in the shop so poll here
    mApp->mAP->Poll();
    if (!mInert)
    {
        mApp->mMusic->MakeSureMusicIsPlaying(MUSIC_TUNE_TITLE_CRAZY_DAVE_MAIN_THEME);
    }
    mApp->UpdateCrazyDave();

    // 更新 DataArray<Coin> 中的所有 Coin
    Coin* aCoin = nullptr;
    while (mCoins.IterateNext(aCoin))
    {
        if (!aCoin->mDead)
        {
            aCoin->Update();
        }
    }

    if (mWaitForDialog)
        return;

    if (mApp->mCrazyDaveState == CRAZY_DAVE_OFF)
    {
        if (mDrawnOnce)
        {
            StorePreload();
        }
        return;
    }

    mStoreTime++;
    if (mApp->mCrazyDaveState != CRAZY_DAVE_OFF && mApp->mCrazyDaveState != CRAZY_DAVE_ENTERING)
    {
        if (mHatchTimer > 0)
        {
            mHatchTimer--;
            mBackButton->mX -= mShakeX;
            mBackButton->mY -= mShakeY;
            mPrevButton->mX -= mShakeX;
            mPrevButton->mY -= mShakeY;
            mNextButton->mX -= mShakeX;
            mNextButton->mY -= mShakeY;
            mEnergyButton->mX -= mShakeX;
            mEnergyButton->mY -= mShakeY;
            mEnergyBackButton->mX -= mShakeX;
            mEnergyBackButton->mY -= mShakeY;
            mDepositButton->mX -= mShakeX;
            mDepositButton->mY -= mShakeY;
            mWithdrawButton->mX -= mShakeX;
            mWithdrawButton->mY -= mShakeY;
            mEnergyAtmBackButton->mX -= mShakeX;
            mEnergyAtmBackButton->mY -= mShakeY;
            mEnergyLessButton->mX -= mShakeX;
            mEnergyLessButton->mY -= mShakeY;
            mEnergyMoreButton->mX -= mShakeX;
            mEnergyMoreButton->mY -= mShakeY;
            mEnergyGoButton->mX -= mShakeX;
            mEnergyGoButton->mY -= mShakeY;

            /*
            if (mHatchTimer <= 35)
            {
                if (mHatchTimer == 0)
                {
                    EnableButtons(true);
                }
                mShakeY = 0;
            }
            else
            {
                mShakeY = RandRangeInt(1, 3);
            }
            mShakeX = 0;
            */

            if (mHatchTimer == 0)
            {
                EnableButtons(true);
                mShakeX = 0;
                mShakeY = 0;
        
                ScoutPage();
            }
            else
            {
                mShakeX = 0;
                if (mHatchTimer > 35)
                {
                    mShakeY = RandRangeInt(1, 3);
                }
                else
                {
                    mShakeY = 0;
                }
            }

            mBackButton->mX += mShakeX;
            mBackButton->mY += mShakeY;
            mPrevButton->mX += mShakeX;
            mPrevButton->mY += mShakeY;
            mNextButton->mX += mShakeX;
            mNextButton->mY += mShakeY;
            mEnergyButton->mX += mShakeX;
            mEnergyButton->mY += mShakeY;
            mEnergyBackButton->mX += mShakeX;
            mEnergyBackButton->mY += mShakeY;
            mWithdrawButton->mX += mShakeX;
            mWithdrawButton->mY += mShakeY;
            mDepositButton->mX += mShakeX;
            mDepositButton->mY += mShakeY;
            mEnergyAtmBackButton->mX += mShakeX;
            mEnergyAtmBackButton->mY += mShakeY;
            mEnergyLessButton->mX += mShakeX;
            mEnergyLessButton->mY += mShakeY;
            mEnergyMoreButton->mX += mShakeX;
            mEnergyMoreButton->mY += mShakeY;
            mEnergyGoButton->mX += mShakeX;
            mEnergyGoButton->mY += mShakeY;
        }
        else if (mStartDialog != -1)
        {
            SetBubbleText(mStartDialog, 0, true);
            mStartDialog = -1;
        }
        else if (!mBubbleClickToContinue && !mInert)
        {
            if (mBubbleCountDown > 0)
            {
                mBubbleCountDown--;
                if (mBubbleCountDown == 0)
                {
                    if (mCrazyDaveLastTalkIndex >= 4000 && mCrazyDaveLastTalkIndex < 4004)
                    {
                        mCrazyDaveLastTalkIndex++;
                        SetBubbleText(mCrazyDaveLastTalkIndex, 0, true);
                    }
                    else if (mApp->mSoundSystem->IsFoleyPlaying(FOLEY_CRAZY_DAVE_SHORT) ||
                        mApp->mSoundSystem->IsFoleyPlaying(FOLEY_CRAZY_DAVE_LONG) ||
                        mApp->mSoundSystem->IsFoleyPlaying(FOLEY_CRAZY_DAVE_EXTRA_LONG))
                    {
                        mBubbleCountDown = 1;
                        mCrazyDaveLastTalkIndex = -1;
                    }
                    else
                    {
                        mApp->CrazyDaveStopTalking();
                        mCrazyDaveLastTalkIndex = -1;
                    }
                }
            }
            else
            {
                if (mCurrentScreen == CarScreen)
                {
                    mAmbientSpeechCountDown--;
                    if (mAmbientSpeechCountDown <= 0)
                    {
                        TodWeightedArray aPickArray[4];
                        for (int i = 0; i < 4; i++)
                        {
                            int aMessage = 2015 + i;
                            aPickArray[i].mItem = aMessage;
                            if (mPreviousAmbientSpeechIndex == aMessage)
                            {
                                aPickArray[i].mWeight = 0;
                            }
                            else if (i == 3)
                            {
                                aPickArray[i].mWeight = mApp->HasFinishedAdventure() ? 20 : 0;
                            }
                            else
                            {
                                aPickArray[i].mWeight = 100;
                            }
                        }

                        int aDaveMessage = TodPickFromWeightedArray(aPickArray, 4);
                        mPreviousAmbientSpeechIndex = aDaveMessage;
                        SetBubbleText(aDaveMessage, 800, false);
                        mAmbientSpeechCountDown = RandRangeInt(500, 1000);
                    }
                }
            }
        }
    }

    UpdateMouse();
    // 如果进入商店时为试玩版，而当前为完整版，且可以与按钮进行交互，则可以判断玩家已购买完整版
    if (CanInteractWithButtons() && mTrialLockedWhenStoreOpened && !mApp->IsTrialStageLocked())
    {
        mPurchasedFullVersion = true;
        mResult = Dialog::ID_OK;
    }
    else
    {
        Widget::Update();
        MarkDirty();
    }
}

//0x48C350
void StoreScreen::AddedToManager(WidgetManager* theWidgetManager)
{
    WidgetContainer::AddedToManager(theWidgetManager);
    AddWidget(mBackButton);
    AddWidget(mPrevButton);
    AddWidget(mNextButton);
    if (mApp->mSlotData->energylink_enabled())
    {
        AddWidget(mEnergyButton);
    }
    AddWidget(mEnergyBackButton);
    AddWidget(mDepositButton);
    AddWidget(mWithdrawButton);
    AddWidget(mEnergyAtmBackButton);
    AddWidget(mEnergyMoreButton);
    AddWidget(mEnergyLessButton);
    AddWidget(mEnergyGoButton);
    AddWidget(mOverlayWidget);
}

void StoreScreen::OrderInManagerChanged()
{
    mWidgetManager->PutInfront(mBackButton, this);
    mWidgetManager->PutInfront(mPrevButton, this);
    mWidgetManager->PutInfront(mNextButton, this);
    if (mApp->mSlotData->energylink_enabled())
    {
        mWidgetManager->PutInfront(mEnergyButton, this);
    }
    mWidgetManager->PutInfront(mEnergyBackButton, this);
    mWidgetManager->PutInfront(mDepositButton, this);
    mWidgetManager->PutInfront(mWithdrawButton, this);
    mWidgetManager->PutInfront(mEnergyAtmBackButton, this);
    mWidgetManager->PutInfront(mEnergyLessButton, this);
    mWidgetManager->PutInfront(mEnergyMoreButton, this);
    mWidgetManager->PutInfront(mEnergyGoButton, this);
    mWidgetManager->PutInfront(mOverlayWidget, this);
}

int StoreScreen::ZenPageOffset()
{
    return mApp->mAP->ReceivedItemCount(PVZRAPData::Items::ZEN_GARDEN) > 0 ? 1 : 0;
}

int StoreScreen::AvailableRestocks()
{
    auto progressives = mApp->mAP->ReceivedItemCount(PVZRAPData::Items::PROGRESSIVE_TWIDDYDINKIES);
    if (progressives > 0)
    {
        return progressives;
    }
    
    return mApp->mAP->ReceivedItemCount(PVZRAPData::Items::TWIDDYDINKIES_RESTOCK) + 1;
}

void StoreScreen::ScoutPage()
{
    std::list<int64_t> locations;
    for (int i = 0; i < MAX_PAGE_SPOTS; i++)
    {
        auto twiddydinkie = PVZRAPData::Locations::Twiddydinkie(i + (mPage - 1) * 8);
        if (mApp->mAP->IsLocationPresent(twiddydinkie))
        {
            // Scout this item if required
            locations.push_back(twiddydinkie);
        }
    }
    
    if (!locations.empty())
    {
        mApp->mAP->HintLocations(locations);
    }
}

//0x48C3B0
void StoreScreen::RemovedFromManager(WidgetManager* theWidgetManager)
{
    WidgetContainer::RemovedFromManager(theWidgetManager);
    RemoveWidget(mBackButton);
    RemoveWidget(mPrevButton);
    RemoveWidget(mNextButton);
    if (mApp->mSlotData->energylink_enabled())
    {
        RemoveWidget(mEnergyButton);
    }
    RemoveWidget(mEnergyBackButton);
    RemoveWidget(mDepositButton);
    RemoveWidget(mWithdrawButton);
    RemoveWidget(mEnergyAtmBackButton);
    RemoveWidget(mEnergyLessButton);
    RemoveWidget(mEnergyMoreButton);
    RemoveWidget(mEnergyGoButton);
    RemoveWidget(mOverlayWidget);
    mApp->CrazyDaveDie();
}

//0x48C410
void StoreScreen::ButtonPress(int theId)
{
    if (theId != StoreScreen::StoreScreen_Prev && theId != StoreScreen::StoreScreen_Next)
    {
        mApp->PlaySample(Sexy::SOUND_BUTTONCLICK);
    }

    if (theId == StoreScreen_EnergyLess)
    {
        if (mEnergyMode != Idle)
        {
            if (mPendingEnergyTransaction != 1)
            {
                mPendingEnergyTransaction--;
                mButtonDownTickCounter = 60;
            }
        }
    }
    else if (theId == StoreScreen_EnergyMore)
    {
        if (mEnergyMode != Idle)
        {
            mPendingEnergyTransaction++;
            mButtonDownTickCounter = 60;
        }
    }
}

//0x48C440
bool StoreScreen::IsPageShown(int thePage)
{
    if (thePage == 0)
    {
        return mApp->mAP->ReceivedItemCount(PVZRAPData::Items::ZEN_GARDEN) > 0;
    }
    return this->AvailableRestocks() >= thePage;
    // 试玩模式下，仅显示默认页
    if (mApp->IsTrialStageLocked()) return thePage == STORE_PAGE_SLOT_UPGRADES;
    // 一周目完成后，所有页全解锁
    if (mApp->HasFinishedAdventure()) return true;
    // 到达或已通过冒险模式 5-2 关卡时，显示紫卡页
    if (thePage == STORE_PAGE_PLANT_UPGRADES) return mApp->mPlayerInfo->mLevel >= 42;
    // 到达或已通过冒险模式 5-5 关卡时，显示花园工具页
    if (thePage == STORE_PAGE_ZEN1) return mApp->mPlayerInfo->mLevel >= 45;
    // 冒险模式未完成时，不显示智慧树工具页
    return thePage != STORE_PAGE_ZEN2;
}

//0x48C4D0
void StoreScreen::ButtonDepress(int theId)
{
    if (theId == StoreScreen::StoreScreen_Back)
        mResult = 1000;
    else if (theId == StoreScreen::StoreScreen_Prev || theId == StoreScreen::StoreScreen_Next)
    {
        mHatchTimer = 50;
        mApp->PlaySample(Sexy::SOUND_HATCHBACK_CLOSE);
        mBubbleCountDown = 0;
        mApp->CrazyDaveStopTalking();
        EnableButtons(false);
        do
        {
            if (theId == StoreScreen::StoreScreen_Prev)
            {
                mPage = mPage - 1;
                if (mPage < 0)
                {
                    mPage = this->AvailableRestocks();
                }
            }
            else
            {
                mPage = mPage + 1;
                if (mPage > this->AvailableRestocks())
                {
                    mPage = 0;
                }
            }
        }
        while (!IsPageShown(mPage));
    }
    else if (theId == StoreScreen::StoreScreen_Energy)
    {
        mCurrentScreen = EnergyScreen;

        // Change to EnergyLink page
        SlideTo(-mApp->mWidth, 0);

        mApp->CrazyDaveStopTalking();
    }
    else if (theId == StoreScreen::StoreScreen_EnergyBack)
    {
        mCurrentScreen = CarScreen;
        SlideTo(0, 0);
        mAmbientSpeechCountDown = RandRangeInt(500, 1000);

        mApp->CrazyDaveStopTalking();
    }
    else if (theId == StoreScreen_EnergyDeposit)
    {
        if (mEnergyMode == Idle)
        {
            mPendingEnergyTransaction = 1;
            mEnergyMode = Deposit;
        }
    }
    else if (theId == StoreScreen_EnergyWithdraw)
    {
        if (mEnergyMode == Idle)
        {
            mPendingEnergyTransaction = 1;
            mEnergyMode = Withdraw;
        }
    }
    else if (theId == StoreScreen_EnergyAtmBack)
    {
        if (mEnergyMode != Idle)
        {
            mEnergyMode = Idle;
        }
    }
    else if (theId == StoreScreen_EnergyGo)
    {
        if (mEnergyMode != Idle)
        {
            // Perform EnergyLink operation

            uint64_t energy_change;
            int coin_change;
            if (mEnergyMode == Deposit)
            {
                coin_change = -static_cast<int>(mPendingEnergyTransaction);
                energy_change = mPendingEnergyTransaction * EnergyLinkExchangeDeposit;

                if (mApp->mPlayerInfo->mCoins < mPendingEnergyTransaction)
                {
                    // Not enough coins!
                    Dialog* aDialog = mApp->DoDialog(DIALOG_NOT_ENOUGH_MONEY, true, _S("Not enough money"),
                                                     _S("You can't afford to deposit this many coins."),
                                                     _S("[DIALOG_BUTTON_OK]"), BUTTONS_FOOTER);
                    mWaitForDialog = true;
                    aDialog->WaitForResult(true);
                    mWaitForDialog = false;
                    return;
                }

                mApp->mAP->WriteDataStorage(mApp->mAP->DataStorageSlot(APWrapper::KnownDataStorageKey::EnergyLink), 0).
                      add(static_cast<int64_t>(energy_change));
                mApp->mPlayerInfo->AddCoins(coin_change);
                SetBubbleText("Thanks for buying!", 800, false);
                mApp->PlaySample(Sexy::SOUND_THUNDER);
            }
            else
            {
                coin_change = mPendingEnergyTransaction;
                energy_change = mPendingEnergyTransaction * EnergyLinkExchangeWithdraw;

                auto elBalance = mApp->mAP->ReadDataStorage(
                    mApp->mAP->DataStorageSlot(APWrapper::KnownDataStorageKey::EnergyLink)).get<uint64_t>();
                if (elBalance < energy_change)
                {
                    Dialog* aDialog = mApp->DoDialog(DIALOG_NOT_ENOUGH_MONEY, true, _S("Not enough energy"),
                                                     _S(
                                                         "There is not enough energy in the EnergyLink system to withdraw that many coins."),
                                                     _S("[DIALOG_BUTTON_OK]"), BUTTONS_FOOTER);
                    mWaitForDialog = true;
                    aDialog->WaitForResult(true);
                    mWaitForDialog = false;
                    return;
                }

                mApp->mAP->WriteDataStorage(mApp->mAP->DataStorageSlot(APWrapper::KnownDataStorageKey::EnergyLink), 0).
                      add(-static_cast<int64_t>(energy_change));
                mApp->mPlayerInfo->AddCoins(coin_change);

                SetBubbleText("{SHAKE}I'M CRAAAAAAAAZY FOR ENERGY DRINKS!!!{MOUTH_BIG_SMILE}", 800, false);
                mApp->PlaySample(Sexy::SOUND_DIAMOND);
            }

            mZapTickCounter = 500;
            mApp->WriteCurrentUserConfig();
        }
    }
}

void StoreScreen::ButtonDownTick(int theId)
{
    Dialog::ButtonDownTick(theId);
    if (theId == StoreScreen_EnergyLess)
    {
        if (mEnergyMode != Idle)
        {
            if (--mButtonDownTickCounter == 0)
            {
                if (mPendingEnergyTransaction != 1)
                {
                    mPendingEnergyTransaction--;
                }
                mButtonDownTickCounter = 5;
                mApp->PlaySample(Sexy::SOUND_BUTTONCLICK);
            }
        }
    }
    else if (theId == StoreScreen_EnergyMore)
    {
        if (mEnergyMode != Idle)
        {
            if (--mButtonDownTickCounter == 0)
            {
                mPendingEnergyTransaction++;
                mButtonDownTickCounter = 5;
                mApp->PlaySample(Sexy::SOUND_BUTTONCLICK);
            }
        }
    }
}

//0x48C5F0
void StoreScreen::KeyChar(char theChar)
{
    if (mBubbleClickToContinue && (theChar == ' ' || theChar == '\r')) AdvanceCrazyDaveDialog();
}

//0x48C620
int StoreScreen::GetItemCost(int theIndex)
{
    if (mPage == 0)
    {
        if (theIndex == 0)
        {
            // Fertilizer
            return 75;
        }
        if (theIndex == 1)
        {
            // Bugspray
            return 100;
        }
        if (theIndex == 2)
        {
            //Tree Food
            return 250;
        }
        return 0;
    }
    else
    {
        auto slot_data = mApp->mAP->SlotData();
        auto item = mApp->mAP->ItemAtLocation(PVZRAPData::Locations::Twiddydinkie(theIndex + (mPage - 1) * 8));

        int shop_price = slot_data["shop_prices"][theIndex].get<int>();
        if (item.flags & APItem::ITEM_FLAG_PROGRESSION)
        {
            return shop_price + 90;
        }
        if (item.flags & APItem::ITEM_FLAG_USEFUL)
        {
            return shop_price + 50;
        }
        if (item.flags & APItem::ITEM_FLAG_TRAP)
        {
            return shop_price + 10;
        }
        return shop_price + 5;
    }
}

bool StoreScreen::CanAffordItem(int theIndex)
{
    return mApp->mPlayerInfo->mCoins >= GetItemCost(theIndex);
}

//0x48C740
void StoreScreen::PurchaseItem(int theIndex)
{
    if (mApp->mWidgetManager->mOverWidget)
        mApp->SetCursor(CURSOR_POINTER);
    mBubbleCountDown = 0;
    mApp->CrazyDaveStopTalking();
    if (!CanAffordItem(theIndex))
    {
        Dialog* aDialog = mApp->DoDialog(DIALOG_NOT_ENOUGH_MONEY, true, _S("Not enough money"/*[NOT_ENOUGH_MONEY]*/),
                                         _S("You can't afford this item yet. Earn more coins by killing zombies!"
                                             /*[CANNOT_AFFORD_ITEM]*/), _S("[DIALOG_BUTTON_OK]"), BUTTONS_FOOTER);
        mWaitForDialog = true;
        aDialog->WaitForResult(true);
        mWaitForDialog = false;
    }
    else
    {
        LawnDialog* aComfirmDialog = (LawnDialog*)mApp->DoDialog(
            DIALOG_STORE_PURCHASE,
            true,
            _S("Buy this item?"),
            _S("Are you sure you want to buy this item?"),
            _S(""),
            BUTTONS_YES_NO
        );
        aComfirmDialog->mLawnYesButton->SetLabel(_S("[DIALOG_BUTTON_YES]"));
        aComfirmDialog->mLawnNoButton->SetLabel(_S("[DIALOG_BUTTON_NO]"));

        mWaitForDialog = true;
        int aComfirmResult = aComfirmDialog->WaitForResult(true);
        mWaitForDialog = false;

        if (aComfirmResult == ID_OK)
        {
            mApp->mPlayerInfo->AddCoins(-GetItemCost(theIndex));
            if (mPage == 0)
            {
                if (theIndex == 0)
                {
                    // Fertiliser
                    if (mApp->mPlayerInfo->mPurchases[STORE_ITEM_FERTILIZER] < PURCHASE_COUNT_OFFSET)
                    {
                        mApp->mPlayerInfo->mPurchases[STORE_ITEM_FERTILIZER] = PURCHASE_COUNT_OFFSET;
                    }
                    mApp->mPlayerInfo->mPurchases[STORE_ITEM_FERTILIZER] += 5;
                }
                else if (theIndex == 1)
                {
                    // Bugspray
                    if (mApp->mPlayerInfo->mPurchases[STORE_ITEM_BUG_SPRAY] < PURCHASE_COUNT_OFFSET)
                    {
                        mApp->mPlayerInfo->mPurchases[STORE_ITEM_BUG_SPRAY] = PURCHASE_COUNT_OFFSET;
                    }
                    mApp->mPlayerInfo->mPurchases[STORE_ITEM_BUG_SPRAY] += 5;
                }
                else if (theIndex == 2)
                {
                    // Tree Food
                    if (mApp->mPlayerInfo->mPurchases[STORE_ITEM_TREE_FOOD] < PURCHASE_COUNT_OFFSET)
                    {
                        mApp->mPlayerInfo->mPurchases[STORE_ITEM_TREE_FOOD] = PURCHASE_COUNT_OFFSET;
                    }
                    mApp->mPlayerInfo->mPurchases[STORE_ITEM_TREE_FOOD]++;
                }
            }
            else
            {
                mApp->mAP->CheckLocations({PVZRAPData::Locations::Twiddydinkie(theIndex + (mPage - 1) * 8)});
            }
            mApp->WriteCurrentUserConfig();
        }
    }
}

//0x48CF50
void StoreScreen::AdvanceCrazyDaveDialog()
{
    if (!mBubbleClickToContinue)
        return;

    // “嘿，我的邻居！我有一些新东西出售啦！”
    if (mApp->mCrazyDaveMessageIndex == 3100)
    {
        mHatchTimer = 150;
        mHatchOpen = true;
        mApp->PlaySample(Sexy::SOUND_HATCHBACK_OPEN);
    }
    if (!mApp->AdvanceCrazyDaveText())
    {
        mApp->CrazyDaveStopTalking();
        mBubbleClickToContinue = false;
        mBubbleCountDown = 500;
        if (mHatchTimer == 0)
        {
            EnableButtons(true);
        }
    }
    else
    {
        SetBubbleText(mApp->mCrazyDaveMessageIndex, 0, true);
    }

    int aMessage = mApp->mCrazyDaveMessageIndex;
    if (aMessage == 303 || aMessage == 606 || aMessage == 2601)
    {
        mHatchTimer = 150;
        mHatchOpen = true;
        mApp->PlaySample(Sexy::SOUND_HATCHBACK_OPEN);
        
        ScoutPage();
    }
    else if (aMessage == 603)
    {
        mApp->mPlayerInfo->mNeedsMagicTacoReward = false;
        mApp->WriteCurrentUserConfig();
        mApp->PlaySample(Sexy::SOUND_DIAMOND);
        Coin* aCoin = mCoins.DataArrayAlloc();
        aCoin->CoinInitialize(80, 520, COIN_DIAMOND, COIN_MOTION_FROM_PRESENT);
        aCoin->mVelX = 0;
        aCoin->mVelY = -5;
    }
    else if (aMessage == 902 || aMessage == 1002)
    {
        mApp->mPlayerInfo->AddCoins(100);
    }
}

//0x48D130
void StoreScreen::MouseDown(int x, int y, int theClickCount)
{
    if (mBubbleClickToContinue)
    {
        AdvanceCrazyDaveDialog();
        return;
    }
    if (!CanInteractWithButtons()) return;
    for (int aItemPos = 0; aItemPos < MAX_PAGE_SPOTS; aItemPos++)
    {
        StoreItem aItemType = GetStoreItemType(aItemPos);
        if (aItemType == STORE_ITEM_INVALID) continue;
        int aItemX, aItemY;
        GetStorePosition(aItemPos, aItemX, aItemY);
        if (Rect(aItemX, aItemY, 50, 87).Contains(x, y))
        {
            if (IsFullVersionOnly(aItemType))
            {
                mWaitForDialog = true;
                mApp->LawnMessageBox(DIALOG_MESSAGE, _S("[GET_FULL_VERSION_TITLE]"), _S("[FULL_VERSION_TO_BUY]"),
                                     _S("[DIALOG_BUTTON_OK]"), _S(""), BUTTONS_FOOTER);
                mWaitForDialog = false;
            }
            else if (aItemType == STORE_ITEM_PVZ)
            {
                mWaitForDialog = true;
                int aResult = mApp->LawnMessageBox(
                    DIALOG_MESSAGE, _S("[BUY_PVZ_TITLE]"), _S("[BUY_PVZ_BODY]"), _S("[GET_FULL_VERSION_YES_BUTTON]"),
                    _S("[GET_FULL_VERSION_NO_BUTTON]"), BUTTONS_YES_NO);
                mWaitForDialog = false;
                if (aResult == ID_OK)
                {
                    if (mApp->mDRM)
                    {
                        mApp->mDRM->BuyGame();
                    }
                }
            }
            else if (!IsItemSoldOut(aItemPos) && !IsItemUnavailable(aItemType) && !IsComingSoon(aItemType))
                PurchaseItem(aItemPos);
            break;
        }
    }
}

//0x48D2E0
void StoreScreen::EnableButtons(bool theEnable)
{
    if (mEasyBuyingCheat || IsPageShown(1) || !theEnable)
    {
        mNextButton->mMouseVisible = theEnable;
        mNextButton->SetDisabled(!theEnable);
        mPrevButton->mMouseVisible = theEnable;
        mPrevButton->SetDisabled(!theEnable);
    }
    mBackButton->mMouseVisible = theEnable;
    mBackButton->SetDisabled(!theEnable);
    mEnergyButton->mMouseVisible = theEnable;
    mEnergyButton->SetDisabled(!theEnable);
    mEnergyBackButton->mMouseVisible = theEnable;
    mEnergyBackButton->SetDisabled(!theEnable);
    mDepositButton->mMouseVisible = theEnable;
    mDepositButton->SetDisabled(!theEnable);
    mWithdrawButton->mMouseVisible = theEnable;
    mWithdrawButton->SetDisabled(!theEnable);
    mEnergyAtmBackButton->mMouseVisible = theEnable;
    mEnergyAtmBackButton->SetDisabled(!theEnable);
    mEnergyLessButton->mMouseVisible = theEnable;
    mEnergyLessButton->SetDisabled(!theEnable);
    mEnergyMoreButton->mMouseVisible = theEnable;
    mEnergyMoreButton->SetDisabled(!theEnable);
    mEnergyGoButton->mMouseVisible = theEnable;
    mEnergyGoButton->SetDisabled(!theEnable);
}

// GOTY @Patoke: 0x498110
void StoreScreen::SetupForIntro(int theDialogIndex)
{
    mStartDialog = theDialogIndex;
    mHatchOpen = false;
    mBackButton->mLabel = TodStringTranslate(_S("[STORE_NEXT_LEVEL_BUTTON]"));
    EnableButtons(false);
}


void StoreScreen::SlideTo(int theX, int theY)
{
    mSlideCounter = 75;
    mDestX = theX;
    mDestY = theY;
    mStartX = mX;
    mStartY = mY;
}
