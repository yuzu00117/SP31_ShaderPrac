
#pragma once

//Camera.h
#include "main.h"
#include "renderer.h"


class Camera
{
	public:
		XMFLOAT3	Position;		//�J�����̍��W
		XMFLOAT3	AtPosition;		//�J�����̒����_
		XMFLOAT3	UpVector;		//����x�N�g��
		float		Fov;			//����p
		float		Nearclip;		//�ǂ��܂ŋ߂��������邩
		float		Farclip;		//�ǂ��܂ŉ����������邩
		float		Rotation;

		float m_Yaw = 0.0f;
		float m_Pitch = 0.0f;
		POINT m_PrevMousePos = {};

		


	void Init();
	void Update();
	void Draw();
	void DebugDraw();
	void Uninit();
};



