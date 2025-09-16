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
// 描画処理（テクスチャ付き）- polygon2Dと同じアプローチ
//=============================================================================
void DrawSpriteWithTexture(XMFLOAT2 position, XMFLOAT2 size, int textureID, XMFLOAT4 color)
{
	// 2D用シェーダーを設定
	ID3D11VertexShader* vs = NULL;
	ID3D11InputLayout* layout = NULL;
	ID3D11PixelShader* ps = NULL;
	
	// polygon2Dと同じシェーダーを使用
	CreateVertexShader(&vs, &layout, "UnlitTextureVS.cso");
	CreatePixelShader(&ps, "UnlitTexturePS.cso");
	
	// シェーダーを設定
	GetDeviceContext()->IASetInputLayout(layout);
	GetDeviceContext()->VSSetShader(vs, NULL, 0);
	GetDeviceContext()->PSSetShader(ps, NULL, 0);

	// 深度処理を無効にする
	SetDepthEnable(false);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// テクスチャをセット
	ID3D11ShaderResourceView* tex = GetTexture(textureID);
	if (tex != NULL) {
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);
	}

	// ワールド行列の作成（polygon2Dと同じ方式）
	XMMATRIX TranslationMatrix = XMMatrixTranslation(position.x, position.y, 0.0f);
	XMMATRIX RotationMatrix = XMMatrixRotationZ(0.0f); // 回転なし
	XMMATRIX ScalingMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f); // スケールなし
	XMMATRIX WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;
	SetWorldMatrix(WorldMatrix);

	// スプライト描画（既存のDrawSprite関数を利用）
	DrawSprite(size, color);
	
	// シェーダーリソースを解放
	if (vs) vs->Release();
	if (layout) layout->Release();
	if (ps) ps->Release();
}

