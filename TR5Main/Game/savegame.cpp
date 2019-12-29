#include "savegame.h"
#include "..\Global\global.h"
#include "..\Game\Lara.h"
#include "..\Game\items.h"
#include "..\Game\box.h"
#include "..\Game\pickup.h"
#include "..\Game\lot.h"
#include "..\Game\switch.h"
#include "..\Game\spotcam.h"
#include "traps.h"
#include "..\Game\laramisc.h"
#include "..\Objects\newobjects.h"
#include "..\Game\sound.h"

FileStream* SaveGame::m_stream;
ChunkReader* SaveGame::m_reader;
ChunkWriter* SaveGame::m_writer;
vector<LuaVariable> SaveGame::m_luaVariables;
int SaveGame::LastSaveGame;

ChunkId* SaveGame::m_chunkGameStatus;
ChunkId* SaveGame::m_chunkItems;
ChunkId* SaveGame::m_chunkItem;
ChunkId* SaveGame::m_chunkLara;
ChunkId* SaveGame::m_chunkLuaVariable;
ChunkId* SaveGame::m_chunkStaticFlags;
ChunkId* SaveGame::m_chunkVehicle;
ChunkId* SaveGame::m_chunkSequenceSwitch;
ChunkId* SaveGame::m_chunkFlybyFlags;
ChunkId* SaveGame::m_chunkCdFlags;
ChunkId* SaveGame::m_chunkCamera;
ChunkId* SaveGame::m_chunkFlipStats;
ChunkId* SaveGame::m_chunkFlipMap;
ChunkId* SaveGame::m_chunkItemDummy;
ChunkId* SaveGame::m_chunkStatistics;
ChunkId* SaveGame::m_chunkItemAnims;
ChunkId* SaveGame::m_chunkItemMeshes;
ChunkId* SaveGame::m_chunkItemFlags;
ChunkId* SaveGame::m_chunkItemHitPoints;
ChunkId* SaveGame::m_chunkItemPosition;
ChunkId* SaveGame::m_chunkItemIntelligentData;
ChunkId* SaveGame::m_chunkSpecialItemBurningTorch;
ChunkId* SaveGame::m_chunkSpecialItemChaff;
ChunkId* SaveGame::m_chunkSpecialItemTorpedo;
ChunkId* SaveGame::m_chunkSpecialItemCrossbowBolt;
ChunkId* SaveGame::m_chunkSpecialItemFlare;
ChunkId* SaveGame::m_chunkItemQuadInfo;
ChunkId* SaveGame::m_chunkBats;
ChunkId* SaveGame::m_chunkRats;
ChunkId* SaveGame::m_chunkSpiders;

extern vector<AudioTrack> g_AudioTracks;
extern byte SequenceUsed[6];
extern byte SequenceResults[3][3][3];
extern byte Sequences[3];
extern byte CurrentSequence;

void SaveGame::saveItems()
{
	// Save level items
	for (int i = 0; i < NumItems; i++)
		m_writer->WriteChunkWithChildren(m_chunkItem, &saveItem, i, 0);

	// Save items created at runtime (flares, missiles...)
	for (int i = NumItems; i < NUM_ITEMS; i++)
	{
		ITEM_INFO* item = &Items[i];
		if (item->active)
		{
			// Some items are very special and are saved in specific functions, all the others use the general function
			if (item->objectNumber == ID_BURNING_TORCH_ITEM)
				m_writer->WriteChunk(m_chunkSpecialItemBurningTorch, &saveBurningTorch, i, 1);
			else if (item->objectNumber == ID_CHAFF)
				m_writer->WriteChunk(m_chunkSpecialItemChaff, &saveChaff, i, 1);
			else if (item->objectNumber == ID_TORPEDO)
				m_writer->WriteChunk(m_chunkSpecialItemTorpedo, &saveTorpedo, i, 1);
			else if (item->objectNumber == ID_CROSSBOW_BOLT)
				m_writer->WriteChunk(m_chunkSpecialItemCrossbowBolt, &saveCrossbowBolt, i, 1);
			else if (item->objectNumber == ID_FLARE_ITEM)
				m_writer->WriteChunk(m_chunkSpecialItemFlare, &saveFlare, i, 1);
			else
				m_writer->WriteChunkWithChildren(m_chunkItem, &saveItem, i, 1);
		}
	}

	// Save special items
	if (Objects[ID_BATS].loaded)
		for (int i = 0; i < NUM_BATS; i++)
			if (Bats[i].on)
				m_writer->WriteChunk(m_chunkBats, &saveBats, i, 0);

	if (Objects[ID_RATS].loaded)
		for (int i = 0; i < NUM_RATS; i++)
			if (Rats[i].on)
				m_writer->WriteChunk(m_chunkRats, &saveRats, i, 0);

	if (Objects[ID_SPIDER].loaded)
		for (int i = 0; i < NUM_SPIDERS; i++)
			if (Spiders[i].on)
				m_writer->WriteChunk(m_chunkSpiders, &saveSpiders, i, 0);
}

void SaveGame::saveItem(int itemNumber, int runtimeItem)
{
	ITEM_INFO* item = &Items[itemNumber];
	OBJECT_INFO* obj = &Objects[item->objectNumber];

	LEB128::Write(m_stream, itemNumber);
	LEB128::Write(m_stream, runtimeItem);

	bool hasData = true;

	if (item->flags & IFLAG_KILLED)
	{
		LEB128::Write(m_stream, 0x2000);
		hasData = false;
	}
	else if (item->flags & (IFLAG_ACTIVATION_MASK | IFLAG_INVISIBLE | 0x20) || item->objectNumber == ID_LARA)
	{
		LEB128::Write(m_stream, 0x8000);
		hasData = true;
	}
	else
	{
		LEB128::Write(m_stream, 0x0000);
		hasData = false;
	}

	LEB128::Write(m_stream, item->objectNumber);
	LEB128::Write(m_stream, item->speed);
	LEB128::Write(m_stream, item->fallspeed);

	if (hasData)
	{
		m_writer->WriteChunkInt(m_chunkItemDummy, 1);

		if (obj->saveAnim)
			m_writer->WriteChunk(m_chunkItemAnims, &saveItemAnims, itemNumber, 0);

		if (obj->savePosition)
			m_writer->WriteChunk(m_chunkItemPosition, &saveItemPosition, itemNumber, 0);

		if (obj->saveHitpoints)
			m_writer->WriteChunk(m_chunkItemHitPoints, &saveItemHitPoints, itemNumber, 0);

		if (obj->saveFlags)
			m_writer->WriteChunkWithChildren(m_chunkItemFlags, &saveItemFlags, itemNumber, 0);

		/*if (obj->saveMesh)
			m_writer->WriteChunk(m_chunkItemMeshes, &saveItemMesh, itemNumber, 0);*/

		if (obj->intelligent && item->data != NULL)
			m_writer->WriteChunk(m_chunkItemIntelligentData, &saveItemIntelligentData, itemNumber, 0);
	}
	else
	{
		m_writer->WriteChunkInt(m_chunkItemDummy, 1);
	}
}

void SaveGame::saveGameStatus(int arg1, int arg2)
{
	LEB128::Write(m_stream, FlipStatus);
	LEB128::Write(m_stream, LastInventoryItem);
	LEB128::Write(m_stream, FlipEffect);
	LEB128::Write(m_stream, FlipTimer);
	LEB128::Write(m_stream, CurrentAtmosphere);
	LEB128::Write(m_stream, CurrentSequence);
	
	// Now the sub-chunks
	if (NumberRooms > 0)
	{
		for (int i = 0; i < NumberRooms; i++)
			for (int j = 0; j < Rooms[i].numMeshes; j++)
				m_writer->WriteChunk(m_chunkStaticFlags, &saveStaticFlag, i, j);
	}

	for (int i = 0; i < 6; i++)
		m_writer->WriteChunk(m_chunkSequenceSwitch, &saveSequenceSwitch, i, SequenceUsed[i]);

	for (int i = 0; i < NumberSpotcams; i++)
		m_writer->WriteChunk(m_chunkFlybyFlags, &saveFlybyFlags, i, SpotCam[i].flags);

	for (int i = 0; i < g_AudioTracks.size(); i++)
		m_writer->WriteChunk(m_chunkCdFlags, &saveCdFlags, i, g_AudioTracks[i].Mask);

	for (int i = 0; i < NumberCameras; i++)
		m_writer->WriteChunk(m_chunkCamera, &saveCamera, i, Camera.fixed[i].flags);

	for (int i = 0; i < 255; i++)
		m_writer->WriteChunk(m_chunkFlipStats, &saveFlipStats, i, FlipStats[i]);

	for (int i = 0; i < 255; i++)
		m_writer->WriteChunk(m_chunkFlipMap, &saveFlipMap, i, FlipMap[i]);
}

void SaveGame::saveLara(int arg1, int arg2)
{
	// LARA_INFO struct dumped to savegame
	LARA_INFO lara;
	memcpy(&lara, &Lara, sizeof(Lara));

	for (int i = 0; i < 15; i++)
		lara.meshPtrs[i] = (short*)((char*)lara.meshPtrs[i] - (ptrdiff_t)MeshBase);

	lara.leftArm.frameBase = (short*)((char *)lara.leftArm.frameBase - (ptrdiff_t)Objects[ID_PISTOLS_ANIM].frameBase);
	lara.rightArm.frameBase = (short*)((char *)lara.rightArm.frameBase - (ptrdiff_t)Objects[ID_PISTOLS_ANIM].frameBase);
	lara.generalPtr = (char *)lara.generalPtr - (ptrdiff_t)MallocBuffer;

	m_stream->Write(reinterpret_cast<char*>(&lara), sizeof(Lara));
	
	// Lara weapon data
	ITEM_INFO* weaponItem = &Items[Lara.weaponItem];

	LEB128::Write(m_stream, weaponItem->objectNumber);
	LEB128::Write(m_stream, weaponItem->animNumber);
	LEB128::Write(m_stream, weaponItem->frameNumber);
	LEB128::Write(m_stream, weaponItem->currentAnimState);
	LEB128::Write(m_stream, weaponItem->goalAnimState);
	LEB128::Write(m_stream, weaponItem->requiredAnimState);

	// Lara extra data
	m_writer->WriteChunkInt(m_chunkVehicle, g_LaraExtra.Vehicle);
}

void SaveGame::saveVariables()
{
	m_luaVariables.clear();
	g_GameScript->GetVariables(&m_luaVariables);
	for (int i = 0; i < m_luaVariables.size(); i++)
		m_writer->WriteChunk(m_chunkLuaVariable, &saveVariable, i, 0);
}

void SaveGame::saveVariable(int arg1, int arg2)
{
	LuaVariable variable = m_luaVariables[arg1];
	
	LEB128::Write(m_stream, variable.IsGlobal == 1);
	LEB128::Write(m_stream, variable.Type);
	m_stream->WriteString((char*)variable.Name.c_str());

	if (variable.Type == LUA_VARIABLE_TYPE_BOOL)
		m_stream->WriteBool(variable.BoolValue);
	else if (variable.Type == LUA_VARIABLE_TYPE_INT)
		m_stream->WriteInt32(variable.IntValue);
	else if (variable.Type == LUA_VARIABLE_TYPE_STRING)
		m_stream->WriteString((char*)variable.StringValue.c_str());
	else if (variable.Type == LUA_VARIABLE_TYPE_FLOAT)
		m_stream->WriteFloat(variable.FloatValue);
}

void SaveGame::Start()
{
	m_chunkGameStatus = ChunkId::FromString("TR5MSgGameStatus");
	m_chunkItems = ChunkId::FromString("TR5MSgItems");
	m_chunkItem = ChunkId::FromString("TR5MSgItem");
	m_chunkLara = ChunkId::FromString("TR5MSgLara");
	m_chunkLuaVariable = ChunkId::FromString("TR5MSgLuaVar");
	m_chunkStaticFlags = ChunkId::FromString("TR5MSgStFl");
	m_chunkVehicle = ChunkId::FromString("TR5MSgLaraVeh");
	m_chunkSequenceSwitch = ChunkId::FromString("TR5MSgSeqSw");
	m_chunkFlybyFlags = ChunkId::FromString("TR5MSgFlybyF");
	m_chunkCdFlags = ChunkId::FromString("TR5MSgCdF");
	m_chunkCamera = ChunkId::FromString("TR5MSgCam");
	m_chunkFlipStats = ChunkId::FromString("TR5MSgFlS");
	m_chunkFlipMap = ChunkId::FromString("TR5MSgFlM");
	m_chunkItemDummy = ChunkId::FromString("TR5MSgItemDummy");
	m_chunkStatistics = ChunkId::FromString("TR5MSgStats");
	m_chunkItemAnims = ChunkId::FromString("TR5MSgItAnms");
	m_chunkItemMeshes = ChunkId::FromString("TR5MSgItMsh");
	m_chunkItemFlags = ChunkId::FromString("TR5MSgItFl");
	m_chunkItemHitPoints = ChunkId::FromString("TR5MSgItHP");
	m_chunkItemPosition = ChunkId::FromString("TR5MSgItPos");
	m_chunkItemIntelligentData = ChunkId::FromString("TR5MSgItIntell");
	m_chunkSpecialItemBurningTorch = ChunkId::FromString("TR5MSgItTorch");
	m_chunkSpecialItemChaff = ChunkId::FromString("TR5MSgItChaff");
	m_chunkSpecialItemTorpedo = ChunkId::FromString("TR5MSgItTorpedo");
	m_chunkSpecialItemCrossbowBolt = ChunkId::FromString("TR5MSgItBolt");
	m_chunkSpecialItemFlare = ChunkId::FromString("TR5MSgItFlare");
	m_chunkItemQuadInfo = ChunkId::FromString("TR5MSgQuadInfo");
	m_chunkBats = ChunkId::FromString("TR5MSgBats");
	m_chunkRats = ChunkId::FromString("TR5MSgRats");
	m_chunkSpiders = ChunkId::FromString("TR5MSgSpiders");

	LastSaveGame = 0;
}

void SaveGame::End()
{
	delete m_chunkGameStatus;
	delete m_chunkItems;
	delete m_chunkItem;
	delete m_chunkLara;
	delete m_chunkLuaVariable;
	delete m_chunkStaticFlags;
	delete m_chunkVehicle;
	delete m_chunkFlipMap;
	delete m_chunkFlipStats;
	delete m_chunkFlybyFlags;
	delete m_chunkSequenceSwitch;
	delete m_chunkCdFlags;
	delete m_chunkCamera;
	delete m_chunkItemDummy;
	delete m_chunkStatistics;
	delete m_chunkItemAnims;
	delete m_chunkItemMeshes;
	delete m_chunkItemFlags;
	delete m_chunkItemHitPoints;
	delete m_chunkItemPosition;
	delete m_chunkItemIntelligentData;
	delete m_chunkSpecialItemBurningTorch;
	delete m_chunkSpecialItemChaff;
	delete m_chunkSpecialItemTorpedo;
	delete m_chunkSpecialItemCrossbowBolt;
	delete m_chunkSpecialItemFlare;
	delete m_chunkItemQuadInfo;
	delete m_chunkBats;
	delete m_chunkRats;
	delete m_chunkSpiders;
}

bool SaveGame::Save(char* fileName)
{
	m_stream = new FileStream(fileName, true, true);
	m_writer = new ChunkWriter(0x4D355254, m_stream);

	printf("Timer: %d\n", Savegame.Game.Timer);

	// The header must be here, so no chunks
	m_stream->WriteString(g_GameFlow->GetString(g_GameFlow->GetLevel(CurrentLevel)->NameStringIndex));
	LEB128::Write(m_stream, (GameTimer / 30) / 86400);
	LEB128::Write(m_stream, ((GameTimer / 30) % 86400) / 3600);
	LEB128::Write(m_stream, ((GameTimer / 30) / 60) % 60);
	LEB128::Write(m_stream, (GameTimer / 30) % 60);
	LEB128::Write(m_stream, CurrentLevel);
	LEB128::Write(m_stream, GameTimer);
	LEB128::Write(m_stream, ++LastSaveGame);

	// Now we write chunks
	m_writer->WriteChunk(m_chunkStatistics, &saveStatistics, 0, 0);
	m_writer->WriteChunkWithChildren(m_chunkGameStatus, &saveGameStatus, 0, 0);
	m_writer->WriteChunkWithChildren(m_chunkLara, &saveLara, 0, 0);
	saveItems();
	saveVariables();
	
	// EOF
	m_stream->WriteInt32(0);

	m_stream->Close();

	delete m_writer;
	delete m_stream;

	return true;
}

bool SaveGame::readGameStatus()
{
	FlipStatus = LEB128::ReadInt32(m_stream);
	LastInventoryItem = LEB128::ReadInt32(m_stream);
	FlipEffect = LEB128::ReadInt32(m_stream);
	FlipTimer = LEB128::ReadInt32(m_stream);
	CurrentAtmosphere = LEB128::ReadByte(m_stream);
	CurrentSequence = LEB128::ReadByte(m_stream);

	m_reader->ReadChunks(&readGameStatusChunks, 0);

	return true;
}

bool SaveGame::readLara()
{
	// Read dumped LARA_INFO struct
	char* buffer = (char*)malloc(sizeof(LARA_INFO));
	m_stream->Read(buffer, sizeof(LARA_INFO));
	LARA_INFO* lara = reinterpret_cast<LARA_INFO*>(buffer);
	memcpy(&Lara, lara, sizeof(LARA_INFO));
	free(buffer);

	for (int i = 0; i < 15; i++)
	{
		Lara.meshPtrs[i] = ADD_PTR(Lara.meshPtrs[i], short, MeshBase);
		printf("MeshPtr: %d\n", Lara.meshPtrs[i]);
	}

	Lara.leftArm.frameBase = ADD_PTR(Lara.leftArm.frameBase, short, Objects[ID_PISTOLS_ANIM].frameBase);
	Lara.rightArm.frameBase = ADD_PTR(Lara.rightArm.frameBase, short, Objects[ID_PISTOLS_ANIM].frameBase);
	
	Lara.target = NULL;
	Lara.spazEffect = NULL;
	Lara.generalPtr = ADD_PTR(Lara.generalPtr, char, MallocBuffer);

	// Is Lara burning?
	if (Lara.burn)
	{
		Lara.burn = false;
		bool smokeFlag = false;
		if (Lara.burnSmoke)
		{
			Lara.burnSmoke = false;
			smokeFlag = true;
		}
		
		LaraBurn();
		if (smokeFlag)
			Lara.burnSmoke = true;
	}

	// Lara weapon data
	if (Lara.weaponItem)
	{
		short weaponItemNum = CreateItem();
		Lara.weaponItem = weaponItemNum;

		ITEM_INFO* weaponItem = &Items[Lara.weaponItem];

		weaponItem->objectNumber = LEB128::ReadInt16(m_stream);
		weaponItem->animNumber = LEB128::ReadInt16(m_stream);
		weaponItem->frameNumber = LEB128::ReadInt16(m_stream);
		weaponItem->currentAnimState = LEB128::ReadInt16(m_stream);
		weaponItem->goalAnimState = LEB128::ReadInt16(m_stream);
		weaponItem->requiredAnimState = LEB128::ReadInt16(m_stream);
		weaponItem->roomNumber = 255;
	}
	
	m_reader->ReadChunks(&readLaraChunks, 0);

	return true;
}

bool SaveGame::readItem()
{
	short itemNumber = LEB128::ReadInt16(m_stream);
	short runtimeItem = LEB128::ReadByte(m_stream);
	short itemKind = LEB128::ReadInt16(m_stream);

	// Runtime items must be allocated dynamically
	if (runtimeItem)
		itemNumber = CreateItem();
	
	ITEM_INFO* item = &Items[itemNumber];
	item->objectNumber = LEB128::ReadInt16(m_stream);

	OBJECT_INFO* obj = &Objects[item->objectNumber];

	// Runtime items must be initialised
	// TODO: test test test!!!
	if (runtimeItem)
	{
		InitialiseItem(itemNumber);
		AddActiveItem(itemNumber);
	}

	item->speed = LEB128::ReadInt16(m_stream);
	item->fallspeed = LEB128::ReadInt16(m_stream);

	if (itemKind == 0x2000)
	{
		KillItem(itemNumber);
		item->status = ITEM_DEACTIVATED;
		//*(pItem - 35) |= 1u;
		m_reader->ReadChunks(&readItemChunks, itemNumber);
	}
	else if (itemKind == 0x8000)
	{
		m_reader->ReadChunks(&readItemChunks, itemNumber);
	}
	else
	{
		m_reader->ReadChunks(&readItemChunks, itemNumber);
	}

	// Some post-processing things
	if (obj->collision == PuzzleHoleCollision && (item->status == ITEM_DEACTIVATED || item->status == ITEM_ACTIVE))
	{
		item->objectNumber += 8;
		//*((_WORD *)pItem - 28) = v55 + Objects[*((_WORD *)pItem - 32)].anim_index;
	}

	if (item->objectNumber >= ID_SMASH_OBJECT1 && item->objectNumber <= ID_SMASH_OBJECT8 && (item->flags & ONESHOT))
		item->meshBits = 0x100;

	if (item->objectNumber == ID_RAISING_BLOCK1 && item->itemFlags[1])
	{
		if (item->triggerFlags == -1)
			AlterFloorHeight(item, -255);
		else if (item->triggerFlags == -3)
			AlterFloorHeight(item, -1023);
		else
			AlterFloorHeight(item, -1024);
	}

	if (item->objectNumber == ID_RAISING_BLOCK2 && item->itemFlags[1])
		AlterFloorHeight(item, -2048);
	   
	return true;
}

bool SaveGame::readVariable()
{
	LuaVariable variable;
	
	variable.IsGlobal = LEB128::ReadByte(m_stream) == 1;
	variable.Type = LEB128::ReadInt32(m_stream);

	char* variableName;
	m_stream->ReadString(&variableName);
	variable.Name = string(variableName);
	free(variableName);

	if (variable.Type == LUA_VARIABLE_TYPE_BOOL)
		m_stream->ReadBool(&variable.BoolValue);
	else if (variable.Type == LUA_VARIABLE_TYPE_INT)
		m_stream->ReadInt32(&variable.IntValue);
	else if (variable.Type == LUA_VARIABLE_TYPE_FLOAT)
		m_stream->ReadFloat(&variable.FloatValue);
	else if (variable.Type == LUA_VARIABLE_TYPE_STRING)
	{
		char* variableValue;
		m_stream->ReadString(&variableValue);
		variable.StringValue = string(variableValue);
		free(variableValue);
	}

	return true;
}

bool SaveGame::readSavegameChunks(ChunkId* chunkId, int maxSize, int arg)
{
	if (chunkId->EqualsTo(m_chunkGameStatus))
		return readGameStatus();
	else if (chunkId->EqualsTo(m_chunkLara))
		return readLara();
	else if (chunkId->EqualsTo(m_chunkItem))
		return readItem();
	else if (chunkId->EqualsTo(m_chunkLuaVariable))
		return readVariable();
	else if (chunkId->EqualsTo(m_chunkStatistics))
		return readStatistics();
	else if (chunkId->EqualsTo(m_chunkSpecialItemBurningTorch))
		return readBurningTorch();
	else if (chunkId->EqualsTo(m_chunkSpecialItemChaff))
		return readChaff();
	else if (chunkId->EqualsTo(m_chunkSpecialItemTorpedo))
		return readTorpedo();
	else if (chunkId->EqualsTo(m_chunkSpecialItemCrossbowBolt))
		return readCrossbowBolt();
	else if (chunkId->EqualsTo(m_chunkSpecialItemFlare))
		return readFlare();
	else if (chunkId->EqualsTo(m_chunkBats))
		return readBats();
	else if (chunkId->EqualsTo(m_chunkRats))
		return readRats();
	else if (chunkId->EqualsTo(m_chunkSpiders))
		return readSpiders();

	return false;
}

bool SaveGame::Load(char* fileName)
{
	m_luaVariables.clear();

	m_stream = new FileStream(fileName, true, false);
	m_reader = new ChunkReader(0x4D355254, m_stream);

	// Header must be here
	char* levelName;
	m_stream->ReadString(&levelName);

	// Skip timer
	LEB128::ReadInt32(m_stream);
	LEB128::ReadInt32(m_stream);
	LEB128::ReadInt32(m_stream);
	LEB128::ReadInt32(m_stream);

	CurrentLevel = LEB128::ReadByte(m_stream);
	GameTimer = LEB128::ReadInt32(m_stream);
	LastSaveGame = LEB128::ReadInt32(m_stream);

	// Read chunks
	m_reader->ReadChunks(&readSavegameChunks, 0);

	// Close the stream
	m_stream->Close();
	//delete m_writer;
	//delete m_stream;

	g_GameScript->SetVariables(&m_luaVariables);

	JustLoaded = true;

	return true;
}

bool SaveGame::LoadHeader(char* fileName, SaveGameHeader* header)
{
	m_stream = new FileStream(fileName, true, false);
	m_reader = new ChunkReader(0x4D355254, m_stream);

	// Header must be here
	char* levelName;
	m_stream->ReadString(&levelName);
	header->LevelName = string(levelName);
	free(levelName);

	// Skip timer
	header->Days = LEB128::ReadInt32(m_stream);
	header->Hours = LEB128::ReadInt32(m_stream);
	header->Minutes = LEB128::ReadInt32(m_stream);
	header->Seconds = LEB128::ReadInt32(m_stream);
	
	header->Level = LEB128::ReadByte(m_stream);
	header->Timer = LEB128::ReadInt32(m_stream);
	header->Count = LEB128::ReadInt32(m_stream);

	// Close the stream
	m_stream->Close();
	//delete m_writer;
	//delete m_stream;

	return true;
}

void SaveGame::saveStaticFlag(int arg1, int arg2)
{
	LEB128::Write(m_stream, arg1);
	LEB128::Write(m_stream, arg2);
	LEB128::Write(m_stream, Rooms[arg1].mesh[arg2].Flags);
}

bool SaveGame::readLaraChunks(ChunkId* chunkId, int maxSize, int arg)
{
	if (chunkId->EqualsTo(m_chunkVehicle))
	{
		g_LaraExtra.Vehicle = m_reader->ReadChunkInt16(maxSize);
		return true;
	}
	return false;
}

bool SaveGame::readGameStatusChunks(ChunkId* chunkId, int maxSize, int arg)
{
	if (chunkId->EqualsTo(m_chunkStaticFlags))
	{
		short roomIndex = LEB128::ReadInt16(m_stream);
		short staticIndex = LEB128::ReadInt16(m_stream);
		short flags = LEB128::ReadInt16(m_stream);
		Rooms[roomIndex].mesh[staticIndex].Flags = flags;

		if (!flags)
		{
			FLOOR_INFO* floor = GetFloor(Rooms[roomIndex].mesh[staticIndex].x,
				Rooms[roomIndex].mesh[staticIndex].y,
				Rooms[roomIndex].mesh[staticIndex].z,
				&roomIndex);
			int height = GetFloorHeight(floor, Rooms[roomIndex].mesh[staticIndex].x,
				Rooms[roomIndex].mesh[staticIndex].y,
				Rooms[roomIndex].mesh[staticIndex].z);
			TestTriggers(TriggerIndex, 1, 0);
			floor->stopper = false;
		}

		return true;
	}
	else if (chunkId->EqualsTo(m_chunkFlipStats))
	{
		short index = LEB128::ReadInt16(m_stream);
		short value = LEB128::ReadInt16(m_stream);
		FlipStats[index] = value;
		return true;
	}
	else if (chunkId->EqualsTo(m_chunkFlipMap))
	{
		short index = LEB128::ReadInt16(m_stream);
		short value = LEB128::ReadInt16(m_stream);
		FlipMap[index] = value;
		if (value)
			DoFlipMap(value);
		return true;
	}
	else if (chunkId->EqualsTo(m_chunkCdFlags))
	{
		short index = LEB128::ReadInt16(m_stream);
		printf("Index: %d\n", index);
		short value = LEB128::ReadInt16(m_stream);
		if (index < g_AudioTracks.size())
			g_AudioTracks[index].Mask = value;
		return true;
	}
	else if (chunkId->EqualsTo(m_chunkCamera))
	{
		short index = LEB128::ReadInt16(m_stream);
		short value = LEB128::ReadInt16(m_stream);
		Camera.fixed[index].flags = value;
		return true;
	}
	else if (chunkId->EqualsTo(m_chunkSequenceSwitch))
	{
		short index = LEB128::ReadInt16(m_stream);
		short value = LEB128::ReadInt16(m_stream);
		SequenceUsed[index] = value;
		return true;
	}
	else if (chunkId->EqualsTo(m_chunkFlybyFlags))
	{
		int index = LEB128::ReadInt16(m_stream);
		int value = LEB128::ReadInt16(m_stream);
		SpotCam[index].flags = value;
		return true;
	}
	return false;
}

void SaveGame::saveCamera(int arg1, int arg2)
{
	LEB128::Write(m_stream, arg1);
	LEB128::Write(m_stream, arg2);
}

void SaveGame::saveSequenceSwitch(int arg1, int arg2)
{
	LEB128::Write(m_stream, arg1);
	LEB128::Write(m_stream, arg2);
}

void SaveGame::saveFlybyFlags(int arg1, int arg2)
{
	LEB128::Write(m_stream, arg1);
	LEB128::Write(m_stream, arg2);
}

void SaveGame::saveFlipMap(int arg1, int arg2)
{
	LEB128::Write(m_stream, arg1);
	LEB128::Write(m_stream, arg2);
}

void SaveGame::saveFlipStats(int arg1, int arg2)
{
	LEB128::Write(m_stream, arg1);
	LEB128::Write(m_stream, arg2);
}

void SaveGame::saveCdFlags(int arg1, int arg2)
{
	LEB128::Write(m_stream, arg1);
	LEB128::Write(m_stream, arg2);
}

bool SaveGame::readItemChunks(ChunkId* chunkId, int maxSize, int itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];

	if (chunkId->EqualsTo(m_chunkItemDummy))
		return m_reader->ReadChunkInt32(maxSize);
	else if (chunkId->EqualsTo(m_chunkItemAnims))
	{
		item->currentAnimState = LEB128::ReadInt16(m_stream);
		item->goalAnimState = LEB128::ReadInt16(m_stream);
		item->requiredAnimState = LEB128::ReadInt16(m_stream);
		item->animNumber = LEB128::ReadInt16(m_stream);
		item->frameNumber = LEB128::ReadInt16(m_stream);

		return true;
	}
	else if (chunkId->EqualsTo(m_chunkItemPosition))
	{
		item->pos.xPos = LEB128::ReadInt32(m_stream);
		item->pos.yPos = LEB128::ReadInt32(m_stream);
		item->pos.zPos = LEB128::ReadInt32(m_stream);
		item->pos.xRot = LEB128::ReadInt16(m_stream);
		item->pos.yRot = LEB128::ReadInt16(m_stream);
		item->pos.zRot = LEB128::ReadInt16(m_stream);

		short roomNumber = LEB128::ReadInt16(m_stream);
		if (item->roomNumber != roomNumber)
			ItemNewRoom(itemNumber, roomNumber);

		return true;
	}
	else if (chunkId->EqualsTo(m_chunkItemHitPoints))
	{
		item->hitPoints = LEB128::ReadInt16(m_stream);

		return true;
	}
	else if (chunkId->EqualsTo(m_chunkItemFlags))
	{
		item->flags = LEB128::ReadInt16(m_stream);
		byte active = LEB128::ReadByte(m_stream);
		item->status = LEB128::ReadByte(m_stream);
		item->gravityStatus = LEB128::ReadByte(m_stream);
		item->hitStatus = LEB128::ReadByte(m_stream);
		item->collidable = LEB128::ReadByte(m_stream);
		item->lookedAt = LEB128::ReadByte(m_stream);
		item->dynamicLight = LEB128::ReadByte(m_stream);
		item->poisoned = LEB128::ReadByte(m_stream);
		item->aiBits = LEB128::ReadByte(m_stream);
		item->reallyActive = LEB128::ReadByte(m_stream);
		item->triggerFlags = LEB128::ReadInt16(m_stream);
		item->timer = LEB128::ReadByte(m_stream);
		item->itemFlags[0] = LEB128::ReadInt16(m_stream);
		item->itemFlags[1] = LEB128::ReadInt16(m_stream);
		item->itemFlags[2] = LEB128::ReadInt16(m_stream);
		item->itemFlags[3] = LEB128::ReadInt16(m_stream);

		if (active && !item->active)
			AddActiveItem(itemNumber);

		return true;
	}
	else if (chunkId->EqualsTo(m_chunkItemIntelligentData))
	{
		EnableBaddieAI(itemNumber, 1);

		OBJECT_INFO* obj = &Objects[item->objectNumber];
		CREATURE_INFO* creature = (CREATURE_INFO*)item->data;

		creature->jointRotation[0] = LEB128::ReadInt16(m_stream);
		creature->jointRotation[1] = LEB128::ReadInt16(m_stream);
		creature->jointRotation[2] = LEB128::ReadInt16(m_stream);
		creature->jointRotation[3] = LEB128::ReadInt16(m_stream);
		creature->maximumTurn = LEB128::ReadInt16(m_stream);
		creature->flags = LEB128::ReadInt16(m_stream);
		creature->alerted = LEB128::ReadByte(m_stream);
		creature->headLeft = LEB128::ReadByte(m_stream);
		creature->headRight = LEB128::ReadByte(m_stream);
		creature->reachedGoal = LEB128::ReadByte(m_stream);
		creature->hurtByLara = LEB128::ReadByte(m_stream);
		creature->patrol2 = LEB128::ReadByte(m_stream);
		creature->jumpAhead = LEB128::ReadByte(m_stream);
		creature->monkeyAhead = LEB128::ReadByte(m_stream);
		creature->mood = (MOOD_TYPE)LEB128::ReadInt32(m_stream);

		ITEM_INFO* enemy = (ITEM_INFO*)LEB128::ReadLong(m_stream);
		creature->enemy = ADD_PTR(enemy, ITEM_INFO, MallocBuffer);

		creature->aiTarget.objectNumber = LEB128::ReadInt16(m_stream);
		creature->aiTarget.roomNumber = LEB128::ReadInt16(m_stream);
		creature->aiTarget.boxNumber = LEB128::ReadInt16(m_stream);
		creature->aiTarget.flags = LEB128::ReadInt16(m_stream);
		creature->aiTarget.pos.xPos = LEB128::ReadInt32(m_stream);
		creature->aiTarget.pos.yPos = LEB128::ReadInt32(m_stream);
		creature->aiTarget.pos.zPos = LEB128::ReadInt32(m_stream);
		creature->aiTarget.pos.xRot = LEB128::ReadInt16(m_stream);
		creature->aiTarget.pos.yRot = LEB128::ReadInt16(m_stream);
		creature->aiTarget.pos.zRot = LEB128::ReadInt16(m_stream);

		creature->LOT.canJump = LEB128::ReadByte(m_stream);
		creature->LOT.canMonkey = LEB128::ReadByte(m_stream);
		creature->LOT.isAmphibious = LEB128::ReadByte(m_stream);
		creature->LOT.isJumping = LEB128::ReadByte(m_stream);
		creature->LOT.isMonkeying = LEB128::ReadByte(m_stream);

		return true;
	}
	else if (chunkId->EqualsTo(m_chunkItemQuadInfo))
	{
		QUAD_INFO* quadInfo = (QUAD_INFO*)GameMalloc(sizeof(QUAD_INFO));
		m_stream->ReadBytes(reinterpret_cast<byte*>(quadInfo), sizeof(QUAD_INFO));
		if (item->objectNumber == ID_QUAD)
			item->data = (void*)quadInfo;

		return true;
	}

	return false;
}

void SaveGame::saveStatistics(int arg1, int arg2)
{
	LEB128::Write(m_stream, Savegame.Game.AmmoHits);
	LEB128::Write(m_stream, Savegame.Game.AmmoUsed);
	LEB128::Write(m_stream, Savegame.Game.Distance);
	LEB128::Write(m_stream, Savegame.Game.HealthUsed);
	LEB128::Write(m_stream, Savegame.Game.Kills);
	LEB128::Write(m_stream, Savegame.Game.Secrets);
	LEB128::Write(m_stream, Savegame.Game.Timer);

	LEB128::Write(m_stream, Savegame.Level.AmmoHits);
	LEB128::Write(m_stream, Savegame.Level.AmmoUsed);
	LEB128::Write(m_stream, Savegame.Level.Distance);
	LEB128::Write(m_stream, Savegame.Level.HealthUsed);
	LEB128::Write(m_stream, Savegame.Level.Kills);
	LEB128::Write(m_stream, Savegame.Level.Secrets);
	LEB128::Write(m_stream, Savegame.Level.Timer);
}

bool SaveGame::readStatistics()
{
	Savegame.Game.AmmoHits = LEB128::ReadInt32(m_stream);
	Savegame.Game.AmmoUsed = LEB128::ReadInt32(m_stream);
	Savegame.Game.Distance = LEB128::ReadInt32(m_stream);
	Savegame.Game.HealthUsed = LEB128::ReadByte(m_stream);
	Savegame.Game.Kills = LEB128::ReadInt16(m_stream);
	Savegame.Game.Secrets = LEB128::ReadByte(m_stream);
	Savegame.Game.Timer = LEB128::ReadInt32(m_stream);

	Savegame.Level.AmmoHits = LEB128::ReadInt32(m_stream);
	Savegame.Level.AmmoUsed = LEB128::ReadInt32(m_stream);
	Savegame.Level.Distance = LEB128::ReadInt32(m_stream);
	Savegame.Level.HealthUsed = LEB128::ReadByte(m_stream);
	Savegame.Level.Kills = LEB128::ReadInt16(m_stream);
	Savegame.Level.Secrets = LEB128::ReadByte(m_stream);
	Savegame.Level.Timer = LEB128::ReadInt32(m_stream);

	return true;
}

void SaveGame::saveItemFlags(int arg1, int arg2)
{
	ITEM_INFO* item = &Items[arg1];
	OBJECT_INFO* obj = &Objects[item->objectNumber];

	LEB128::Write(m_stream, item->flags);
	LEB128::Write(m_stream, item->active);
	LEB128::Write(m_stream, item->status);
	LEB128::Write(m_stream, item->gravityStatus);
	LEB128::Write(m_stream, item->hitStatus);
	LEB128::Write(m_stream, item->collidable);
	LEB128::Write(m_stream, item->lookedAt);
	LEB128::Write(m_stream, item->dynamicLight);
	LEB128::Write(m_stream, item->poisoned);
	LEB128::Write(m_stream, item->aiBits);
	LEB128::Write(m_stream, item->reallyActive);
	LEB128::Write(m_stream, item->triggerFlags);
	LEB128::Write(m_stream, item->timer);
	LEB128::Write(m_stream, item->itemFlags[0]);
	LEB128::Write(m_stream, item->itemFlags[1]);
	LEB128::Write(m_stream, item->itemFlags[2]);
	LEB128::Write(m_stream, item->itemFlags[3]);
}

void SaveGame::saveItemIntelligentData(int arg1, int arg2)
{
	ITEM_INFO* item = &Items[arg1];
	OBJECT_INFO* obj = &Objects[item->objectNumber];
	CREATURE_INFO* creature = (CREATURE_INFO*)item->data;

	ITEM_INFO* enemy = (ITEM_INFO*)((char*)creature->enemy - (ptrdiff_t)MallocBuffer);

	LEB128::Write(m_stream, creature->jointRotation[0]);
	LEB128::Write(m_stream, creature->jointRotation[1]);
	LEB128::Write(m_stream, creature->jointRotation[2]);
	LEB128::Write(m_stream, creature->jointRotation[3]);
	LEB128::Write(m_stream, creature->maximumTurn);
	LEB128::Write(m_stream, creature->flags);
	LEB128::Write(m_stream, creature->alerted);
	LEB128::Write(m_stream, creature->headLeft);
	LEB128::Write(m_stream, creature->headRight);
	LEB128::Write(m_stream, creature->reachedGoal);
	LEB128::Write(m_stream, creature->hurtByLara);
	LEB128::Write(m_stream, creature->patrol2);
	LEB128::Write(m_stream, creature->jumpAhead);
	LEB128::Write(m_stream, creature->monkeyAhead);
	LEB128::Write(m_stream, creature->mood);
	LEB128::Write(m_stream, (int)enemy);

	LEB128::Write(m_stream, creature->aiTarget.objectNumber);
	LEB128::Write(m_stream, creature->aiTarget.roomNumber);
	LEB128::Write(m_stream, creature->aiTarget.boxNumber);
	LEB128::Write(m_stream, creature->aiTarget.flags);
	LEB128::Write(m_stream, creature->aiTarget.pos.xPos);
	LEB128::Write(m_stream, creature->aiTarget.pos.yPos);
	LEB128::Write(m_stream, creature->aiTarget.pos.zPos);
	LEB128::Write(m_stream, creature->aiTarget.pos.xRot);
	LEB128::Write(m_stream, creature->aiTarget.pos.yRot);
	LEB128::Write(m_stream, creature->aiTarget.pos.zRot);

	LEB128::Write(m_stream, creature->LOT.canJump);
	LEB128::Write(m_stream, creature->LOT.canMonkey);
	LEB128::Write(m_stream, creature->LOT.isAmphibious);
	LEB128::Write(m_stream, creature->LOT.isJumping);
	LEB128::Write(m_stream, creature->LOT.isMonkeying);
}

void SaveGame::saveItemHitPoints(int arg1, int arg2)
{
	ITEM_INFO* item = &Items[arg1];

	LEB128::Write(m_stream, item->hitPoints);
}

void SaveGame::saveItemPosition(int arg1, int arg2)
{
	ITEM_INFO* item = &Items[arg1];

	LEB128::Write(m_stream, item->pos.xPos);
	LEB128::Write(m_stream, item->pos.yPos);
	LEB128::Write(m_stream, item->pos.zPos);
	LEB128::Write(m_stream, item->pos.xRot);
	LEB128::Write(m_stream, item->pos.yRot);
	LEB128::Write(m_stream, item->pos.zRot);
	LEB128::Write(m_stream, item->roomNumber);
}

void SaveGame::saveItemMesh(int arg1, int arg2)
{
	ITEM_INFO* item = &Items[arg1];

}

void SaveGame::saveItemAnims(int arg1, int arg2)
{
	ITEM_INFO* item = &Items[arg1];

	LEB128::Write(m_stream, item->currentAnimState);
	LEB128::Write(m_stream, item->goalAnimState);
	LEB128::Write(m_stream, item->requiredAnimState);
	LEB128::Write(m_stream, item->animNumber);
	LEB128::Write(m_stream, item->frameNumber);
}

void SaveGame::saveBurningTorch(int itemNumber, int arg2)
{
	ITEM_INFO* item = &Items[itemNumber];

	LEB128::Write(m_stream, itemNumber);
	LEB128::Write(m_stream, item->pos.xPos);
	LEB128::Write(m_stream, item->pos.yPos);
	LEB128::Write(m_stream, item->pos.zPos);
	LEB128::Write(m_stream, item->pos.xRot);
	LEB128::Write(m_stream, item->pos.yRot);
	LEB128::Write(m_stream, item->pos.zRot);
	LEB128::Write(m_stream, item->roomNumber);
	LEB128::Write(m_stream, item->speed);
	LEB128::Write(m_stream, item->fallspeed);

	LEB128::Write(m_stream, item->itemFlags[2]);
}

void SaveGame::saveChaff(int itemNumber, int arg2)
{
	ITEM_INFO* item = &Items[itemNumber];

	LEB128::Write(m_stream, itemNumber);
	LEB128::Write(m_stream, item->pos.xPos);
	LEB128::Write(m_stream, item->pos.yPos);
	LEB128::Write(m_stream, item->pos.zPos);
	LEB128::Write(m_stream, item->pos.xRot);
	LEB128::Write(m_stream, item->pos.yRot);
	LEB128::Write(m_stream, item->pos.zRot);
	LEB128::Write(m_stream, item->roomNumber);
	LEB128::Write(m_stream, item->speed);
	LEB128::Write(m_stream, item->fallspeed);

	LEB128::Write(m_stream, item->itemFlags[0]);
	LEB128::Write(m_stream, item->itemFlags[1]);
}

void SaveGame::saveTorpedo(int itemNumber, int arg2)
{
	ITEM_INFO* item = &Items[itemNumber];

	LEB128::Write(m_stream, itemNumber);
	LEB128::Write(m_stream, item->pos.xPos);
	LEB128::Write(m_stream, item->pos.yPos);
	LEB128::Write(m_stream, item->pos.zPos);
	LEB128::Write(m_stream, item->pos.xRot);
	LEB128::Write(m_stream, item->pos.yRot);
	LEB128::Write(m_stream, item->pos.zRot);
	LEB128::Write(m_stream, item->roomNumber);
	LEB128::Write(m_stream, item->speed);
	LEB128::Write(m_stream, item->fallspeed);

	LEB128::Write(m_stream, item->itemFlags[0]);
	LEB128::Write(m_stream, item->itemFlags[1]);
	LEB128::Write(m_stream, item->currentAnimState);
	LEB128::Write(m_stream, item->goalAnimState);
	LEB128::Write(m_stream, item->requiredAnimState);
}

void SaveGame::saveCrossbowBolt(int itemNumber, int arg2)
{
	ITEM_INFO* item = &Items[itemNumber];

	LEB128::Write(m_stream, itemNumber);
	LEB128::Write(m_stream, item->pos.xPos);
	LEB128::Write(m_stream, item->pos.yPos);
	LEB128::Write(m_stream, item->pos.zPos);
	LEB128::Write(m_stream, item->pos.xRot);
	LEB128::Write(m_stream, item->pos.yRot);
	LEB128::Write(m_stream, item->pos.zRot);
	LEB128::Write(m_stream, item->roomNumber);
	LEB128::Write(m_stream, item->speed);
	LEB128::Write(m_stream, item->fallspeed);
}

void SaveGame::saveFlare(int itemNumber, int arg2)
{
	ITEM_INFO* item = &Items[itemNumber];

	LEB128::Write(m_stream, itemNumber);
	LEB128::Write(m_stream, item->pos.xPos);
	LEB128::Write(m_stream, item->pos.yPos);
	LEB128::Write(m_stream, item->pos.zPos);
	LEB128::Write(m_stream, item->pos.xRot);
	LEB128::Write(m_stream, item->pos.yRot);
	LEB128::Write(m_stream, item->pos.zRot);
	LEB128::Write(m_stream, item->roomNumber);
	LEB128::Write(m_stream, item->speed);
	LEB128::Write(m_stream, item->fallspeed);

	// Flare age
	LEB128::Write(m_stream, (int)item->data);
}

bool SaveGame::readBurningTorch()
{
	LEB128::ReadInt16(m_stream);
	
	short itemNumber = CreateItem();
	ITEM_INFO* item = &Items[itemNumber];

	item->objectNumber = ID_BURNING_TORCH_ITEM;
	item->pos.xPos = LEB128::ReadInt32(m_stream);
	item->pos.yPos = LEB128::ReadInt32(m_stream);
	item->pos.zPos = LEB128::ReadInt32(m_stream);
	item->pos.xRot = LEB128::ReadInt16(m_stream);
	item->pos.yRot = LEB128::ReadInt16(m_stream);
	item->pos.zRot = LEB128::ReadInt16(m_stream);
	item->roomNumber = LEB128::ReadInt16(m_stream);

	short oldXrot = item->pos.xRot;
	short oldYrot = item->pos.yRot;
	short oldZrot = item->pos.zRot;

	InitialiseItem(itemNumber);

	item->pos.xRot = oldXrot;
	item->pos.yRot = oldYrot;
	item->pos.zRot = oldZrot;

	item->speed = LEB128::ReadInt16(m_stream);
	item->fallspeed = LEB128::ReadInt16(m_stream);

	AddActiveItem(itemNumber);

	item->itemFlags[2] = LEB128::ReadInt16(m_stream);

	return true;
}

bool SaveGame::readChaff()
{
	LEB128::ReadInt16(m_stream);

	short itemNumber = CreateItem();
	ITEM_INFO* item = &Items[itemNumber];

	item->objectNumber = ID_CHAFF;
	item->pos.xPos = LEB128::ReadInt32(m_stream);
	item->pos.yPos = LEB128::ReadInt32(m_stream);
	item->pos.zPos = LEB128::ReadInt32(m_stream);
	item->pos.xRot = LEB128::ReadInt16(m_stream);
	item->pos.yRot = LEB128::ReadInt16(m_stream);
	item->pos.zRot = LEB128::ReadInt16(m_stream);
	item->roomNumber = LEB128::ReadInt16(m_stream);

	short oldXrot = item->pos.xRot;
	short oldYrot = item->pos.yRot;
	short oldZrot = item->pos.zRot;

	InitialiseItem(itemNumber);

	item->pos.xRot = oldXrot;
	item->pos.yRot = oldYrot;
	item->pos.zRot = oldZrot;

	item->speed = LEB128::ReadInt16(m_stream);
	item->fallspeed = LEB128::ReadInt16(m_stream);

	AddActiveItem(itemNumber);

	item->itemFlags[0] = LEB128::ReadInt16(m_stream);
	item->itemFlags[1] = LEB128::ReadInt16(m_stream);

	return true;
}

bool SaveGame::readCrossbowBolt()
{
	LEB128::ReadInt16(m_stream);

	short itemNumber = CreateItem();
	ITEM_INFO* item = &Items[itemNumber];

	item->objectNumber = ID_CROSSBOW_BOLT;
	item->pos.xPos = LEB128::ReadInt32(m_stream);
	item->pos.yPos = LEB128::ReadInt32(m_stream);
	item->pos.zPos = LEB128::ReadInt32(m_stream);
	item->pos.xRot = LEB128::ReadInt16(m_stream);
	item->pos.yRot = LEB128::ReadInt16(m_stream);
	item->pos.zRot = LEB128::ReadInt16(m_stream);
	item->roomNumber = LEB128::ReadInt16(m_stream);

	short oldXrot = item->pos.xRot;
	short oldYrot = item->pos.yRot;
	short oldZrot = item->pos.zRot;

	InitialiseItem(itemNumber);

	item->pos.xRot = oldXrot;
	item->pos.yRot = oldYrot;
	item->pos.zRot = oldZrot;

	item->speed = LEB128::ReadInt16(m_stream);
	item->fallspeed = LEB128::ReadInt16(m_stream);

	AddActiveItem(itemNumber);

	return true;
}

bool SaveGame::readFlare()
{
	LEB128::ReadInt16(m_stream);

	short itemNumber = CreateItem();
	ITEM_INFO* item = &Items[itemNumber];

	item->objectNumber = ID_FLARE_ITEM;
	item->pos.xPos = LEB128::ReadInt32(m_stream);
	item->pos.yPos = LEB128::ReadInt32(m_stream);
	item->pos.zPos = LEB128::ReadInt32(m_stream);
	item->pos.xRot = LEB128::ReadInt16(m_stream);
	item->pos.yRot = LEB128::ReadInt16(m_stream);
	item->pos.zRot = LEB128::ReadInt16(m_stream);
	item->roomNumber = LEB128::ReadInt16(m_stream);

	short oldXrot = item->pos.xRot;
	short oldYrot = item->pos.yRot;
	short oldZrot = item->pos.zRot;

	InitialiseItem(itemNumber);

	item->pos.xRot = oldXrot;
	item->pos.yRot = oldYrot;
	item->pos.zRot = oldZrot;

	item->speed = LEB128::ReadInt16(m_stream);
	item->fallspeed = LEB128::ReadInt16(m_stream);

	AddActiveItem(itemNumber);

	// Flare age
	item->data = (void*)LEB128::ReadInt32(m_stream);

	return true;
}

bool SaveGame::readTorpedo()
{
	LEB128::ReadInt16(m_stream);

	short itemNumber = CreateItem();
	ITEM_INFO* item = &Items[itemNumber];

	item->objectNumber = ID_TORPEDO;
	item->pos.xPos = LEB128::ReadInt32(m_stream);
	item->pos.yPos = LEB128::ReadInt32(m_stream);
	item->pos.zPos = LEB128::ReadInt32(m_stream);
	item->pos.xRot = LEB128::ReadInt16(m_stream);
	item->pos.yRot = LEB128::ReadInt16(m_stream);
	item->pos.zRot = LEB128::ReadInt16(m_stream);
	item->roomNumber = LEB128::ReadInt16(m_stream);

	short oldXrot = item->pos.xRot;
	short oldYrot = item->pos.yRot;
	short oldZrot = item->pos.zRot;

	InitialiseItem(itemNumber);

	item->pos.xRot = oldXrot;
	item->pos.yRot = oldYrot;
	item->pos.zRot = oldZrot;

	item->speed = LEB128::ReadInt16(m_stream);
	item->fallspeed = LEB128::ReadInt16(m_stream);

	AddActiveItem(itemNumber);

	item->itemFlags[0] = LEB128::ReadInt16(m_stream);
	item->itemFlags[1] = LEB128::ReadInt16(m_stream);
	item->currentAnimState = LEB128::ReadInt16(m_stream);
	item->goalAnimState = LEB128::ReadInt16(m_stream);
	item->requiredAnimState = LEB128::ReadInt16(m_stream);

	return true;
}

void SaveGame::saveItemQuadInfo(int itemNumber, int arg2)
{
	m_stream->WriteBytes(reinterpret_cast<byte*>(Items[itemNumber].data), sizeof(QUAD_INFO));
}

void SaveGame::saveRats(int arg1, int arg2)
{
	RAT_STRUCT buffer;
	memcpy(&buffer, &Rats[arg1], sizeof(RAT_STRUCT));
	LEB128::Write(m_stream, arg1);
	m_stream->Write(reinterpret_cast<char*>(&buffer), sizeof(RAT_STRUCT));
}

void SaveGame::saveBats(int arg1, int arg2)
{
	BAT_STRUCT buffer;
	memcpy(&buffer, &Bats[arg1], sizeof(BAT_STRUCT));
	LEB128::Write(m_stream, arg1);
	m_stream->Write(reinterpret_cast<char*>(&buffer), sizeof(BAT_STRUCT));
}

void SaveGame::saveSpiders(int arg1, int arg2)
{
	SPIDER_STRUCT buffer;
	memcpy(&buffer, &Spiders[arg1], sizeof(SPIDER_STRUCT));
	LEB128::Write(m_stream, arg1);
	m_stream->Write(reinterpret_cast<char*>(&buffer), sizeof(SPIDER_STRUCT));
}

bool SaveGame::readBats()
{
	int index = LEB128::ReadInt16(m_stream);

	BAT_STRUCT* bats = &Bats[index];

	char* buffer = (char*)malloc(sizeof(BAT_STRUCT));
	m_stream->Read(buffer, sizeof(BAT_STRUCT));
	BAT_STRUCT* b = reinterpret_cast<BAT_STRUCT*>(buffer);
	memcpy(&bats, b, sizeof(BAT_STRUCT));
	free(buffer);

	return true;
}

bool SaveGame::readRats()
{
	int index = LEB128::ReadInt16(m_stream);

	RAT_STRUCT* rats = &Rats[index];

	char* buffer = (char*)malloc(sizeof(RAT_STRUCT));
	m_stream->Read(buffer, sizeof(RAT_STRUCT));
	RAT_STRUCT* r = reinterpret_cast<RAT_STRUCT*>(buffer);
	memcpy(&rats, r, sizeof(RAT_STRUCT));
	free(buffer);

	return true;
}

bool SaveGame::readSpiders()
{
	int index = LEB128::ReadInt16(m_stream);

	SPIDER_STRUCT* spiders = &Spiders[index];

	char* buffer = (char*)malloc(sizeof(SPIDER_STRUCT));
	m_stream->Read(buffer, sizeof(SPIDER_STRUCT));
	SPIDER_STRUCT* s = reinterpret_cast<SPIDER_STRUCT*>(buffer);
	memcpy(&spiders, s, sizeof(SPIDER_STRUCT));
	free(buffer);

	return true;
}