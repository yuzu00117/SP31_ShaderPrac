#include"common.hlsl"

// 頂点シェーダーの入力構造体
void main(in VS_IN In, out PS_IN Out)
{
    //ここで頂点変換
    //頂点座標を出力
    //頂点変換処理　この処理は必ず必要
    matrix wvp; //行列変数を作成
    wvp = mul(World, View); //wvp=ワールド行列＊カメラ行列
    wvp = mul(wvp, Projection); //wvp＝wvp＊プロジェクション行列
    Out.Position = mul(In.Position, wvp); //変換結果を出力する
    
    
    //頂点法線をワールド行列で回転させる（頂点と同じ回転をさせる）
    float4 worldNormal, normal; //ローカル変数を作成
    
    normal = float4(In.Normal.xyz, 0.0); //入力法線ベクトルのｗを０としてコピー（平行移動しないため）
    worldNormal = mul(normal, World); //コピーされた法線をワールド行列で回転する
    worldNormal = normalize(worldNormal); //回転後の法線を正規化する
    Out.Normal =
    worldNormal; //回転後の法線出力　In.Normalではなく回転後の法線を出力

    Out.Diffuse = In.Diffuse; //頂点の物をそのまま出力
    Out.TexCoord = In.TexCoord; //頂点の物をそのまま出力
    
    //ワールド変換した頂点座標を出力（この行は最初追加しない）
    Out.WorldPosition = mul(In.Position, World);


}