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




//�V�F�[�_�[�n�̌Ăяo��
#include"pixelLightBlinnPhong.h"
#include"pixeLighting.h"
#include"vertexDirectionaLighting.h"
#include"hemisphereLighting.h"
#include"unlitColor.h"
#include"pointLightingBlinnPhong.h"
#include"spotLighting.h"
#include"limLighting.h"

#include"bumpMapField.h"

//===============================================
//�O���[�o���ϐ�
 
Camera		*CameraObject;
Object2D	test2D;
Object3D    test3D;


//���f���n�̌Ăяo���@�V�F�[�_�[��
PixelLightingModel pixelLightingModel;
PixelLightBlinnPhongModel pixelLightBlinnPhongModel;
VertexDirectionalLightingModel vertexDirectionalLightingModel;
HemisphereLighting hemisphereLighting;
UnlitColorModel unlitColorModel;
PointLightBlinnPhongModel pointLightBlinnPhongModel;
SpotLightingModel spotLightingModel;
LimLightingModel limLightingModel;

BumpMapField bumpMapField;


//�|�[�Y�t���O
static	bool	pause = false;

//===============================================
//�|�[�Y�t���O�Z�b�g
void	SetPause(bool flg)
{
	pause = flg;
}
//===============================================
//�|�[�Y�t���O�擾
bool	GetPause()
{
	return pause;
}

//===============================================
//�Q�[���V�[��������
void InitGame()
{
	TextureInitialize(GetDevice());
	CameraObject = new Camera();
	CameraObject->Init();

	test2D.InitPolygon2D();

	

	//3D�I�u�W�F�N�g������
	test3D.InitPolygon3D();
	bumpMapField.InitBumpMapField();
	//���f���n�̏�����
	pixelLightingModel.InitPolygonModel();
	pixelLightBlinnPhongModel.InitPolygonModel();
	vertexDirectionalLightingModel.InitPolygonModel();
	hemisphereLighting.InitPolygonModel();
	unlitColorModel.InitPolygonModel();
	pointLightBlinnPhongModel.InitPolygonModel();
	spotLightingModel.InitPolygonModel();
	limLightingModel.InitPolygonModel();
	

	


}

//===============================================
//�Q�[���V�[���I��
void FinalizeGame()
{
	CameraObject->Uninit();
	test2D.FinalizePolygon2D();
	

	//���f���n�̏I��
	test3D.FinalizePolygon3D();
	bumpMapField.FinalizeBumpMapField();
	//�V�F�[�_�[�𗘗p�������f���̏I��
	pixelLightingModel.FinalizePolygonModel();
	pixelLightBlinnPhongModel.FinalizePolygonModel();
	vertexDirectionalLightingModel.FinalizePolygonModel();
	hemisphereLighting.FinalizePolygonModel();
	unlitColorModel.FinalizePolygonModel();
	pointLightBlinnPhongModel.FinalizePolygonModel();
	spotLightingModel.FinalizePolygonModel();
	limLightingModel.FinalizePolygonModel();

	TextureFinalize();
}

//===============================================
//�Q�[���V�[���X�V
void UpdateGame()
{

	if (GetPause() == false)//�|�[�Y���łȂ���΍X�V���s
	{
		CameraObject->Update();
		test2D.UpdatePolygon2D();
		

		//���f���n�̍X�V
		test3D.UpdatePolygon3D();
		bumpMapField.UpdateBumpMapField();
		//�V�F�[�_�[�𗘗p�������f���̍X�V
		pixelLightingModel.UpdatePolygonModel();
		pixelLightBlinnPhongModel.UpdatePolygonModel();
		vertexDirectionalLightingModel.UpdatePolygonModel();
		hemisphereLighting.UpdatePolygonModel();
		unlitColorModel.UpdatePolygonModel();
		pointLightBlinnPhongModel.UpdatePolygonModel();
		spotLightingModel.UpdatePolygonModel();
		limLightingModel.UpdatePolygonModel();

				
	}

}

//===============================================
//�Q�[���V�[���`��
void DrawGame()
{

	//3D�p�}�g���N�X�ݒ�
	SetDepthEnable(true);//���s�������L��
	CameraObject->Draw();

	ResetWorldViewProjection3D();
	

	//���f���n�̕`��
	//test3D.DrawPolygon3D();
	bumpMapField.DrawBumpMapField();

	//�V�F�[�_�[�𗘗p�������f���̕`��
	/*pixelLightingModel.DrawPolygonModel();
	pixelLightBlinnPhongModel.DrawPolygonModel();
	vertexDirectionalLightingModel.DrawPolygonModel();
	hemisphereLighting.DrawPolygonModel();
	unlitColorModel.DrawPolygonModel();*/
	pointLightBlinnPhongModel.DrawPolygonModel();
	spotLightingModel.DrawPolygonModel();
	limLightingModel.DrawPolygonModel();

	// 2D�p�}�g���N�X�ݒ�
	SetWorldViewProjection2D();
	SetDepthEnable(false);//���s����������
	test2D.DrawPolygon2D();
	CameraObject->DebugDraw();





}


