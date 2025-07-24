#include"common.hlsl"

// 頂点シェーダーの入力構造体
void main(in VS_IN In, out PS_IN Out)
{
    //行列で頂点変換
    //頂点座標を出力
    //頂点変換は常習法の処理は必ず必要
    matrix wvp; //行列変数を作成
    wvp = mul(World, View); //wvp=ワールド行列×カメラ行列
    wvp = mul(wvp, Projection); //wvp＝wvp×プロジェクション行列
    Out.Position = mul(In.Position, wvp); //変換結果を出力する
    
    //頂点法線をワールド行列で回転させる（頂点と同じように回転させる）
    float4 worldNormal, normal; //ローカル変数を作成
    
    normal = float4(In.Normal.xyz, 0.0); //入力法線ベクトルのwを0としてコピー（平行移動しないため）
    worldNormal = mul(normal, World); //コピーした法線をワールド行列で回転する
    worldNormal = normalize(worldNormal); //回転後の法線を正規化する
    Out.Normal = worldNormal; //回転後の法線を出力	In.Normalではなく回転後の法線を出力

    Out.Diffuse = In.Diffuse; //頂点の物をそのまま出力
    Out.TexCoord = In.TexCoord; //頂点の物をそのまま出力
    
    //ワールド変換された頂点座標を出力（この処理は最重要追加した）
    Out.WorldPosition = mul(In.Position, World);
}