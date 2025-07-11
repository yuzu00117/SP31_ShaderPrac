#include "skyDome.h"

#include "texture.h"

#include "model.h"



static LIGHT Light;

static ID3D11VertexShader* g_VertexShader = NULL;

static ID3D11InputLayout* g_VertexLayout = NULL;

static ID3D11PixelShader* g_PixelShader = NULL;



HRESULT SkyDome::InitPolygonModel(void)

{

    CreateVertexShader(&g_VertexShader, &g_VertexLayout, "unlitTextureVS.cso");

    CreatePixelShader(&g_PixelShader, "unlitTexturePS.cso");



    Light.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    Light.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    Light.Direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);



    Position = XMFLOAT3(0.0f, 0.0f, 0.0f);

    Rotate   = XMFLOAT3(0.0f, 0.0f, 0.0f);

    Scale    = XMFLOAT3(-50.0f, 50.0f, 50.0f);



    TexID = TextureLoad(L"asset\\texture\\sura.jpg");

    Model = ModelLoad("asset\\model\\ball.fbx");



    return S_OK;

}



void SkyDome::FinalizePolygonModel(void)

{

    ModelRelease(Model);

}



void SkyDome::UpdatePolygonModel(void)

{

}



void SkyDome::DrawPolygonModel(void)

{

    SetLight(Light);



    GetDeviceContext()->IASetInputLayout(g_VertexLayout);

    GetDeviceContext()->VSSetShader(g_VertexShader, NULL, 0);

    GetDeviceContext()->PSSetShader(g_PixelShader, NULL, 0);



    ID3D11ShaderResourceView* tex = GetTexture(TexID);

    GetDeviceContext()->PSSetShaderResources(0, 1, &tex);



    XMMATRIX TranslationMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);

    XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotate.x), XMConvertToRadians(Rotate.y), XMConvertToRadians(Rotate.z));

    XMMATRIX ScalingMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);

    XMMATRIX WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;

    SetWorldMatrix(WorldMatrix);



    GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    MATERIAL material;

    ZeroMemory(&material, sizeof(MATERIAL));

    material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    SetMaterial(material);



    ModelDraw(Model);

}



