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
#include"cookTorrance.h"
#include"disneyPBR.h"

#include"bumpMapField.h"
#include"skyDome.h"  // �X�J�C�h�[���ǉ�

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
CookTorranceModel cookTorranceModel;
DisneyPBRModel disneyPBRModel;

BumpMapField bumpMapField;
SkyDome skyDome;  // �X�J�C�h�[���ǉ�

//�|�[�Y�t���O
static	bool	pause = false;

// �V�F�[�_�[�؂�ւ��p�ϐ�
static int currentShaderIndex = 0;
static int maxShaderCount = 10;  // ���p�\�ȃV�F�[�_�[��
static bool spaceKeyPressed = false;  // �X�y�[�X�L�[�̑O����

// �V�F�[�_�[���ꗗ
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
	skyDome.InitSkyDome();  // �X�J�C�h�[��������
	//���f���n�̏�����
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
//�Q�[���V�[���I��
void FinalizeGame()
{
	CameraObject->Uninit();
	test2D.FinalizePolygon2D();
	
	//���f���n�̏I��
	test3D.FinalizePolygon3D();
	bumpMapField.FinalizeBumpMapField();
	skyDome.FinalizeSkyDome();  // �X�J�C�h�[���I������
	//�V�F�[�_�[�𗘗p�������f���̏I��
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
//�Q�[���V�[���X�V
void UpdateGame()
{
	if (GetPause() == false)//�|�[�Y���łȂ���΍X�V���s
	{
		// �X�y�[�X�L�[���͏����i�g���K�[�j
		bool currentSpaceKeyState = Keyboard_IsKeyDown(KK_SPACE);
		if (currentSpaceKeyState && !spaceKeyPressed)
		{
			// �X�y�[�X�L�[�������ꂽ�u��
			currentShaderIndex = (currentShaderIndex + 1) % maxShaderCount;
		}
		spaceKeyPressed = currentSpaceKeyState;

		CameraObject->Update();
		test2D.UpdatePolygon2D();
		
		//���f���n�̍X�V
		test3D.UpdatePolygon3D();
		bumpMapField.UpdateBumpMapField();
		skyDome.UpdateSkyDome();  // �X�J�C�h�[���X�V
		//�V�F�[�_�[�𗘗p�������f���̍X�V
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
//�Q�[���V�[���`��
void DrawGame()
{
	//3D�p�}�g���N�X�ݒ�
	SetDepthEnable(true);//���s�������L��
	CameraObject->Draw();

	ResetWorldViewProjection3D();
	
	// �X�J�C�h�[�����ŏ��ɕ`��i�w�i�Ƃ��āA�[�x�������ݖ����j
	skyDome.DrawSkyDome();

	//���f���n�̕`��i�[�x�e�X�g�L���Œʏ�`��j
	//test3D.DrawPolygon3D();
	bumpMapField.DrawBumpMapField();

	//�V�F�[�_�[�𗘗p�������f���̕`��i���ݑI�����ꂽ���̂̂݁j
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

	// 2D�p�}�g���N�X�ݒ�
	SetWorldViewProjection2D();
	SetDepthEnable(false);//���s����������
	test2D.DrawPolygon2D();
	
	// �J�����̃f�o�b�O����\���i�㕔�j
	CameraObject->DebugDraw();

	// ���݂̃V�F�[�_�[����\���i�����j
	char shaderInfo[256];
	sprintf_s(shaderInfo, "Current Shader: %s\nPress SPACE to switch", shaderNames[currentShaderIndex]);
	DrawTextDebugAtPosition(shaderInfo, 10, 150, 600, 100);
}


