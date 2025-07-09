#include "texture.h"
#include "DirectXTex.h"
using namespace DirectX;
//#include "directx.h"
#include <string>
#include "renderer.h"

#define TEXTURE_MAX (256)


struct TextureData
{
	std::wstring filename;
	ID3D11ShaderResourceView* shader_resource_view;
	int width;
	int height;
};

static TextureData g_Textures[TEXTURE_MAX] = {};

static int g_TextureCount = 0;

// ※外部からもらうデバイスのポインタなのでReleaseはしちゃダメ
static ID3D11Device* g_Device = NULL;


void TextureInitialize(ID3D11Device* device)
{
	for (int i = 0; i < TEXTURE_MAX; i++) {
		g_Textures[i].shader_resource_view = NULL;
	}

	g_TextureCount = 0;

	g_Device = device;
}

int TextureLoad(const std::wstring& texture_filename)
{
	// すでに同名のテクスチャが読み込まれていないか確認する
	for (int i = 0; i < g_TextureCount; i++) {
		if (g_Textures[i].filename == texture_filename) {
			return i; // すでに読み込まれていたのでidをわたす
		}
	}

	// テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(texture_filename.c_str(), WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(g_Device, image.GetImages(), image.GetImageCount(), metadata, &g_Textures[g_TextureCount].shader_resource_view);
	g_Textures[g_TextureCount].width = (int)metadata.width;
	g_Textures[g_TextureCount].height = (int)metadata.height;

	if (!g_Textures[g_TextureCount].shader_resource_view) {
		MessageBoxW(NULL, L"ファイルが読み込めなかった", texture_filename.c_str(), MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}

	return g_TextureCount++;
}

ID3D11ShaderResourceView* GetTexture(int id)
{
	if (id < 0 || id >= g_TextureCount) {
		return NULL;
	}

	return g_Textures[id].shader_resource_view;
}

int TextureGetWidth(int id)
{
	return g_Textures[id].width;
}

int TextureGetHeight(int id)
{
	return g_Textures[id].height;
}

void TextureFinalize(void)
{
	for (int i = 0; i < g_TextureCount; i++) {
		// g_Textures[i].shader_resource_view->Release();
		// g_Textures[i] = NULL;
		SAFE_RELEASE(g_Textures[i].shader_resource_view);
	}
}
