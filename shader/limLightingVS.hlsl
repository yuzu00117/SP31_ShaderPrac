#include"common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    //�����Œ��_�ϊ�
    //���_���W���o��
    //���_�ϊ������@���̏����͕K���K�v
    matrix wvp; //�s��ϐ����쐬
    wvp = mul(World, View); //wvp=���[���h�s�񁖃J�����s��
    wvp = mul(wvp, Projection); //wvp��wvp���v���W�F�N�V�����s��
    Out.Position = mul(In.Position, wvp); //�ϊ����ʂ��o�͂���
    
    
    //���_�@�������[���h�s��ŉ�]������i���_�Ɠ�����]��������j
    float4 worldNormal, normal; //���[�J���ϐ����쐬
    
    normal = float4(In.Normal.xyz, 0.0); //���͖@���x�N�g���̂����O�Ƃ��ăR�s�[�i���s�ړ����Ȃ����߁j
    worldNormal = mul(normal, World); //�R�s�[���ꂽ�@�������[���h�s��ŉ�]����
    worldNormal = normalize(worldNormal); //��]��̖@���𐳋K������
    Out.Normal =
    worldNormal; //��]��̖@���o�́@In.Normal�ł͂Ȃ���]��̖@�����o��

    Out.Diffuse = In.Diffuse; //���_�̕������̂܂܏o��
    Out.TexCoord = In.TexCoord; //���_�̕������̂܂܏o��
    
    //���[���h�ϊ��������_���W���o�́i���̍s�͍ŏ��ǉ����Ȃ��j
    Out.WorldPosition = mul(In.Position, World);


}