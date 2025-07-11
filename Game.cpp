#include "Main.h"
#include "Renderer.h"
#include "Manager.h"
#include "Sprite.h"
#include "Game.h"
#include "keyboard.h"
#include "Manager.h"
#include "Camera.h"
#include "texture.h"
#include"polygon2D.h"
#include"model.h"
#include"polygon3D.h"
#include"DrawTextDebug.h"

//シェーダー系の呼び出し
#include"pixelLightBlinnPhong.h"
#include"pixeLighting.h"
#include"vertexDirectionaLighting.h"
#include"hemisphereLighting.h"
#include"unlitColor.h"
#include"pointLightingBlinnPhong.h"
#include"spotLighting.h"
#include"limLighting.h"
#include"cookTorrance.h"
#include"disneyPBR.h"

#include"bumpMapField.h"
#include"skyDome.h"  // スカイドーム追加

//===============================================
//グローバル変数
 
Camera		*CameraObject;
Object2D	test2D;
Object3D    test3D;

//モデル系の呼び出し　シェーダー別
PixelLightingModel pixelLightingModel;
PixelLightBlinnPhongModel pixelLightBlinnPhongModel;
VertexDirectionalLightingModel vertexDirectionalLightingModel;
HemisphereLighting hemisphereLighting;
UnlitColorModel unlitColorModel;
PointLightBlinnPhongModel pointLightBlinnPhongModel;
SpotLightingModel spotLightingModel;
LimLightingModel limLightingModel;
CookTorranceModel cookTorranceModel;
DisneyPBRModel disneyPBRModel;

BumpMapField bumpMapField;
SkyDome skyDome;  // スカイドーム追加

//ポーズフラグ
static	bool	pause = false;

// シェーダー切り替え用変数
static int currentShaderIndex = 0;
static int maxShaderCount = 10;  // 利用可能なシェーダー数
static bool spaceKeyPressed = false;  // スペースキーの前回状態

// シェーダー名一覧
static const char* shaderNames[] = {
    "Pixel Lighting",
    "Pixel Light Blinn-Phong",
    "Vertex Directional Lighting",
    "Hemisphere Lighting",
    "Unlit Color",
    "Point Light Blinn-Phong",
    "Spot Lighting",
    "Rim Lighting",
    "Cook-Torrance",
    "Disney PBR"
};

//===============================================
//ポーズフラグセット
void	SetPause(bool flg)
{
	pause = flg;
}
//===============================================
//ポーズフラグ取得
bool	GetPause()
{
	return pause;
}

//===============================================
//ゲームシーン初期化
void InitGame()
{
	TextureInitialize(GetDevice());
	CameraObject = new Camera();
	CameraObject->Init();

	test2D.InitPolygon2D();

	//3Dオブジェクト初期化
	test3D.InitPolygon3D();
	bumpMapField.InitBumpMapField();
	skyDome.InitSkyDome();  // スカイドーム初期化
	//モデル系の初期化
	pixelLightingModel.InitPolygonModel();
	pixelLightBlinnPhongModel.InitPolygonModel();
	vertexDirectionalLightingModel.InitPolygonModel();
	hemisphereLighting.InitPolygonModel();
	unlitColorModel.InitPolygonModel();
	pointLightBlinnPhongModel.InitPolygonModel();
	spotLightingModel.InitPolygonModel();
	limLightingModel.InitPolygonModel();
	cookTorranceModel.InitPolygonModel();
	disneyPBRModel.InitPolygonModel();
}

//===============================================
//ゲームシーン終了
void FinalizeGame()
{
	CameraObject->Uninit();
	test2D.FinalizePolygon2D();
	
	//モデル系の終了
	test3D.FinalizePolygon3D();
	bumpMapField.FinalizeBumpMapField();
	skyDome.FinalizeSkyDome();  // スカイドーム終了処理
	//シェーダーを利用したモデルの終了
	pixelLightingModel.FinalizePolygonModel();
	pixelLightBlinnPhongModel.FinalizePolygonModel();
	vertexDirectionalLightingModel.FinalizePolygonModel();
	hemisphereLighting.FinalizePolygonModel();
	unlitColorModel.FinalizePolygonModel();
	pointLightBlinnPhongModel.FinalizePolygonModel();
	spotLightingModel.FinalizePolygonModel();
	limLightingModel.FinalizePolygonModel();
	cookTorranceModel.FinalizePolygonModel();
	disneyPBRModel.FinalizePolygonModel();

	TextureFinalize();
}

//===============================================
//ゲームシーン更新
void UpdateGame()
{
	if (GetPause() == false)//ポーズ中でなければ更新実行
	{
		// スペースキー入力処理（トリガー）
		bool currentSpaceKeyState = Keyboard_IsKeyDown(KK_SPACE);
		if (currentSpaceKeyState && !spaceKeyPressed)
		{
			// スペースキーが押された瞬間
			currentShaderIndex = (currentShaderIndex + 1) % maxShaderCount;
		}
		spaceKeyPressed = currentSpaceKeyState;

		CameraObject->Update();
		test2D.UpdatePolygon2D();
		
		//モデル系の更新
		test3D.UpdatePolygon3D();
		bumpMapField.UpdateBumpMapField();
		skyDome.UpdateSkyDome();  // スカイドーム更新
		//シェーダーを利用したモデルの更新
		pixelLightingModel.UpdatePolygonModel();
		pixelLightBlinnPhongModel.UpdatePolygonModel();
		vertexDirectionalLightingModel.UpdatePolygonModel();
		hemisphereLighting.UpdatePolygonModel();
		unlitColorModel.UpdatePolygonModel();
		pointLightBlinnPhongModel.UpdatePolygonModel();
		spotLightingModel.UpdatePolygonModel();
		limLightingModel.UpdatePolygonModel();
		cookTorranceModel.UpdatePolygonModel();
		disneyPBRModel.UpdatePolygonModel();
	}
}

//===============================================
//ゲームシーン描画
void DrawGame()
{
	//3D用マトリクス設定
	SetDepthEnable(true);//奥行き処理有効
	CameraObject->Draw();

	ResetWorldViewProjection3D();
	
	// スカイドームを最初に描画（背景として、深度書き込み無効）
	skyDome.DrawSkyDome();

	//モデル系の描画（深度テスト有効で通常描画）
	//test3D.DrawPolygon3D();
	bumpMapField.DrawBumpMapField();

	//シェーダーを利用したモデルの描画（現在選択されたもののみ）
	switch (currentShaderIndex)
	{
	case 0:
		pixelLightingModel.DrawPolygonModel();
		break;
	case 1:
		pixelLightBlinnPhongModel.DrawPolygonModel();
		break;
	case 2:
		vertexDirectionalLightingModel.DrawPolygonModel();
		break;
	case 3:
		hemisphereLighting.DrawPolygonModel();
		break;
	case 4:
		unlitColorModel.DrawPolygonModel();
		break;
	case 5:
		pointLightBlinnPhongModel.DrawPolygonModel();
		break;
	case 6:
		spotLightingModel.DrawPolygonModel();
		break;
	case 7:
		limLightingModel.DrawPolygonModel();
		break;
	case 8:
		cookTorranceModel.DrawPolygonModel();
		break;
	case 9:
		disneyPBRModel.DrawPolygonModel();
		break;
	}

	// 2D用マトリクス設定
	SetWorldViewProjection2D();
	SetDepthEnable(false);//奥行き処理無効
	test2D.DrawPolygon2D();
	
	// カメラのデバッグ情報を表示（上部）
	CameraObject->DebugDraw();

	// 現在のシェーダー名を表示（下部）
	char shaderInfo[256];
	sprintf_s(shaderInfo, "Current Shader: %s\nPress SPACE to switch", shaderNames[currentShaderIndex]);
	DrawTextDebugAtPosition(shaderInfo, 10, 150, 600, 100);
}


