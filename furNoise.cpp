/*==============================================================================

   ファーノイズテクスチャ生成 [furNoise.cpp]

==============================================================================*/
#include "furNoise.h"
#include <cstdlib>
#include <ctime>

// ランタイムでファー用ノイズテクスチャを生成
ID3D11ShaderResourceView* CreateFurNoiseTexture(
    ID3D11Device* device,
    int width, int height,
    float density)
{
    // ピクセルデータを生成（R8 フォーマット）
    int totalPixels = width * height;
    unsigned char* pixels = new unsigned char[totalPixels];

    srand((unsigned int)time(NULL));

    for (int i = 0; i < totalPixels; i++)
    {
        // density の確率で白（255）、それ以外は黒（0）
        float r = (float)rand() / (float)RAND_MAX;
        pixels[i] = (r < density) ? 255 : 0;
    }

    // テクスチャ作成
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = pixels;
    initData.SysMemPitch = width; // R8 = 1byte per pixel

    ID3D11Texture2D* texture = nullptr;
    HRESULT hr = device->CreateTexture2D(&texDesc, &initData, &texture);

    delete[] pixels;

    if (FAILED(hr))
        return nullptr;

    // シェーダーリソースビュー作成
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    ID3D11ShaderResourceView* srv = nullptr;
    hr = device->CreateShaderResourceView(texture, &srvDesc, &srv);

    texture->Release(); // SRV が参照を持つのでテクスチャは解放可能

    if (FAILED(hr))
        return nullptr;

    return srv;
}
