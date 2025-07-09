#include"common.hlsl"

void main(in VS_IN In,out PS_IN Out)
{
    //�����Œ��_�ϊ�
    //���_���W���o��
    //���_�ϊ������@���̏����͕K���K�v
    matrix wvp;                 //�s��ϐ����쐬
    wvp = mul(World, View);     //wvp=���[���h�s�񁖃J�����s��
    wvp = mul(wvp, Projection); //wvp��wvp���v���W�F�N�V�����s��
    Out.Position = mul(In.Position, wvp);   //�ϊ����ʂ��o�͂���
    
    
    //���_�@�������[���h�s��ŉ�]������i���_�Ɠ�����]��������j
    float4 worldNormal, normal;//���[�J���ϐ����쐬
    
    normal = float4(In.Normal.xyz,0.0);//���͖@���x�N�g���̂����O�Ƃ��ăR�s�[�i���s�ړ����Ȃ����߁j
    worldNormal = mul(normal,World);//�R�s�[���ꂽ�@�������[���h�s��ŉ�]����
    worldNormal = normalize(worldNormal);//��]��̖@���𐳋K������
    Out.Normal = worldNormal;//��]��̖@���o�́@In.Normal�ł͂Ȃ���]��̖@�����o��
    
    
    //���邳�̌v�Z
    //���̃x�N�g���Ɩ@���̓��ρ@XYZ�v�f�݂̂Ōv�Z
    float light = 0.5-0.5*dot(Light.Direction.xyz,worldNormal.xyz);
    light = saturate(light);//���邳���O�ƂP�̊ԂŖO�a������
    
    //���_�̃f�t���[�Y�o��
    //�e�N�X�`�����W���o��
    Out.Diffuse.rgb = light * In.Diffuse.rgb;//���邳�����_�F���o��
    Out.Diffuse.a = In.Diffuse.a;//a�͒��_�̕������̂܂܏o��
    
    //�󂯎�������̒��_��UV���W�����̂܂܏o��
    Out.TexCoord = In.TexCoord;
    
    Out.Diffuse = light * In.Diffuse;//���邳*���_�F�Ƃ��ďo��

}