#pragma once
/*==============================================================================

   GPUパーティクルシステム [gpuParticle.h]
                                                         Author :
                                                         Date   :
--------------------------------------------------------------------------------
   Compute Shader でパーティクル更新、
   Geometry Shader でビルボード展開、
   Pixel Shader でフェードアウト描画
==============================================================================*/

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// 定数
//*****************************************************************************
#define PARTICLE_MAX_DEFAULT 4096

//*****************************************************************************
// パーティクルクラス
//*****************************************************************************
class GPUParticle
{
private:
    // パーティクルデータ構造体（GPU側と一致させる）
    struct Particle
    {
        XMFLOAT3 Position;
        float    Life;
        XMFLOAT3 Velocity;
        float    MaxLife;
    };

    // 定数バッファ構造体（CS用）
    struct ParticleParams
    {
        XMFLOAT3 EmitterPosition;
        float    DeltaTime;
        XMFLOAT4 Seed;
    };

    // シェーダー
    ID3D11ComputeShader*   m_ComputeShader;
    ID3D11VertexShader*    m_VertexShader;
    ID3D11GeometryShader*  m_GeometryShader;
    ID3D11PixelShader*     m_PixelShader;

    // パーティクルバッファ
    ID3D11Buffer*              m_ParticleBuffer;
    ID3D11UnorderedAccessView* m_ParticleUAV;
    ID3D11ShaderResourceView*  m_ParticleSRV;

    // 定数バッファ
    ID3D11Buffer* m_ParamsCB;

    // 描画ステート
    ID3D11BlendState*       m_BlendState;
    ID3D11DepthStencilState* m_DepthState;
    ID3D11RasterizerState*  m_RasterizerState;

    int m_MaxParticles;

public:
    void Init(int maxParticles = PARTICLE_MAX_DEFAULT);
    void Finalize();
    void Update(float deltaTime);
    void Draw();
};
