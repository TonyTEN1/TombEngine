#include "framework.h"
#include "Renderer11.h"
#include "input.h"
#include "winmain.h"
#include "level.h"
#include "configuration.h"
#include "draw.h"
#include "health.h"
#include "pickup.h"
#include "inventory.h"
#include "gameflow.h"
#include "Lara.h"
#include "effect2.h"
#include "rope.h"
#include "camera.h"
#include "tomb4fx.h"
#include "trmath.h"
#include "misc.h"
#include "footprint.h"
#include "setup.h"
#include "Utils.h"
#include "VertexBuffer/VertexBuffer.h"
#include "RenderView/RenderView.h"
using std::vector;
namespace T5M::Renderer {
	using namespace T5M::Renderer::Utils;
	using std::array;
	Renderer11 g_Renderer;
	Renderer11::Renderer11() : gameCamera({ 0,0,0 }, { 0,0,1 }, {0,1,0},1,1,0,1,10,90) {

		m_blinkColorDirection = 1;
	}

	Renderer11::~Renderer11() {
		freeRendererData();
	}

	void Renderer11::freeRendererData()
{
		m_meshPointersToMesh.clear();
		m_moveableObjects.clear();
		m_staticObjects.clear();
		m_sprites.clear();
		m_rooms.clear();
		m_roomTextures.clear();
		m_moveablesTextures.clear();
		m_staticsTextures.clear();
		m_spritesTextures.clear();
		m_animatedTextures.clear();
		m_animatedTextureSets.clear();
		gameCamera.clear();
	}

	void Renderer11::clearSceneItems() {
		m_lines3DToDraw.clear();
		m_lines2DToDraw.clear();
		gameCamera.clear();
	}

	int Renderer11::SyncRenderer() {
		// Sync the renderer
		int nf = Sync();
		if (nf < 2) {
			int i = 2 - nf;
			nf = 2;
			do {
				while (!Sync());
				i--;
			} while (i);
		}

		GnFrameCounter++;
		return nf;
	}

	void Renderer11::updateProgress(float value) {
		m_progress = value;
	}



	void Renderer11::renderToCubemap(const RenderTargetCube& dest,const Vector3& pos,int roomNumer) {
		for (int i = 0; i < 6; i++) {
			RenderView renderView = RenderView(pos,RenderTargetCube::forwardVectors[i],RenderTargetCube::upVectors[i],dest.resolution,dest.resolution,Camera.pos.roomNumber,10,20480, 90* RADIAN);
			renderSimpleScene(dest.RenderTargetView[i].Get(), dest.DepthStencilView[i].Get(), renderView);
			m_context->ClearState();
		}
	}

	RendererHUDBar::RendererHUDBar(ID3D11Device* m_device, int x, int y, int w, int h, int borderSize, array<Vector4, 9> colors) {
		array<Vector3, 9> barVertices = {
			Vector3(x, HUD_ZERO_Y + y, 0.5),
			Vector3(x + (w / 2.0f), HUD_ZERO_Y + y, 0.5),
			Vector3(x + w, HUD_ZERO_Y + y, 0.5),
			Vector3(x, HUD_ZERO_Y + (y + h / 2.0f), 0.5),
			Vector3(x + (w / 2.0f), HUD_ZERO_Y + (y + h / 2.0f), 0.5),
			Vector3(x + w, HUD_ZERO_Y + (y + h / 2.0f), 0.5),
			Vector3(x, HUD_ZERO_Y + y + h, 0.5),
			Vector3(x + (w / 2.0f), HUD_ZERO_Y + y + h, 0.5),
			Vector3(x + w, HUD_ZERO_Y + y + h, 0.5),

		};
		const float HUD_BORDER_WIDTH = borderSize * (REFERENCE_RES_WIDTH / REFERENCE_RES_HEIGHT);
		const float HUD_BORDER_HEIGT = borderSize;
		array<Vector3, 16> barBorderVertices = {
			//top left
			Vector3(x - HUD_BORDER_WIDTH	,HUD_ZERO_Y + y - HUD_BORDER_HEIGT,0),
			Vector3(x						,HUD_ZERO_Y + y - HUD_BORDER_HEIGT,0),
			Vector3(x						,HUD_ZERO_Y + y,0),
			Vector3(x - HUD_BORDER_WIDTH	,HUD_ZERO_Y + y,0),
			//top right
			Vector3(x + w					,HUD_ZERO_Y + y - HUD_BORDER_HEIGT,0),
			Vector3(x + w + HUD_BORDER_WIDTH,HUD_ZERO_Y + y - HUD_BORDER_HEIGT,0),
			Vector3(x + w + HUD_BORDER_WIDTH,HUD_ZERO_Y + y,0),
			Vector3(x + w					,HUD_ZERO_Y + y,0),
			//bottom right
			Vector3(x + w					,HUD_ZERO_Y + y + h,0),
			Vector3(x + w + HUD_BORDER_WIDTH,HUD_ZERO_Y + y + h,0),
			Vector3(x + w + HUD_BORDER_WIDTH,HUD_ZERO_Y + y + h + HUD_BORDER_HEIGT,0),
			Vector3(x + w					,HUD_ZERO_Y + y + h + HUD_BORDER_HEIGT,0),
			//bottom left
			Vector3(x - HUD_BORDER_WIDTH	,HUD_ZERO_Y + y + h,0),
			Vector3(x						,HUD_ZERO_Y + y + h,0),
			Vector3(x						,HUD_ZERO_Y + y + h + HUD_BORDER_HEIGT,0),
			Vector3(x - HUD_BORDER_WIDTH	,HUD_ZERO_Y + y + h + HUD_BORDER_HEIGT,0)
		};

		array<Vector2, 9> barUVs = {
			Vector2(0,0),
			Vector2(0.5,0),
			Vector2(1,0),
			Vector2(0,0.5),
			Vector2(0.5,0.5),
			Vector2(1,0.5),
			Vector2(0,1),
			Vector2(0.5,1),
			Vector2(1,1),
		};
		array<Vector2, 16> barBorderUVs = {
			//top left
			Vector2(0,0),
			Vector2(0.25,0),
			Vector2(0.25,0.25),
			Vector2(0,0.25),
			//top right
			Vector2(0.75,0),
			Vector2(1,0),
			Vector2(1,0.25),
			Vector2(0.75,0.25),
			//bottom right
			Vector2(0.75,0.75),
			Vector2(1,0.75),
			Vector2(1,1),
			Vector2(0.75,1),
			//bottom left
			Vector2(0,0.75),
			Vector2(0.25,0.75),
			Vector2(0.25,1),
			Vector2(0,1),
		};

		array<int, 24> barIndices = {
			0,1,3,1,4,3,
			//
			1,2,4,2,5,4,
			//
			3,4,6,4,7,6,
			//
			4,5,7,5,8,7
		};
		array<int, 56> barBorderIndices = {
			//top left
			0,1,3,1,2,3,
			//top center
			1,4,2,4,7,2,
			//top right
			4,5,7,5,6,7,
			//right
			7,6,8,6,9,8,
			//bottom right
			8,9,11,9,10,11,
			//bottom
			13,8,14,8,11,14,
			//bottom left
			12,13,15,13,14,15,
			//left
			3,2,12,2,13,12,
			//center
			2,7,13,7,8,13
		};
		array<RendererVertex, 9> vertices;
		for (int i = 0; i < 9; i++) {

			vertices[i].Position = barVertices[i];
			vertices[i].Color = colors[i];
			vertices[i].UV = barUVs[i];
			vertices[i].Normal = Vector3(0, 0, 0);
			vertices[i].Bone = 0.0f;
		}
		vertexBuffer = VertexBuffer(m_device, vertices.size(), vertices.data());
		indexBuffer = IndexBuffer(m_device, barIndices.size(), barIndices.data());

		array<RendererVertex, barBorderVertices.size()> verticesBorder;
		for (int i = 0; i < barBorderVertices.size(); i++) {
			verticesBorder[i].Position = barBorderVertices[i];
			verticesBorder[i].Color = Vector4(1, 1, 1, 1);
			verticesBorder[i].UV = barBorderUVs[i];
			verticesBorder[i].Normal = Vector3(0, 0, 0);
			verticesBorder[i].Bone = 0.0f;
		}
		vertexBufferBorder = VertexBuffer(m_device, verticesBorder.size(), verticesBorder.data());
		indexBufferBorder = IndexBuffer(m_device, barBorderIndices.size(), barBorderIndices.data());

	}

}
