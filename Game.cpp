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




#include "skyDome.h"
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
SkyDome skyDome;


//ポーズフラグ
static	bool	pause = false;

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
        skyDome.InitPolygonModel();
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
        skyDome.FinalizePolygonModel();
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
		CameraObject->Update();
		test2D.UpdatePolygon2D();
		

		//モデル系の更新
		test3D.UpdatePolygon3D();
                bumpMapField.UpdateBumpMapField();
                skyDome.UpdatePolygonModel();
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
	

	//モデル系の描画
	//test3D.DrawPolygon3D();
        bumpMapField.DrawBumpMapField();
        skyDome.DrawPolygonModel();

	//シェーダーを利用したモデルの描画
	/*pixelLightingModel.DrawPolygonModel();
	pixelLightBlinnPhongModel.DrawPolygonModel();
	vertexDirectionalLightingModel.DrawPolygonModel();
	hemisphereLighting.DrawPolygonModel();
	unlitColorModel.DrawPolygonModel();*/
	/*pointLightBlinnPhongModel.DrawPolygonModel();
	spotLightingModel.DrawPolygonModel();
	limLightingModel.DrawPolygonModel();*/
	//cookTorranceModel.DrawPolygonModel();
	disneyPBRModel.DrawPolygonModel();

	// 2D用マトリクス設定
	SetWorldViewProjection2D();
	SetDepthEnable(false);//奥行き処理無効
	test2D.DrawPolygon2D();
	CameraObject->DebugDraw();





}


