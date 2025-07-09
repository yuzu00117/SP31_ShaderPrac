#include"common.hlsl"

void main(in VS_IN In,out PS_IN Out)
{
    //ここで頂点変換
    //頂点座標を出力
    //頂点変換処理　この処理は必ず必要
    matrix wvp;                 //行列変数を作成
    wvp = mul(World, View);     //wvp=ワールド行列＊カメラ行列
    wvp = mul(wvp, Projection); //wvp＝wvp＊プロジェクション行列
    Out.Position = mul(In.Position, wvp);   //変換結果を出力する
    
    
    //頂点法線をワールド行列で回転させる（頂点と同じ回転をさせる）
    float4 worldNormal, normal;//ローカル変数を作成
    
    normal = float4(In.Normal.xyz,0.0);//入力法線ベクトルのｗを０としてコピー（平行移動しないため）
    worldNormal = mul(normal,World);//コピーされた法線をワールド行列で回転する
    worldNormal = normalize(worldNormal);//回転後の法線を正規化する
    Out.Normal = worldNormal;//回転後の法線出力　In.Normalではなく回転後の法線を出力
    
    
    //明るさの計算
    //光のベクトルと法線の内積　XYZ要素のみで計算
    float light = 0.5-0.5*dot(Light.Direction.xyz,worldNormal.xyz);
    light = saturate(light);//明るさを０と１の間で飽和化する
    
    //頂点のデフューズ出力
    //テクスチャ座標を出力
    Out.Diffuse.rgb = light * In.Diffuse.rgb;//明るさ＊頂点色を出力
    Out.Diffuse.a = In.Diffuse.a;//aは頂点の物をそのまま出力
    
    //受け取ったこの頂点のUV座標をそのまま出力
    Out.TexCoord = In.TexCoord;
    
    Out.Diffuse = light * In.Diffuse;//明るさ*頂点色として出力

}