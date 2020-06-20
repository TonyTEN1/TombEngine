#pragma once
#include "RenderEnums.h"
#include "StaticBuffer.h"
#include "LightBuffer.h"
#include "MiscBuffer.h"
#include "HUDBarBuffer.h"
#include "HUDBuffer.h"
#include "ShadowLightBuffer.h"
#include "RoomBuffer.h"
#include "ItemBuffer.h"
#include "CameraMatrixBuffer.h"
#include "Frustum.h"

#include "items.h"
#include "effect.h"
#include <IndexBuffer.h>
#include "VertexBuffer.h"
#include "RenderTarget2D.h"

class RenderTarget2D;

#define MESH_BITS(x) (1 << x)
#define DX11_RELEASE(x) if (x != NULL) x->Release()
#define DX11_DELETE(x) if (x != NULL) { delete x; x = NULL; }
constexpr auto NUM_ANIMATED_SETS = 1024;
constexpr auto MAX_LIGHTS_DRAW = 16384;
constexpr auto MAX_DYNAMIC_LIGHTS = 16384;
constexpr auto MAX_DRAW_STATICS = 16384;
constexpr auto MAX_BONES = 32;
constexpr auto MAX_SPRITES = 16384;
constexpr auto REFERENCE_RES_WIDTH = 800;
constexpr auto REFERENCE_RES_HEIGHT = 450;
constexpr auto HUD_UNIT_X = 1.0f / REFERENCE_RES_WIDTH;
constexpr auto HUD_UNIT_Y = 1.0f / REFERENCE_RES_HEIGHT;
constexpr auto HUD_ZERO_Y = -REFERENCE_RES_HEIGHT;

struct RendererDisplayMode
{
	int Width;
	int Height;
	int RefreshRate;
};
struct RendererVideoAdapter
{
	string Name;
	int Index;
	vector<RendererDisplayMode> DisplayModes;
};

struct RendererVertex
{
	Vector3 Position;
	Vector3 Normal;
	Vector2 UV;
	Vector4 Color;
	float Bone;
};


class Texture2D
{
public:
	ID3D11ShaderResourceView* ShaderResourceView;
	ID3D11Texture2D* Texture;

	Texture2D()
	{		
		
	}

	~Texture2D()
	{
		DX11_RELEASE(ShaderResourceView);
		DX11_RELEASE(Texture);
	}

	static Texture2D* LoadFromByteArray(ID3D11Device* device, int w, int h, byte* data)
	{
		Texture2D* texture = new Texture2D();

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = w;
		desc.Height = h;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		
		D3D11_SUBRESOURCE_DATA subresourceData; 
		subresourceData.pSysMem = data; 
		subresourceData.SysMemPitch = w * 4; 
		subresourceData.SysMemSlicePitch = 0;
		
		HRESULT res = device->CreateTexture2D(&desc, &subresourceData, &texture->Texture);
		if (FAILED(res))
			return NULL;

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderDesc;
		shaderDesc.Format = desc.Format;
		shaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderDesc.Texture2D.MostDetailedMip = 0;
		shaderDesc.Texture2D.MipLevels = 1;

		res = device->CreateShaderResourceView(texture->Texture, &shaderDesc, &texture->ShaderResourceView);
		if (FAILED(res))
			return NULL;
		
		return texture;
	}

	static Texture2D* LoadFromFile(ID3D11Device* device, const char* fileName)
	{
		Texture2D* texture = new Texture2D();

		wchar_t buffer[255];
		size_t converted = 0;
		mbstowcs_s(&converted, buffer, fileName, strlen(fileName));

		ID3D11Resource* resource = NULL;
		ID3D11DeviceContext* context = NULL;
		device->GetImmediateContext(&context);

		HRESULT res = CreateWICTextureFromFile(device, context, &buffer[0], &resource, &texture->ShaderResourceView, (size_t)0);
		if (FAILED(res))
			return NULL;

		resource->QueryInterface(IID_ID3D11Texture2D, (void **)&texture->Texture);
		
		return texture;
	}

	static Texture2D* LoadFromMemory(ID3D11Device* device, byte* data, int length)
	{
		Texture2D* texture = new Texture2D();

		ID3D11Resource* resource = NULL;
		ID3D11DeviceContext* context = NULL;
		device->GetImmediateContext(&context);

		HRESULT res = CreateWICTextureFromMemory(device, context, data, length, &resource, &texture->ShaderResourceView, (size_t)0);
		if (FAILED(res))
			return NULL;

		resource->QueryInterface(IID_ID3D11Texture2D, (void**)& texture->Texture);

		return texture;
	}
};


struct RendererHUDBar
{
	VertexBuffer vertexBufferBorder;
	IndexBuffer indexBufferBorder;
	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;
	/*
		Initialises a new Bar for rendering. the Coordinates are set in the Reference Resolution (default 800x600).
		The colors are setup like this
		0-----------1-----------2
		|           |           |
		3-----------4-----------5
		|           |           |
		6-----------7-----------8
	*/
	RendererHUDBar(ID3D11Device* m_device, int x, int y, int w, int h, int borderSize, array<Vector4, 9> colors);
};

struct RendererStringToDraw
{
	float X;
	float Y;
	int Flags;
	wstring String;
	Vector3 Color;
};

struct RendererPolygon 
{
	byte Shape;
	int AnimatedSet;
	int TextureId;
	int Distance;
	int Indices[4];
};

struct RendererBone 
{
	Vector3 Translation;
	Matrix GlobalTransform;
	Matrix Transform;
	Vector3 GlobalTranslation;
	vector<RendererBone*> Children;
	RendererBone* Parent;
	int Index;
	Vector3 ExtraRotation;
	byte ExtraRotationFlags;

	RendererBone(int index)
	{
		Index = index;
		ExtraRotationFlags = 0;
		Translation = Vector3(0, 0, 0);
		ExtraRotation = Vector3(0, 0, 0);
	}
};

struct RendererLight
{
	Vector3 Position;
	float Type;
	Vector3 Color;
	bool Dynamic;
	Vector4 Direction;
	float Intensity;
	float In;
	float Out;
	float Range;

	RendererLight()
	{
	}
};

struct RendererAnimatedTexture 
{
	int Id;
	Vector2 UV[4];
};

struct RendererAnimatedTextureSet 
{
	int NumTextures;
	vector<RendererAnimatedTexture> Textures;
};

struct RendererBucket
{
	vector<RendererVertex> Vertices;
	vector<int> Indices;
	vector<RendererPolygon> Polygons;
	vector<RendererPolygon> AnimatedPolygons;
	int StartVertex;
	int StartIndex;
	int NumTriangles;
	int NumVertices;
	int NumIndices;
};

struct RendererStatic
{
	int Id;
	short RoomIndex;
	MESH_INFO* Mesh;
	Matrix World;
};

struct RendererRoom 
{
	ROOM_INFO* Room;
	Vector4 AmbientLight;
	RendererBucket Buckets[NUM_BUCKETS];
	RendererBucket AnimatedBuckets[NUM_BUCKETS];
	vector<RendererLight> Lights;
	vector<RendererStatic> Statics;
	bool Visited;
	float Distance;
	int RoomNumber;
	vector<RendererLight*> LightsToDraw;
};

struct RendererRoomNode
{
	short From;
	short To;
	Vector4 ClipPort;
};

struct RendererItem
{
	int Id;
	ITEM_INFO* Item;
	Matrix World;
	Matrix Translation;
	Matrix Rotation;
	Matrix Scale;
	Matrix AnimationTransforms[32];
	int NumMeshes;
	vector<RendererLight*> Lights;
	bool DoneAnimations;
};

struct RendererMesh
{
	BoundingSphere Sphere;
	RendererBucket Buckets[NUM_BUCKETS];
	RendererBucket AnimatedBuckets[NUM_BUCKETS];
	vector<Vector3> Positions;
};

struct RendererEffect
{
	int Id;
	FX_INFO* Effect;
	Matrix World;
	RendererMesh* Mesh;
	vector<RendererLight*> Lights;
};

struct RendererObject
{
	int Id;
	vector<RendererMesh*> ObjectMeshes;
	RendererBone* Skeleton;
	vector<Matrix> AnimationTransforms;
	vector<Matrix> BindPoseTransforms;
	vector<RendererBone*> LinearizedBones;
	bool DoNotDraw;
	bool HasDataInBucket[NUM_BUCKETS];
	bool HasDataInAnimatedBucket[NUM_BUCKETS];

	~RendererObject()
	{
		/*for (int i = 0; i < ObjectMeshes.size(); i++)
			delete ObjectMeshes[i];*/
		for (int i = 0; i < LinearizedBones.size(); i++)
			delete LinearizedBones[i];
	}
};

struct RendererSprite
{
	int Width;
	int Height;
	Vector2 UV[4];
	Texture2D* Texture;
};

struct RendererSpriteSequence
{
	int Id;
	vector<RendererSprite*> SpritesList;
	int NumSprites;

	RendererSpriteSequence()
	{
		Id = 0;
		NumSprites = 0;
	}

	RendererSpriteSequence(int id, int num)
	{
		Id = id;
		NumSprites = num;
		SpritesList = vector<RendererSprite*>(NumSprites);
	}

	RendererSpriteSequence(const RendererSpriteSequence& rhs)
	{
		Id = rhs.Id;
		NumSprites = rhs.NumSprites;
		SpritesList = rhs.SpritesList;
	}

	RendererSpriteSequence& operator=(const RendererSpriteSequence& other)
	{
		if (this != &other)
		{
			Id = other.Id;
			NumSprites = other.NumSprites;
			SpritesList = vector<RendererSprite*>(NumSprites);
			std::copy(other.SpritesList.begin(), other.SpritesList.end(),back_inserter(SpritesList));
		}
		return *this;
	}
};

struct RendererSpriteToDraw
{
	RENDERER_SPRITE_TYPE Type;
	RendererSprite* Sprite;
	float Distance;
	float Scale;
	Vector3 pos;
	Vector3 vtx1;
	Vector3 vtx2;
	Vector3 vtx3;
	Vector3 vtx4;
	Vector4 color;
	float Rotation;
	float Width;
	float Height;
	BLEND_MODES BlendMode;
	Vector3 ConstrainAxis;
};

struct RendererLine3D
{
	Vector3 start;
	Vector3 end;
	Vector4 color;
};

struct RendererWeatherParticle
{
	float X, Y, Z;
	float AngleH;
	float AngleV;
	float Size;
	short Room;
	bool Reset;
	bool Draw;
};

struct RendererUnderwaterDustParticle
{
	float X, Y, Z;
	short Life;
	short Room;
	bool Reset;
};

struct RendererLine2D
{
	Vector2 Vertices[2];
	Vector4 Color;
};

class Renderer11
{
private:
	Frustum frustum;
	// Core DX11 objects
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	IDXGIDevice* m_dxgiDevice = nullptr;
	CommonStates* m_states = nullptr;
	ID3D11BlendState* m_subtractiveBlendState = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;
	D3D11_VIEWPORT m_viewport;
	D3D11_VIEWPORT m_shadowMapViewport;
	Viewport* m_viewportToolkit;
	vector<RendererVideoAdapter> m_adapters;

	// Main back buffer
	ID3D11RenderTargetView* m_backBufferRTV;
	ID3D11Texture2D* m_backBufferTexture;

	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11Texture2D* m_depthStencilTexture;

	RenderTarget2D m_dumpScreenRenderTarget;
	RenderTarget2D m_renderTarget;
	RenderTarget2D m_currentRenderTarget;
	RenderTarget2D m_shadowMap;

	// Shaders
	ID3D11VertexShader* m_vsRooms;
	ID3D11PixelShader* m_psRooms;
	ID3D11VertexShader* m_vsItems;
	ID3D11PixelShader* m_psItems;
	ID3D11VertexShader* m_vsHairs;
	ID3D11PixelShader* m_psHairs;
	ID3D11VertexShader* m_vsStatics;
	ID3D11PixelShader* m_psStatics;
	ID3D11VertexShader* m_vsSky;
	ID3D11PixelShader* m_psSky;
	ID3D11VertexShader* m_vsSprites;
	ID3D11PixelShader* m_psSprites;
	ID3D11VertexShader* m_vsSolid;
	ID3D11PixelShader* m_psSolid;
	ID3D11VertexShader* m_vsInventory;
	ID3D11PixelShader* m_psInventory;
	ID3D11VertexShader* m_vsFullScreenQuad;
	ID3D11PixelShader* m_psFullScreenQuad;
	ID3D11VertexShader* m_vsShadowMap;
	ID3D11PixelShader* m_psShadowMap;
	ID3D11VertexShader* m_vsHUD;
	ID3D11PixelShader* m_psHUDColor;
	ID3D11PixelShader* m_psHUDTexture;
	ID3D11PixelShader* m_psHUDBarColor;

	ID3D11ShaderResourceView* m_shadowMapRV;
	ID3D11Texture2D* m_shadowMapTexture;
	ID3D11DepthStencilView* m_shadowMapDSV;


	// Constant buffers
	T5M::Renderer::CCameraMatrixBuffer m_stCameraMatrices;
	ID3D11Buffer* m_cbCameraMatrices;
	CItemBuffer m_stItem;
	ID3D11Buffer* m_cbItem;
	CStaticBuffer m_stStatic;
	ID3D11Buffer* m_cbStatic;
	CLightBuffer m_stLights;
	ID3D11Buffer* m_cbLights;
	CMiscBuffer m_stMisc;
	ID3D11Buffer* m_cbMisc;
	CRoomBuffer m_stRoom;
	ID3D11Buffer* m_cbRoom;
	CShadowLightBuffer m_stShadowMap;
	ID3D11Buffer* m_cbShadowMap;
	CHUDBuffer m_stHUD;
	ID3D11Buffer* m_cbHUD;
	CHUDBarBuffer m_stHUDBar;
	ID3D11Buffer* m_cbHUDBar;
	// Text and sprites
	SpriteFont* m_gameFont;
	SpriteBatch* m_spriteBatch;
	vector<RendererStringToDraw> m_strings;
	int m_blinkColorValue;
	int m_blinkColorDirection;
	PrimitiveBatch<RendererVertex>* m_primitiveBatch;

	// System resources
	Texture2D* m_HUDBarBorderTexture;
	Texture2D* m_caustics[NUM_CAUSTICS_TEXTURES];
	Texture2D* m_binocularsTexture;
	Texture2D* m_whiteTexture;

	// Level data
	Texture2D* m_titleScreen;
	Texture2D* m_loadScreen;
	Texture2D* m_textureAtlas;
	Texture2D* m_skyTexture;
	Texture2D* m_logo;
	VertexBuffer m_roomsVertexBuffer;
	IndexBuffer m_roomsIndexBuffer;
	VertexBuffer m_moveablesVertexBuffer;
	IndexBuffer m_moveablesIndexBuffer;
	VertexBuffer m_staticsVertexBuffer;
	IndexBuffer m_staticsIndexBuffer;
	vector<RendererRoom> m_rooms;
	Matrix m_hairsMatrices[12];
	short m_numHairVertices;
	short m_numHairIndices;
	vector<RendererVertex> m_hairVertices;
	vector<short> m_hairIndices;
	vector<RendererRoom*> m_roomsToDraw;
	vector<RendererItem*> m_itemsToDraw;
	vector<RendererEffect*> m_effectsToDraw;
	vector<RendererStatic*> m_staticsToDraw;
	vector<RendererLight*> m_lightsToDraw;
	vector<RendererLight*> m_dynamicLights;
	vector<RendererSpriteToDraw*> m_spritesToDraw;
	vector<RendererLine3D*> m_lines3DToDraw;
	vector<RendererLine2D*> m_lines2DToDraw;
	vector<RendererLight*> m_tempItemLights;
	RendererSpriteToDraw* m_spritesBuffer;
	int m_nextSprite;
	RendererLine3D* m_lines3DBuffer;
	int m_nextLine3D;
	RendererLine2D* m_lines2DBuffer;
	int m_nextLine2D;
	RendererLight* m_shadowLight;
	RendererObject** m_moveableObjects;
	RendererObject** m_staticObjects;
	RendererSprite** m_sprites;
	int m_numMoveables;
	int m_numStatics;
	int m_numSprites;
	int m_numSpritesSequences;
	vector<RendererSpriteSequence> m_spriteSequences;
	unordered_map<unsigned int, RendererMesh*> m_meshPointersToMesh;
	Matrix m_LaraWorldMatrix;
	vector<RendererAnimatedTextureSet> m_animatedTextureSets;
	int m_numAnimatedTextureSets;
	int m_currentCausticsFrame;
	RendererUnderwaterDustParticle m_underwaterDustParticles[NUM_UNDERWATER_DUST_PARTICLES];
	bool m_firstUnderwaterDustParticles = true;
	vector<RendererMesh*> m_meshes;
	vector<Texture2D*> m_roomTextures;
	vector<Texture2D*> m_moveablesTextures;
	vector<Texture2D*> m_staticsTextures;
	vector<Texture2D*> m_spritesTextures;

	// Debug variables
	int m_numDrawCalls = 0;

	// Preallocated pools of objects for avoiding new/delete
	// Items and effects are safe (can't be more than 1024 items in TR), 
	// lights should be oversized (eventually ignore lights more than MAX_LIGHTS)
	RendererItem m_items[NUM_ITEMS];
	RendererEffect m_effects[NUM_ITEMS];
	RendererLight m_lights[MAX_LIGHTS];
	int m_nextLight;
	int m_currentY;

	// Times for debug
	int m_timeUpdate;
	int m_timeDraw;
	int m_timeFrame;

	// Others
	bool m_firstWeather;
	RendererWeatherParticle	m_rain[NUM_RAIN_DROPS];
	RendererWeatherParticle	m_snow[NUM_SNOW_PARTICLES];
	RENDERER_FADE_STATUS m_fadeStatus;
	float m_fadeFactor;
	int m_progress;
	bool m_enableCinematicBars = false;
	int m_pickupRotation;

	// Private functions
	bool											drawScene(bool dump);
	bool											drawAllStrings();
	ID3D11VertexShader*								compileVertexShader(const wchar_t * fileName, const char* function, const char* model, ID3D10Blob** bytecode);
	ID3D11GeometryShader*							compileGeometryShader(const wchar_t * fileName);
	ID3D11PixelShader*								compilePixelShader(const wchar_t * fileName, const char* function, const char* model, ID3D10Blob** bytecode);
	ID3D11ComputeShader*							compileComputeShader(const wchar_t * fileName);
	ID3D11Buffer*									createConstantBuffer(size_t size);
	int												getAnimatedTextureInfo(short textureId);
	void											initialiseHairRemaps();
	RendererMesh*									getRendererMeshFromTrMesh(RendererObject* obj, short* meshPtr, short boneIndex, int isJoints, int isHairs);
	void											fromTrAngle(Matrix* matrix, short* frameptr, int index);
	void											buildHierarchy(RendererObject* obj);
	void											buildHierarchyRecursive(RendererObject* obj, RendererBone* node, RendererBone* parentNode);
	void											updateAnimation(RendererItem* item, RendererObject* obj, short** frmptr, short frac, short rate, int mask,bool useObjectWorldRotation = false);
	bool											printDebugMessage(int x, int y, int alpha, byte r, byte g, byte b, LPCSTR Message);
	bool											checkPortal(short roomIndex, short* portal, Vector4* viewPort, Vector4* clipPort);
	void											getVisibleRooms(int from, int to, Vector4* viewPort, bool water, int count);
	void											collectRooms();
	void											collectItems(short roomNumber);
	void											collectStatics(short roomNumber);
	void											collectLightsForRoom(short roomNumber);
	void											collectLightsForItem(short roomNumber, RendererItem* item);
	void											collectLightsForEffect(short roomNumber, RendererEffect* effect);
	void											collectEffects(short roomNumber);
	void											prepareLights();
	void											clearSceneItems();
	bool											updateConstantBuffer(ID3D11Buffer* buffer, void* data, int size);
	void											updateItemsAnimations();
	void											updateEffects();
	int												getFrame(short animation, short frame, short** framePtr, int* rate);
	bool											drawAmbientCubeMap(short roomNumber);
	bool											sphereBoxIntersection(Vector3 boxMin, Vector3 boxMax, Vector3 sphereCentre, float sphereRadius);
	bool											drawHorizonAndSky();
	bool											drawRooms(bool transparent, bool animated);
	bool											drawStatics(bool transparent);
	bool											drawItems(bool transparent, bool animated);
	bool											drawAnimatingItem(RendererItem* item, bool transparent, bool animated);
	bool											drawBaddieGunflashes();
	bool											drawScaledSpikes(RendererItem* item, bool transparent, bool animated);
	bool											drawWaterfalls();
	bool											drawShadowMap();
	bool											drawObjectOn2DPosition(short x, short y, short objectNum, short rotX, short rotY, short rotZ);
	bool											drawLara(bool transparent, bool shadowMap);
	void											printDebugMessage(LPCSTR message, ...);
	void											drawFires();
	void											drawSparks();
	void											drawSmokes();
	void											drawEnergyArcs();
	void											drawBlood();
	void											drawDrips();
	void											drawBubbles();
	bool											drawEffects(bool transparent);
	bool											drawEffect(RendererEffect* effect, bool transparent);
	void											drawSplahes();
	bool											drawSprites();
	bool											drawLines3D();
	bool											drawLines2D();
	bool											drawOverlays();
	bool											drawRopes();
	bool											drawBats();
	bool											drawRats();
	bool											drawSpiders();
	bool											drawGunFlashes();
	bool											drawGunShells();
	bool											drawDebris(bool transparent);
	int												drawInventoryScene();
	int												drawFinalPass();
	void											updateAnimatedTextures();
	void											createBillboardMatrix(Matrix* out, Vector3* particlePos, Vector3* cameraPos, float rotation);
	void											drawShockwaves();
	void											drawRipples();
	void											drawUnderwaterDust();	
	bool											doRain();
	bool											doSnow();
	bool											drawFullScreenQuad(ID3D11ShaderResourceView* texture, Vector3 color, bool cinematicBars);
	bool											drawFullScreenImage(ID3D11ShaderResourceView* texture, float fade);
	bool											isRoomUnderwater(short roomNumber);
	bool											isInRoom(int x, int y, int z, short roomNumber);
	bool											drawColoredQuad(int x, int y, int w, int h, Vector4 color);
	bool											initialiseScreen(int w, int h, int refreshRate, bool windowed, HWND handle, bool reset);
	bool											initialiseBars();
	bool											drawSmokeParticles();
	bool											drawSparkParticles();
	bool                                            drawDripParticles();
	bool											drawExplosionParticles();
public:
	Matrix View;
	Matrix Projection;
	Matrix ViewProjection;
	float FieldOfView;
	int ScreenWidth;
	int ScreenHeight;
	bool Windowed;
	int NumTexturePages;

	Renderer11();
	~Renderer11();

	bool Create();
	bool EnumerateVideoModes();
	bool Initialise(int w, int h, int refreshRate, bool windowed, HWND handle);
	int Draw();
	bool PrepareDataForTheRenderer();
	void UpdateCameraMatrices(float posX, float posY, float posZ, float targetX, float targetY, float targetZ, float roll, float fov);
	int DumpGameScene();
	int DrawInventory();
	int DrawPickup(short objectNum);
	int SyncRenderer();
	bool PrintString(int x, int y, char* string, D3DCOLOR color, int flags);
	void ClearDynamicLights();
	void AddDynamicLight(int x, int y, int z, short falloff, byte r, byte g, byte b);
	void FreeRendererData();
	void EnableCinematicBars(bool value);
	void FadeIn();
	void FadeOut();
	void DrawLoadingScreen(char* fileName);
	void UpdateProgress(float value);
	bool IsFading();
	void GetLaraBonePosition(Vector3* pos, int bone);
	bool ToggleFullScreen();
	bool IsFullsScreen();
	vector<RendererVideoAdapter>*					GetAdapters();
	bool DoTitleImage();
	void AddLine2D(int x1, int y1, int x2, int y2, byte r, byte g, byte b, byte a);
	void AddSpriteBillboard(RendererSprite* sprite, Vector3 pos,Vector4 color, float rotation, float scale, float width, float height, BLEND_MODES blendMode);
	void AddSpriteBillboardConstrained(RendererSprite* sprite, Vector3 pos, Vector4 color, float rotation, float scale, float width, float height, BLEND_MODES blendMode, Vector3 constrainAxis);
	void AddSprite3D(RendererSprite* sprite, Vector3 vtx1, Vector3 vtx2, Vector3 vtx3, Vector3 vtx4, Vector4 color, float rotation, float scale, float width, float height, BLEND_MODES blendMode);
	void AddLine3D(Vector3 start, Vector3 end, Vector4 color);
	bool ChangeScreenResolution(int width, int height, int frequency, bool windowed);
	bool DrawBar(float percent, const RendererHUDBar* const bar);
	void FlipRooms(short roomNumber1, short roomNumber2);
	void ResetAnimations();
	void UpdateLaraAnimations(bool force);
	void UpdateItemAnimations(int itemNumber, bool force);
	void GetLaraAbsBonePosition(Vector3* pos, int joint);
	void GetItemAbsBonePosition(int itemNumber, Vector3* pos, int joint);
	int GetSpheres(short itemNumber, BoundingSphere* ptr, char worldSpace, Matrix local);
	void GetBoneMatrix(short itemNumber, int joint, Matrix* outMatrix);

	RendererMesh* getMeshFromMeshPtr(unsigned int meshp);
private:
	void drawFootprints();
	void prepareCameraForFrame();
};
extern Renderer11* g_Renderer;