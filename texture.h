/*==============================================================================

   テクスチャ管理 [texture.h]
	
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <d3d11.h>


void TextureInitialize(ID3D11Device* device);
int TextureLoad(const std::wstring& texture_filename);
ID3D11ShaderResourceView* GetTexture(int id);
int TextureGetWidth(int id);
int TextureGetHeight(int id);
void TextureFinalize(void);

#endif // TEXTURE_H

#define SAFE_RELEASE(o)      if (o){  (o)->Release(); o = NULL; }