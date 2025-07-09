
//Camera.cpp

#include	"Camera.h"
#include	"keyboard.h"
#include <windows.h>
#include"DrawTextDebug.h"




void	Camera::Init()
{
	//�J�����̏�����
	Position = XMFLOAT3(0.0f, 1.0f, -1.5f);	//�J������{���W
	UpVector = XMFLOAT3(0.0f, 1.0f, 0.0f);		//�J�����̏���x�N�g��
	AtPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);	//�J�����̒����_���W
	Fov = 45.0f;								//��p
	Nearclip = 0.5f;							//�ߖʃN���b�v
	Farclip = 1000.0f;							//���ʃN���b�v
	Rotation = 0.0f;

	ShowCursor(false); // �J�[�\�����\���ɂ���
}

void	Camera::Uninit()
{

}

void	Camera::Update()
{

	/*Rotation = 0;

	if (Keyboard_IsKeyDown(KK_A))
	{
		Rotation = 0.3f;
	}
	else if (Keyboard_IsKeyDown(KK_D))
	{
		Rotation = -0.3f;
	}

	float co = cosf(XMConvertToRadians(Rotation));
	float si = sinf(XMConvertToRadians(Rotation));
	float posx = (Position.x * co) - (Position.z * si);
	float posz = (Position.x * si) + (Position.z * co);

	Position.x = posx;
	Position.z = posz;*/


    const float speed = 0.1f;
    const float sensitivity = 0.003f;

    // �}�E�X�ʒu�擾
    POINT mousePos;
    GetCursorPos(&mousePos);

    // �������ς݂łȂ��ꍇ�̓X�L�b�v
    static bool firstMouse = true;
    if (firstMouse)
    {
        m_PrevMousePos = mousePos;
        firstMouse = false;
        return;
    }

    // �}�E�X�ړ��ʌv�Z
    float dx = (float)(mousePos.x - m_PrevMousePos.x);
    float dy = (float)(mousePos.y - m_PrevMousePos.y);

    m_PrevMousePos = mousePos;

    // Yaw��Pitch�X�V
    m_Yaw += dx * sensitivity;
    m_Pitch -= dy * sensitivity;

    // Pitch�����i���グ�����h�~�j
    if (m_Pitch > XM_PIDIV2 - 0.1f) m_Pitch = XM_PIDIV2 - 0.1f;
    if (m_Pitch < -XM_PIDIV2 + 0.1f) m_Pitch = -XM_PIDIV2 + 0.1f;

    // �J�����̕����x�N�g���v�Z
    XMVECTOR dir = XMVectorSet(
        cosf(m_Pitch) * sinf(m_Yaw),
        sinf(m_Pitch),
        cosf(m_Pitch) * cosf(m_Yaw),
        0.0f
    );

    // �J�����^�[�Q�b�g���X�V
    XMVECTOR pos = XMLoadFloat3(&Position);
    XMVECTOR target = pos + dir;
    XMStoreFloat3(&AtPosition, target);

    // WASD�ňړ�
    XMVECTOR forward = XMVector3Normalize(dir);
    XMVECTOR right = XMVector3Normalize(XMVector3Cross(XMVectorSet(0, 1, 0, 0), forward));



    if (GetAsyncKeyState('W') & 0x8000)
        pos += forward * speed;
    if (GetAsyncKeyState('S') & 0x8000)
        pos -= forward * speed;
    if (GetAsyncKeyState('A') & 0x8000)
        pos -= right * speed;
    if (GetAsyncKeyState('D') & 0x8000)
        pos += right * speed;

    XMStoreFloat3(&Position, pos);

    // ��������target�Čv�Z�i�ʒu�𓮂�������I�j
    XMVECTOR newTarget = pos + dir;
    XMStoreFloat3(&AtPosition, newTarget);

    // �J�[�\������ʒ����ɖ߂�
    SetCursorPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    m_PrevMousePos.x = SCREEN_WIDTH / 2;
    m_PrevMousePos.y = SCREEN_HEIGHT / 2;


}

void	Camera::Draw()//3D�g�p��
{
	//�v���W�F�N�V�����s����쐬
	XMMATRIX	ProjectionMatrix =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(Fov),
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			Nearclip,
			Farclip
		);
	//�v���W�F�N�V�����s����Z�b�g
	SetProjectionMatrix(ProjectionMatrix);

	//�J�����s����쐬
    XMVECTOR pos = XMLoadFloat3(&Position);
    XMVECTOR target = XMLoadFloat3(&AtPosition);
    XMVECTOR up = XMLoadFloat3(&UpVector);
	XMMATRIX	ViewMatrix =
	XMMatrixLookAtLH(pos, target, up);

	//�J�����s����Z�b�g
	SetViewMatrix(ViewMatrix);
	//�J�������W�Z�b�g
	SetCameraPosition(Position);

 
    
}

void Camera::DebugDraw()
{
	//�J�����̈ʒu���f�o�b�O�\��
	char buf[256];
    sprintf_s(buf, "Pos     : x:%.2f y:%.2f z:%.2f\nTarget : x:%.2f y:%.2f z:%.2f",
        Position.x, Position.y, Position.z,
        AtPosition.x,AtPosition.y,AtPosition.z);
	DrawTextDebug(buf);
}




