#pragma once
#include "Game/gui_items.h"
#include "LanguageScript.h"
#include "Specific/configuration.h"

enum class InventoryMode
{
	None,
	InGame,
	Pause,
	Statistics,
	Examine,
	Diary,
	Load,
	Save
};

enum class InventoryResult
{
	None,
	UseItem,
	NewGame,
	LoadGame,
	SaveGame,
	ExitGame,
	ExitToTitle,
	NewGameSelectedLevel
};

enum class LoadResult
{
	None,
	Load,
	Cancel
};

enum class MenuType
{
	None,
	Use,
	ChooseAmmo,
	Combine,
	Seperate,
	Equip,
	Ammo1,
	Ammo2,
	Ammo3,
	Load,
	Save,
	Examine,
	Statistics,
	Diary
};

enum class RingTypes
{
	Inventory,
	Ammo
};

enum class Menu
{
	Title,
	Pause,
	Statistics,
	SelectLevel,
	LoadGame,
	Options,
	Display,
	Controls,
	OtherSettings
};

struct MenuOption
{
	MenuType type;
	char const* text;
};

struct ObjectList
{
	short invitem;
	unsigned short xrot;
	unsigned short yrot;
	unsigned short zrot;
	unsigned short bright;
};

struct InventoryRing
{
	ObjectList CurrentObjectList[INVENTORY_TABLE_SIZE + 1];
	int ringactive;
	int objlistmovement;
	int curobjinlist;
	int numobjectsinlist;
};

struct SettingsData
{
	bool WaitingForKey = false; // Waiting for a key to be pressed when configuring controls
	bool IgnoreInput = false;   // Ignore input unless all keys were released
	int SelectedScreenResolution;
	GameConfiguration Configuration;
};

class GuiController
{
public:
	bool CallInventory(bool resetMode);
	InventoryResult TitleOptions();
	InventoryResult DoPauseMenu();
	void DrawInventory();
	void DrawCurrentObjectList(int ringIndex);
	int IsObjectInInventory(int objectNumber);
	int ConvertObjectToInventoryItem(int objectNumber);
	int ConvertInventoryItemToObject(int objectNumber);
	void FadeAmmoSelector();
	void DrawAmmoSelector();
	bool PerformWaterskinCombine(int flag);
	void DrawCompass();

	// Getters
	InventoryRing* GetRings(int ringIndex);
	short GetSelectedOption();
	Menu GetMenuToDisplay();
	InventoryMode GetInventoryMode();
	int GetInventoryItemChosen();
	int GetEnterInventory();
	int GetLastInventoryItem();
	SettingsData GetCurrentSettings();
	short GetLoadSaveSelection();

	// Setters
	void SetSelectedOption(int menu);
	void SetMenuToDisplay(Menu menu);
	void SetInventoryMode(InventoryMode mode);
	void SetEnterInventory(int num);
	void SetInventoryItemChosen(int num);

private:
	// Input macros
	#define GUI_INPUT_PULSE_UP	  IsPulsed(In::Forward, 0.1f, 0.4f)
	#define GUI_INPUT_PULSE_DOWN  IsPulsed(In::Back, 0.1f, 0.4f)
	#define GUI_INPUT_PULSE_LEFT  IsPulsed(In::Left, 0.1f, 0.4f)
	#define GUI_INPUT_PULSE_RIGHT IsPulsed(In::Right, 0.1f, 0.4f)
	#define GUI_INPUT_SELECT	  ((IsReleased(In::Select) || IsReleased(In::Action)) && !IsHeld(In::Deselect))
	#define GUI_INPUT_DESELECT	  (IsClicked(In::Deselect) && !(IsHeld(In::Select) || IsHeld(In::Action)))

	// GUI variables
	Menu MenuToDisplay = Menu::Title;
	int SelectedOption;
	int OptionCount;
	int SelectedSaveSlot;

	SettingsData CurrentSettings;

	// Inventory variables
	short CombineObject1;
	short CombineObject2;
	char UseItem;
	char SeperateTypeFlag;
	char CombineTypeFlag;
	int CompassNeedleAngle;
	InventoryRing PCRing1;
	InventoryRing PCRing2;
	InventoryRing* Rings[2];
	int CurrentSelectedOption;
	int MenuActive;
	char AmmoSelectorFlag;
	char NumAmmoSlots;
	char* CurrentAmmoType;
	AmmoList AmmoObjectList[3];
	short AmmoSelectorFadeVal;
	short AmmoSelectorFadeDir;
	short CombineRingFadeVal;
	short CombineRingFadeDir;
	short NormalRingFadeVal;
	short NormalRingFadeDir;
	unsigned char AmmoActive;
	int OBJLIST_SPACING;
	MenuOption CurrentOptions[3];
	InventoryMode InvMode;
	int InventoryItemChosen;
	int EnterInventory;
	int LastInvItem;
	AmmoData Ammo;

	void HandleDisplaySettingsInput(bool fromPauseMenu);
	void HandleControlSettingsInput(bool fromPauseMenu);
	void HandleOtherSettingsInput(bool fromPauseMenu);
	void HandleOptionsInput();
	void BackupOptions();
	bool DoObjectsCombine(int objectNumber1, int objectNumber2);
	void InitialiseInventory();
	void FillDisplayOptions();
	bool IsItemCurrentlyCombinable(int objectNumber);
	bool IsItemInInventory(int objectNumber);
	void CombineObjects(int objectNumber1, int objectNumber2);
	void SetupObjectListStartPosition(int newObjectNumber);
	void SetupObjectListStartPosition2(int newObjectNumber);
	void HandleObjectChangeover(int ringIndex);
	void SetupAmmoSelector();
	void ConstructObjectList();
	void SeparateObject(int objectNumber);
	void InsertObjectIntoList(int objectNumber);
	void InsertObjectIntoList_v2(int objectNumber);
	void UseCurrentItem();
	void SpinBack(unsigned short* angle);
	void UpdateWeaponStatus();
	void DoStatisticsMode();
	void DoExamineMode();
	void DoDiary();
	LoadResult DoLoad();
	bool DoSave();
	void DoInventory();
	void ConstructCombineObjectList();
};

extern GuiController g_Gui;
extern const char* ControlStrings[];
