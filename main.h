/*==============================================================================

   ���ʃw�b�_�[ [main.h]
                                                         Author : 
                                                         Date   : 
--------------------------------------------------------------------------------

==============================================================================*/
#pragma once


#pragma warning(push)
#pragma warning(disable:4005)


#define _CRT_SECURE_NO_WARNINGS			// scanf ��warning�h�~
#include <stdio.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#define DIRECTINPUT_VERSION 0x0800		// �x���Ώ�
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)



#include <DirectXMath.h>
using namespace DirectX;

#include "DirectXTex.h"//<<<<

#if _DEBUG
#pragma comment(lib, "DirectXTex_Debug.lib")
#else
#pragma comment(lib, "DirectXTex_Release.lib")
#endif

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SCREEN_WIDTH	(1280)				// �E�C���h�E�̕�
#define SCREEN_HEIGHT	(720)				// �E�C���h�E�̍���


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

