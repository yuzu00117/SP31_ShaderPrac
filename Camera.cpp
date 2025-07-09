
//Camera.cpp

#include	"Camera.h"
#include	"keyboard.h"
#include <windows.h>
#include"DrawTextDebug.h"




void	Camera::Init()
{
	//カメラの初期化
	Position = XMFLOAT3(0.0f, 1.0f, -1.5f);	//カメラ基本座標
	UpVector = XMFLOAT3(0.0f, 1.0f, 0.0f);		//カメラの上方ベクトル
	AtPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);	//カメラの注視点座標
	Fov = 45.0f;								//画角
	Nearclip = 0.5f;							//近面クリップ
	Farclip = 1000.0f;							//遠面クリップ
	Rotation = 0.0f;

	ShowCursor(false); // カーソルを非表示にする
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

    // マウス位置取得
    POINT mousePos;
    GetCursorPos(&mousePos);

    // 初期化済みでない場合はスキップ
    static bool firstMouse = true;
    if (firstMouse)
    {
        m_PrevMousePos = mousePos;
        firstMouse = false;
        return;
    }

    // マウス移動量計算
    float dx = (float)(mousePos.x - m_PrevMousePos.x);
    float dy = (float)(mousePos.y - m_PrevMousePos.y);

    m_PrevMousePos = mousePos;

    // YawとPitch更新
    m_Yaw += dx * sensitivity;
    m_Pitch -= dy * sensitivity;

    // Pitch制限（見上げすぎ防止）
    if (m_Pitch > XM_PIDIV2 - 0.1f) m_Pitch = XM_PIDIV2 - 0.1f;
    if (m_Pitch < -XM_PIDIV2 + 0.1f) m_Pitch = -XM_PIDIV2 + 0.1f;

    // カメラの方向ベクトル計算
    XMVECTOR dir = XMVectorSet(
        cosf(m_Pitch) * sinf(m_Yaw),
        sinf(m_Pitch),
        cosf(m_Pitch) * cosf(m_Yaw),
        0.0f
    );

    // カメラターゲットを更新
    XMVECTOR pos = XMLoadFloat3(&Position);
    XMVECTOR target = pos + dir;
    XMStoreFloat3(&AtPosition, target);

    // WASDで移動
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

    // ←ここでtarget再計算（位置を動かした後！）
    XMVECTOR newTarget = pos + dir;
    XMStoreFloat3(&AtPosition, newTarget);

    // カーソルを画面中央に戻す
    SetCursorPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    m_PrevMousePos.x = SCREEN_WIDTH / 2;
    m_PrevMousePos.y = SCREEN_HEIGHT / 2;


}

void	Camera::Draw()//3D使用時
{
	//プロジェクション行列を作成
	XMMATRIX	ProjectionMatrix =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(Fov),
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			Nearclip,
			Farclip
		);
	//プロジェクション行列をセット
	SetProjectionMatrix(ProjectionMatrix);

	//カメラ行列を作成
    XMVECTOR pos = XMLoadFloat3(&Position);
    XMVECTOR target = XMLoadFloat3(&AtPosition);
    XMVECTOR up = XMLoadFloat3(&UpVector);
	XMMATRIX	ViewMatrix =
	XMMatrixLookAtLH(pos, target, up);

	//カメラ行列をセット
	SetViewMatrix(ViewMatrix);
	//カメラ座標セット
	SetCameraPosition(Position);

 
    
}

void Camera::DebugDraw()
{
	//カメラの位置をデバッグ表示
	char buf[256];
    sprintf_s(buf, "Pos     : x:%.2f y:%.2f z:%.2f\nTarget : x:%.2f y:%.2f z:%.2f",
        Position.x, Position.y, Position.z,
        AtPosition.x,AtPosition.y,AtPosition.z);
	DrawTextDebug(buf);
}




