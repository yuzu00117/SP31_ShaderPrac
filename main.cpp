
#include	"main.h"
#include	"renderer.h"
#include	"Manager.h"
#include	"keyboard.h"
//�f�o�b�N�p�����̃N���X
#include"DrawTextDebug.h"

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

//===================================
// ���C�u�����̃����N
//===================================




#pragma	comment (lib, "d3d11.lib")
#pragma	comment (lib, "d3dcompiler.lib")
#pragma	comment (lib, "winmm.lib")
#pragma	comment (lib, "dxguid.lib")
#pragma	comment (lib, "dinput8.lib")

//=================================
//�}�N����`
//=================================
#define		CLASS_NAME		"GameProject Window"
#define		WINDOW_CAPTION	"GameProject   <<�V�F�[�_�[�̒��̐l>>"

//===================================
//�v���g�^�C�v�錾
//===================================
//�R�[���o�b�N�֐��������l���Ăяo���Ă����֐�
LRESULT	CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//�������֐�
HRESULT	Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
//�I������
void	Finalize(void);
//�X�V����
void	Update(void);
//�`�揈��
void	Draw(void);

//==================================
//�O���[�o���ϐ�
//==================================
#ifdef _DEBUG	//�f�o�b�O���[�h���̂ݕϐ������
int	g_CountFPS;							//FPS�J�E���^�[
char g_DebugStr[2048] = WINDOW_CAPTION;	//�\��������ݒ�
#endif



#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
//=====================================
//���C���֐�
//======================================
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance, LPSTR lpCmd, int nCmdShow)
{
	// ���������[�N���o��L���ɂ���
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//����������
	srand((UINT)timeGetTime());
	//�t���[�����[�g�v���p�ϐ�
	DWORD	dwExecLastTime;
	DWORD	dwFPSLastTime;
	DWORD	dwCurrentTime;
	DWORD	dwFrameCount;
	//COM�R���|�[�l���g�̏����i�@�\�𕔕i�����ĊO���̃v���O�������狤�L���p����d�g�݁j
	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	//�E�B���h�E�N���X�̓o�^�i�E�B���h�E�̎d�l�I�ȕ������߂�Windows�փZ�b�g����j
	WNDCLASS	wc;	//�\���̂�����
	ZeroMemory(&wc, sizeof(WNDCLASS));//���e���O�ŏ�����
	wc.lpfnWndProc = WndProc;	//�R�[���o�b�N�֐��̃|�C���^�[
	wc.lpszClassName = CLASS_NAME;	//���̎d�l���̖��O
	wc.hInstance = hInstance;	//���̃A�v���P�[�V�����̂���
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);//�J�[�\���̎��
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);//�E�B���h�E�̔w�i�F
	RegisterClass(&wc);	//�\���̂�Windows�փZ�b�g


	//�E�B���h�E�T�C�Y�̒���
	//             ����@�@�E��
	RECT	rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };//��1280 �c720
	//�`��̈悪1280X720�ɂȂ�悤�ɃT�C�Y�𒲐�����
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME|WS_MAXIMIZEBOX|WS_MINIMIZEBOX), FALSE);

	//�E�B���h�E�̍쐬
	HWND	hWnd = CreateWindow(
		CLASS_NAME,	//��肽���E�B���h�E
		WINDOW_CAPTION,	//�E�B���h�E�ɕ\������^�C�g��
		WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX),//�W���I�Ȍ`��̃E�B���h�E �T�C�Y�ύX�֎~
		CW_USEDEFAULT,		//�f�t�H���g�ݒ�ł��܂���
		CW_USEDEFAULT,
		rc.right - rc.left,//CW_USEDEFAULT,//�E�B���h�E�̕�
		rc.bottom - rc.top,//CW_USEDEFAULT,//�E�B���h�E�̍���
		NULL,
		NULL,
		hInstance,		//�A�v���P�[�V�����̃n���h��
		NULL
	);

	//����������
	if (FAILED(Init(hInstance, hWnd, true)))
	{
		return -1;//���������s
	}

	//�쐬�����E�B���h�E��\������
	ShowWindow(hWnd, nCmdShow);//�����ɏ]���ĕ\���A�܂��͔�\��
	//�E�B���h�E�̓��e�������\��
	UpdateWindow(hWnd);

	//���b�Z�[�W���[�v
	MSG	msg;
	ZeroMemory(&msg, sizeof(MSG));//���b�Z�[�W�\���̂��쐬���ď�����
	//�t���[�����[�g�v��������
	timeBeginPeriod(1);	//�^�C�}�[�̕���\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();//���݂̃^�C�}�[�l
	dwCurrentTime = dwFrameCount = 0;

	//�I�����b�Z�[�W������܂Ń��[�v����
	//�Q�[�����[�v
	while (1)
	{	//���b�Z�[�W�̗L�����`�F�b�N
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{ //Windows���烁�b�Z�[�W�������I
			if (msg.message == WM_QUIT)//���S�I�����܂������b�Z�[�W
			{
				break;	//while���[�v����ʂ���
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);	//WndProc���Ăяo�����
			}
	
		}
		else //Windows���烁�b�Z�[�W�����Ă��Ȃ�
		{
			dwCurrentTime = timeGetTime();//���݂̃^�C�}�[�l���擾
			if ((dwCurrentTime - dwFPSLastTime) >= 1000)//1�b�o�߂�����
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;	//���݂̃^�C�}�[�l��ۑ�
				dwFrameCount = 0;				//�t���[���J�E���g���N���A
			}
			if ((dwCurrentTime - dwExecLastTime) >= ((float)1000 / 60)) // 1/60�b�o�߂�����
			{
				dwExecLastTime = dwCurrentTime;	//���݂̎��Ԃ�ۑ�
#ifdef _DEBUG
				wsprintf(g_DebugStr, WINDOW_CAPTION);//�z��ɃL���v�V������������i�[
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
				SetWindowText(hWnd, g_DebugStr);//�L���v�V���������̏�������
#endif

				Update();	//�X�V����
				Draw();		//�`�揈��
				keycopy();	//�L�[�����p

				dwFrameCount++;	//�t���[���J�E���g��i�߂�
			}

		}
	}//while

	//�I������
	Finalize();

	//�I������
	return (int)msg.wParam;

}

//=========================================
//�E�B���h�E�v���V�[�W��
// ���b�Z�[�W���[�v���ŌĂяo�����
//=========================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_ACTIVATEAPP:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			Keyboard_ProcessMessage(uMsg, wParam, lParam);
			break;
		case WM_KEYDOWN:	//�L�[�������ꂽ
			if (wParam == VK_ESCAPE)//�����ꂽ�̂�ESC�L�[
			{
				//�E�B���h�E����������N�G�X�g��Windows�ɑ���
				SendMessage(hWnd, WM_CLOSE, 0, 0);
			}
			Keyboard_ProcessMessage(uMsg, wParam, lParam);
			break;
		case WM_CLOSE:	//�E�B���h�E����Ȃ�������	
			ShowCursor(true); // �J�[�\����\���ɂ���
			if (
				MessageBox(hWnd, "�{���ɏI�����Ă�낵���ł����H",
					"�m�F", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK
				)
			{//OK�������ꂽ�Ƃ�
				DestroyWindow(hWnd);//�I������葱����Windows�փ��N�G�X�g
			}
			else
			{
				ShowCursor(false); // �J�[�\�����\���ɂ���
				return 0;	//����ς�I���Ȃ�
			}

			break;
		case WM_DESTROY:	//�I������OK�ł���
			PostQuitMessage(0);		//�����̃��b�Z�[�W�ɂO�𑗂�
			break;

	}

	//�K�p�̖������b�Z�[�W�͓K���ɏ��������ďI��
	return DefWindowProc(hWnd, uMsg, wParam, lParam);

}

//==================================
//������
//==================================
HRESULT	Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	//DirectX�֘A�̏�����
	InitRenderer(hInstance, hWnd, bWindow);
	//�L�[���͏�����
	Keyboard_Initialize();


	// �e�L�X�g�`��̏������iD3D�f�o�C�X�E�X���b�v�`�F�[�������������ꂽ��I�j
	InitTextRenderer(GetDevice(), GetDeviceContext(), GetSwapChain()); // �� �K�v�ɉ����Ċ֐��p��
	//�}�l�[�W��������
	InitManager();



	return	S_OK;
}

//====================================
//	�I������
//====================================
void	Finalize(void)
{
	//�e�L�X�g�`��I��
	ReleaseTextRender();

	//�}�l�[�W���I��
	FinalizeManager();
	//DirectX�֘A�̏I������
	FinalizeRenderer();
}

//===================================
//�X�V����
//====================================
void	Update(void)
{
	//�}�l�[�W���X�V
	UpdateManager();

}

//==================================
//�`�揈��
//==================================
void	Draw(void)
{
	//�o�b�N�o�b�t�@�̃N���A
	Clear();
	//�}�l�[�W���`��
	DrawManager();
	//�o�b�N�o�b�t�@���t�����g�o�b�t�@�փR�s�[
	Present();



}


