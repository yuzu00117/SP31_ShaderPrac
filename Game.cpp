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
#include "Mosaic.h"
#include "horror.h"
#include "posterize.h"
#include "GaussianBlur.h"
#include "Bloom.h"

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
#include"toon1.h"
#include"toon2.h"  // ランプテクスチャトゥーン追加
#include"furShader.h"  // ファーシェーダー追加

#include"bumpMapField.h"
#include"waterField.h" // 水面フィールド
#include"skyDome.h"  // スカイドーム追加

//===============================================
//グローバル変数
 
Camera		*CameraObject;
Object2D	test2D;
Object3D    test3D;
static Mosaic g_Mosaic; // 既存（参考用に残す）
static Horror g_Horror; // 既存ホラーエフェクト
static Posterize g_Posterize; // 新規ポスタライズ
static GaussianBlur g_Blur;   // ガウスぼかし
static Bloom g_Bloom;         // Bloom

// ポストエフェクト切り替え
enum class PostEffectMode { Horror = 0, Mosaic = 1, Posterize = 2, Gaussian = 3, Bloom = 4, None = 5 };
static PostEffectMode g_PostMode = PostEffectMode::Horror;

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
Toon1Model toon1Model;  // Toonシェーダー追加
Toon2Model toon2Model;  // ランプテクスチャToonシェーダー追加
FurShaderModel furShaderModel;  // ファーシェーダー追加

BumpMapField bumpMapField;
WaterField waterField;  // 水面フィールド
SkyDome skyDome;  // スカイドーム追加

//ポーズフラグ
static	bool	pause = false;

// シェーダー切り替え用変数
static int currentShaderIndex = 0;
static int maxShaderCount = 13;  // 利用可能なシェーダー数（furShader追加で13個に）
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
    "Disney PBR",
    "Toon1",
    "Toon2 (Ramp Texture)",
    "Fur Shader"
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
	waterField.InitWaterField();
	// バンプマップの隣（+X方向）に配置。SIZE=5なので中心を+5移動で面がちょうど接する
	waterField.SetPosition(XMFLOAT3(5.0f, 0.0f, 0.0f));
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
	toon1Model.InitPolygonModel();
	toon2Model.InitPolygonModel();  // ランプテクスチャトゥーン初期化
	furShaderModel.InitPolygonModel();  // ファーシェーダー初期化

	// ポストエフェクト初期化
	g_Mosaic.Initialize(); // 既存（参考）
	g_Horror.Init();       // 既存ホラー
	g_Horror.SetGrayScaleEnabled(true);
	g_Horror.SetContrastPower(5.0f);
	g_Horror.SetNoiseScale(120.0f);
	g_Horror.SetNoiseAddPerFrame(XMFLOAT2(0.01f, 0.013f));

	g_Posterize.Init();    // 新規ポスタライズ
	g_Posterize.SetLevels(4);
	g_Posterize.SetContrastPower(1.0f);
	g_Posterize.SetGrayScaleEnabled(false);

	g_Blur.Init();         // Gaussian blur
	g_Blur.SetSigma(2.0f);
	g_Blur.SetRadius(4);

	g_Bloom.Init();        // Bloom
	g_Bloom.SetThreshold(1.0f);
	g_Bloom.SetSoftKnee(0.5f);
	g_Bloom.SetIntensity(0.8f);
	g_Bloom.SetSigma(2.0f);
	g_Bloom.SetRadius(4);
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
	waterField.FinalizeWaterField();
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
	toon1Model.FinalizePolygonModel();
	toon2Model.FinalizePolygonModel();  // ランプテクスチャトゥーン終了
	furShaderModel.FinalizePolygonModel();  // ファーシェーダー終了

	// ポストエフェクト終了
	g_Mosaic.Finalize();
	g_Horror.Finalize();
	g_Posterize.Finalize();
	g_Blur.Finalize();
	g_Bloom.Finalize();

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

		// Vキーでポストエフェクト切り替え（Horror -> Mosaic -> Posterize -> Gaussian -> Bloom -> None -> Horror ...）
		if (Keyboard_IsKeyDownTrigger(KK_V))
		{
			if      (g_PostMode == PostEffectMode::Horror)    g_PostMode = PostEffectMode::Mosaic;
			else if (g_PostMode == PostEffectMode::Mosaic)    g_PostMode = PostEffectMode::Posterize;
			else if (g_PostMode == PostEffectMode::Posterize) g_PostMode = PostEffectMode::Gaussian;
			else if (g_PostMode == PostEffectMode::Gaussian)  g_PostMode = PostEffectMode::Bloom;
			else if (g_PostMode == PostEffectMode::Bloom)     g_PostMode = PostEffectMode::None;
			else                                             g_PostMode = PostEffectMode::Horror;
		}

		CameraObject->Update();
		test2D.UpdatePolygon2D();
		
		//モデル系の更新
		test3D.UpdatePolygon3D();
		bumpMapField.UpdateBumpMapField();
		waterField.UpdateWaterField();
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
		toon1Model.UpdatePolygonModel();
		toon2Model.UpdatePolygonModel();  // ランプテクスチャトゥーン更新

		// ポストエフェクト更新
		g_Mosaic.Update();
		g_Horror.Update();
		g_Posterize.Update();
		g_Blur.Update();
		g_Bloom.Update();
	}
}

//===============================================
//ゲームシーン描画
void DrawGame()
{
	// 1pass: オフスクリーンへ（選択されたポストエフェクト）
	switch (g_PostMode)
	{
	case PostEffectMode::Horror:
		g_Horror.BeginScene();
		break;
	case PostEffectMode::Mosaic:
		g_Mosaic.BeginScene();
		break;
	case PostEffectMode::Posterize:
		g_Posterize.BeginScene();
		break;
	case PostEffectMode::Gaussian:
		g_Blur.BeginScene();
		break;
	case PostEffectMode::Bloom:
		g_Bloom.BeginScene();
		break;
	case PostEffectMode::None:
		// 何もしない（バックバッファに直接描画）
		BindBackBuffer();
		break;
	}

	//3D用マトリクス設定
	SetDepthEnable(true);//奥行き処理有効
	CameraObject->Draw();

	ResetWorldViewProjection3D();
	
	// スカイドームを最初に描画（背景として、深度書き込み無効）
	skyDome.DrawSkyDome();

	//モデル系の描画（深度テスト有効で通常描画）
	//test3D.DrawPolygon3D();
	bumpMapField.DrawBumpMapField();
	waterField.DrawWaterField();

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
	case 10:
		toon1Model.DrawPolygonModel();
		break;
	case 11:
		toon2Model.DrawPolygonModel();  // ランプテクスチャトゥーン描画
		break;
	case 12:
		furShaderModel.DrawPolygonModel();  // ファーシェーダー描画
		break;
	}

	// 2D用マトリクス設定
	SetWorldViewProjection2D();
	SetDepthEnable(false);//奥行き処理無効
	// モザイク選択中のみ中央マーカー（center_white.png）を描画（デモ用）
	if (g_PostMode == PostEffectMode::Mosaic)
	{
		test2D.DrawPolygon2D();
	}
	
	// Toon2のランプテクスチャを2Dスプライトとして表示（最終版）
	if (currentShaderIndex == 11) // Toon2が選択されている場合
	{
		int rampTexID = toon2Model.GetRampTextureID();
		
		// デバッグ出力
		char debugMsg[256];
		sprintf_s(debugMsg, "Game.cpp: currentShaderIndex=%d, rampTexID=%d\n", currentShaderIndex, rampTexID);
		OutputDebugStringA(debugMsg);
		
		if (rampTexID >= 0) // 有効なテクスチャIDの場合
		{
			// test2Dの直後なので、既に2D用のシェーダー設定が済んでいる
			// テクスチャだけを変更してスプライトを描画
			ID3D11ShaderResourceView* tex = GetTexture(rampTexID);
			if (tex != NULL) {
				GetDeviceContext()->PSSetShaderResources(0, 1, &tex);
				
				// ワールド行列を左上位置に設定
				XMMATRIX TranslationMatrix = XMMatrixTranslation(SCREEN_WIDTH - 64, 64.0f, 0.0f);
				XMMATRIX RotationMatrix = XMMatrixRotationZ(0.0f);
				XMMATRIX ScalingMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
				XMMATRIX WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;
				SetWorldMatrix(WorldMatrix);
				
				// スプライト描画
				XMFLOAT2 spriteSize = XMFLOAT2(128.0f, 128.0f);
				XMFLOAT4 spriteColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				DrawSprite(spriteSize, spriteColor);
				
				OutputDebugStringA("Game.cpp: Sprite drawn successfully\n");
			}
			else
			{
				OutputDebugStringA("Game.cpp: Failed to get texture\n");
			}
		}
		else
		{
			OutputDebugStringA("Game.cpp: Invalid rampTexID\n");
		}
	}

	// 2pass: 選択したポストエフェクト → バックバッファへ
	switch (g_PostMode)
	{
	case PostEffectMode::Horror:
		g_Horror.EndSceneAndDraw();
		break;
	case PostEffectMode::Mosaic:
		g_Mosaic.EndSceneAndDraw();
		break;
	case PostEffectMode::Posterize:
		g_Posterize.EndSceneAndDraw();
		break;
	case PostEffectMode::Gaussian:
		g_Blur.EndSceneAndDraw();
		break;
	case PostEffectMode::Bloom:
		g_Bloom.EndSceneAndDraw();
		break;
	case PostEffectMode::None:
		// 何もしない
		break;
	}
	
	// カメラのデバッグ情報を表示（上部）
	CameraObject->DebugDraw();

	// 現在のシェーダー名を表示（下部）
	char shaderInfo[256];
	sprintf_s(shaderInfo, "Current Shader: %s\nPress SPACE to switch ShaderModel", shaderNames[currentShaderIndex]);
	sprintf_s(shaderInfo, "ShaderModdel (SPACE to switch): %s", shaderNames[currentShaderIndex]);
	DrawTextDebugAtPosition(shaderInfo, 10, 150, 600, 100);

	// 現在のポストエフェクト情報を表示（下部）
	const char* peName =
		(g_PostMode == PostEffectMode::Horror)    ? "Horror"   :
		(g_PostMode == PostEffectMode::Mosaic)    ? "Mosaic"   :
		(g_PostMode == PostEffectMode::Posterize) ? "Posterize" :
		(g_PostMode == PostEffectMode::Gaussian)  ? "Gaussian" :
		(g_PostMode == PostEffectMode::Bloom)     ? "Bloom"     : "None";
	char postInfo[256];
	sprintf_s(postInfo, "PostEffect (V to switch): %s", peName);
	DrawTextDebugAtPosition(postInfo, 10, 200, 800, 100);

	if (g_PostMode == PostEffectMode::Bloom)
	{
		char bloomInfo[128];
		sprintf_s(bloomInfo, "Bloom Intensity (J/K to -/+): %.2f", g_Bloom.GetIntensity());
		DrawTextDebugAtPosition(bloomInfo, 10, 230, 800, 100);
	}
}
