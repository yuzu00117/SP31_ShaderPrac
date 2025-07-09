#include "Main.h"
#include "Manager.h"
#include "Renderer.h"
#include "Sprite.h"
#include "keyboard.h"
#include "Game.h"

static SCENE g_Scene = SCENE_NONE;

void InitManager()
{
	InitSprite();
	SetScene(SCENE_GAME);

}

void FinalizeManager()
{
	FinalizeSprite();
	SetScene(SCENE_NONE);

}

void UpdateManager()
{


	switch (g_Scene)
	{
	case SCENE_NONE:
		break;

	case SCENE_TITLE:
		break;

	case SCENE_GAME:
		UpdateGame();
		break;

	case SCENE_RESULT:
		break;

	default:
		break;
	}

}

void DrawManager()
{

	switch (g_Scene)
	{
	case SCENE_NONE:
		break;

	case SCENE_TITLE:
		break;

	case SCENE_GAME:
		DrawGame();
		break;

	case SCENE_RESULT:
		break;

	default:
		break;
	}

}





void SetScene(SCENE Scene)
{
	//���݂̃V�[�����I��
	switch (g_Scene)
	{
	case SCENE_NONE:
		break;

	case SCENE_TITLE:
		break;

	case SCENE_GAME:
		FinalizeGame();
		break;

	case SCENE_RESULT:
		break;

	default:
		break;
	}


	//���̃V�[����������
	g_Scene = Scene;
	switch (g_Scene)
	{
	case SCENE_NONE:
		break;

	case SCENE_TITLE:
		break;

	case SCENE_GAME:
		InitGame();
		break;

	case SCENE_RESULT:
		break;

	default:
		break;
	}


}