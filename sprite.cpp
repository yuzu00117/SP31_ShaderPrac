/*==============================================================================

   簡易スプライト表示
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include	"main.h"
#include	"sprite.h"
#include	"texture.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NUM_SPRITEVERTEX 4

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点バッファ

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSprite(void)
{
	ID3D11Device *pDevice = GetDevice();

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;//動的にアクセス
	bd.ByteWidth = sizeof(VERTEX_3D) * NUM_SPRITEVERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPUで書き込み可能
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void FinalizeSprite(void)
{
	// 頂点バッファの解放
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 描画処理（単色）
//=============================================================================
void DrawSprite(XMFLOAT2 size, XMFLOAT4 color)
{

	//頂点バッファの書き込み設定ポインターを取得
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float size_W = size.x * 0.5f;
	float size_H = size.y * 0.5f;

	//左上 中心からサイズの半分の場所で座標設定
	vertex[0].Position = XMFLOAT3(-size_W, -size_H, 0.0f);
	vertex[0].Diffuse = color;
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	//右上 中心からサイズの半分の場所で座標設定
	vertex[1].Position = XMFLOAT3(size_W, -size_H, 0.0f);
	vertex[1].Diffuse = color;
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	//左下 中心からサイズの半分の場所で座標設定
	vertex[2].Position = XMFLOAT3(-size_W, size_H, 0.0f);
	vertex[2].Diffuse = color;
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	//右下 中心からサイズの半分の場所で座標設定
	vertex[3].Position = XMFLOAT3(size_W, size_H, 0.0f);
	vertex[3].Diffuse = color;
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);
	//書き込み解除
	GetDeviceContext()->Unmap(g_VertexBuffer, 0);


	// 頂点バッファをDirectXへセット
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);
	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// マテリアルをシェーダーへ送信
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);
	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
}

//=============================================================================
// 描画処理（テクスチャ付き）
//=============================================================================
void DrawSpriteWithTexture(XMFLOAT2 position, XMFLOAT2 size, int textureID, XMFLOAT4 color)
{
	//頂点バッファの書き込み設定ポインターを取得
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float size_W = size.x * 0.5f;
	float size_H = size.y * 0.5f;

	// 指定された位置を中心としてスプライトを配置
	//左上
	vertex[0].Position = XMFLOAT3(position.x - size_W, position.y - size_H, 0.0f);
	vertex[0].Diffuse = color;
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	//右上
	vertex[1].Position = XMFLOAT3(position.x + size_W, position.y - size_H, 0.0f);
	vertex[1].Diffuse = color;
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	//左下
	vertex[2].Position = XMFLOAT3(position.x - size_W, position.y + size_H, 0.0f);
	vertex[2].Diffuse = color;
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	//右下
	vertex[3].Position = XMFLOAT3(position.x + size_W, position.y + size_H, 0.0f);
	vertex[3].Diffuse = color;
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);
	//書き込み解除
	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// テクスチャをセット
	ID3D11ShaderResourceView* tex = GetTexture(textureID);
	GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

	// 頂点バッファをDirectXへセット
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);
	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// マテリアルをシェーダーへ送信
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);
	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
}

