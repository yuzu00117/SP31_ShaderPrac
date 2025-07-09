
#include	"main.h"
#include	"renderer.h"
#include	"Manager.h"
#include	"keyboard.h"
//デバック用文字のクラス
#include"DrawTextDebug.h"

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

//===================================
// ライブラリのリンク
//===================================




#pragma	comment (lib, "d3d11.lib")
#pragma	comment (lib, "d3dcompiler.lib")
#pragma	comment (lib, "winmm.lib")
#pragma	comment (lib, "dxguid.lib")
#pragma	comment (lib, "dinput8.lib")

//=================================
//マクロ定義
//=================================
#define		CLASS_NAME		"GameProject Window"
#define		WINDOW_CAPTION	"GameProject   <<シェーダーの中の人>>"

//===================================
//プロトタイプ宣言
//===================================
//コールバック関数＝＞他人が呼び出してくれる関数
LRESULT	CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//初期化関数
HRESULT	Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
//終了処理
void	Finalize(void);
//更新処理
void	Update(void);
//描画処理
void	Draw(void);

//==================================
//グローバル変数
//==================================
#ifdef _DEBUG	//デバッグモード時のみ変数を作る
int	g_CountFPS;							//FPSカウンター
char g_DebugStr[2048] = WINDOW_CAPTION;	//表示文字列設定
#endif



#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
//=====================================
//メイン関数
//======================================
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance, LPSTR lpCmd, int nCmdShow)
{
	// メモリリーク検出を有効にする
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//乱数初期化
	srand((UINT)timeGetTime());
	//フレームレート計測用変数
	DWORD	dwExecLastTime;
	DWORD	dwFPSLastTime;
	DWORD	dwCurrentTime;
	DWORD	dwFrameCount;
	//COMコンポーネントの準備（機能を部品化して外部のプログラムから共有利用する仕組み）
	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	//ウィンドウクラスの登録（ウィンドウの仕様的な物を決めてWindowsへセットする）
	WNDCLASS	wc;	//構造体を準備
	ZeroMemory(&wc, sizeof(WNDCLASS));//内容を０で初期化
	wc.lpfnWndProc = WndProc;	//コールバック関数のポインター
	wc.lpszClassName = CLASS_NAME;	//この仕様書の名前
	wc.hInstance = hInstance;	//このアプリケーションのこと
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);//カーソルの種類
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);//ウィンドウの背景色
	RegisterClass(&wc);	//構造体をWindowsへセット


	//ウィンドウサイズの調整
	//             左上　　右下
	RECT	rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };//横1280 縦720
	//描画領域が1280X720になるようにサイズを調整する
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME|WS_MAXIMIZEBOX|WS_MINIMIZEBOX), FALSE);

	//ウィンドウの作成
	HWND	hWnd = CreateWindow(
		CLASS_NAME,	//作りたいウィンドウ
		WINDOW_CAPTION,	//ウィンドウに表示するタイトル
		WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX),//標準的な形状のウィンドウ サイズ変更禁止
		CW_USEDEFAULT,		//デフォルト設定でおまかせ
		CW_USEDEFAULT,
		rc.right - rc.left,//CW_USEDEFAULT,//ウィンドウの幅
		rc.bottom - rc.top,//CW_USEDEFAULT,//ウィンドウの高さ
		NULL,
		NULL,
		hInstance,		//アプリケーションのハンドル
		NULL
	);

	//初期化処理
	if (FAILED(Init(hInstance, hWnd, true)))
	{
		return -1;//初期化失敗
	}

	//作成したウィンドウを表示する
	ShowWindow(hWnd, nCmdShow);//引数に従って表示、または非表示
	//ウィンドウの内容を強制表示
	UpdateWindow(hWnd);

	//メッセージループ
	MSG	msg;
	ZeroMemory(&msg, sizeof(MSG));//メッセージ構造体を作成して初期化
	//フレームレート計測初期化
	timeBeginPeriod(1);	//タイマーの分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();//現在のタイマー値
	dwCurrentTime = dwFrameCount = 0;

	//終了メッセージが来るまでループする
	//ゲームループ
	while (1)
	{	//メッセージの有無をチェック
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{ //Windowsからメッセージが来た！
			if (msg.message == WM_QUIT)//完全終了しましたメッセージ
			{
				break;	//whileループからぬける
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);	//WndProcが呼び出される
			}
	
		}
		else //Windowsからメッセージが来ていない
		{
			dwCurrentTime = timeGetTime();//現在のタイマー値を取得
			if ((dwCurrentTime - dwFPSLastTime) >= 1000)//1秒経過したか
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;	//現在のタイマー値を保存
				dwFrameCount = 0;				//フレームカウントをクリア
			}
			if ((dwCurrentTime - dwExecLastTime) >= ((float)1000 / 60)) // 1/60秒経過したか
			{
				dwExecLastTime = dwCurrentTime;	//現在の時間を保存
#ifdef _DEBUG
				wsprintf(g_DebugStr, WINDOW_CAPTION);//配列にキャプション文字列を格納
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
				SetWindowText(hWnd, g_DebugStr);//キャプション部分の書き換え
#endif

				Update();	//更新処理
				Draw();		//描画処理
				keycopy();	//キー処理用

				dwFrameCount++;	//フレームカウントを進める
			}

		}
	}//while

	//終了処理
	Finalize();

	//終了する
	return (int)msg.wParam;

}

//=========================================
//ウィンドウプロシージャ
// メッセージループ内で呼び出される
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
		case WM_KEYDOWN:	//キーが押された
			if (wParam == VK_ESCAPE)//押されたのはESCキー
			{
				//ウィンドウを閉じたいリクエストをWindowsに送る
				SendMessage(hWnd, WM_CLOSE, 0, 0);
			}
			Keyboard_ProcessMessage(uMsg, wParam, lParam);
			break;
		case WM_CLOSE:	//ウィンドウを閉じなさい命令	
			ShowCursor(true); // カーソルを表示にする
			if (
				MessageBox(hWnd, "本当に終了してよろしいですか？",
					"確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK
				)
			{//OKが押されたとき
				DestroyWindow(hWnd);//終了する手続きをWindowsへリクエスト
			}
			else
			{
				ShowCursor(false); // カーソルを非表示にする
				return 0;	//やっぱり終わらない
			}

			break;
		case WM_DESTROY:	//終了してOKですよ
			PostQuitMessage(0);		//自分のメッセージに０を送る
			break;

	}

	//必用の無いメッセージは適当に処理させて終了
	return DefWindowProc(hWnd, uMsg, wParam, lParam);

}

//==================================
//初期化
//==================================
HRESULT	Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	//DirectX関連の初期化
	InitRenderer(hInstance, hWnd, bWindow);
	//キー入力初期化
	Keyboard_Initialize();


	// テキスト描画の初期化（D3Dデバイス・スワップチェーンが初期化された後！）
	InitTextRenderer(GetDevice(), GetDeviceContext(), GetSwapChain()); // ← 必要に応じて関数用意
	//マネージャ初期化
	InitManager();



	return	S_OK;
}

//====================================
//	終了処理
//====================================
void	Finalize(void)
{
	//テキスト描画終了
	ReleaseTextRender();

	//マネージャ終了
	FinalizeManager();
	//DirectX関連の終了処理
	FinalizeRenderer();
}

//===================================
//更新処理
//====================================
void	Update(void)
{
	//マネージャ更新
	UpdateManager();

}

//==================================
//描画処理
//==================================
void	Draw(void)
{
	//バックバッファのクリア
	Clear();
	//マネージャ描画
	DrawManager();
	//バックバッファをフロントバッファへコピー
	Present();



}


