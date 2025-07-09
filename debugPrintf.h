
//==============================================================================
//DebugPrintfを作って使おう [debugPrintf.h]
//=============================================================================

#pragma once
#include <windows.h>
#include <stdio.h>
// デバック用printf...VisualStudioの出力ウィンドウに出力される
inline void DebugPrintf(const char* pFormat, ...)
{
// こんなふうに書くとReleaseビルド時はビルドされない範囲を設定できる
#if defined(_DEBUG) || defined(DEBUG)
	va_list argp;
	char buf[256]; // バッファは大きめに取りましょう
	va_start(argp, pFormat);
	vsprintf_s(buf, 256, pFormat, argp); // バッファ量と第２引数を合わせること
	va_end(argp);
	OutputDebugStringA(buf);
#endif // _DEBUG || DEBUG
}
