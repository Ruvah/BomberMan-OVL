#include "../States.fx"

SamplerComparisonState cmpSampler
{
   // sampler state
    Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
    AddressU = MIRROR;
    AddressV = MIRROR;
 
   // sampler comparison state
    ComparisonFunc = LESS_EQUAL;
};

float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 

float4x4 gBones[70];

float4x4 gLightVP;
float3 gLightDirection;

Texture2D gDiffuseMap;
Texture2D gShadowMap;

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
    float4 blendIndices : BLENDINDICES;
    float4 blendWeights : BLENDWEIGHTS;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
    float4 lPos : TEXCOORD1;
};



//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{

	VS_OUTPUT output;

	float4 originalPosition = float4(input.pos, 1);
	float4 transformedPosition = 0;
	float3 transformedNormal = 0;

	//Skinning Magic...
    for (int i = 0; i < 4; ++i)
    {
        int boneIndex = input.blendIndices[i];
        if (boneIndex > -1)
        {
            transformedPosition += mul(originalPosition, gBones[boneIndex]) * input.blendWeights[i];
            transformedPosition.w = 1;

            transformedNormal += input.normal * input.blendWeights[i];
        }
    }
	
	//Don't forget to change the output.pos & output.normal variables...
	output.pos = mul (transformedPosition, gWorldViewProj ); //Non skinned position
	output.normal = normalize(mul(transformedNormal, (float3x3)gWorld)); //Non skinned normal
	output.texCoord = input.texCoord;
    output.lPos = mul(float4(input.pos, 1.0f), mul(gWorld, gLightVP));

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    input.lPos.xyz /= input.lPos.w;

    float4 diffuseColor = gDiffuseMap.Sample(gSS_LinearWrap, input.texCoord);

    float diffuseStrength = dot(input.normal, -gLightDirection);
    diffuseStrength = diffuseStrength * 0.5 + 0.5;
    diffuseStrength = saturate(diffuseStrength);
    diffuseColor.rgb *= diffuseStrength;

    if (input.lPos.x < -1.0f || input.lPos.x > 1.0f ||
        input.lPos.y < -1.0f || input.lPos.y > 1.0f ||
        input.lPos.z < 0.0f || input.lPos.z > 1.0f)
        return float4(diffuseColor.rgb, 1.0f);

    input.lPos.x = input.lPos.x / 2 + 0.5f;
    input.lPos.y = input.lPos.y / -2 + 0.5f;

    float shadowMapDepth = gShadowMap.Sample(gSS_PointWrap, input.lPos.xy).r;

    float Epsilon = 1e-4;
    if (shadowMapDepth + Epsilon < input.lPos.z)
        diffuseColor *= 0.5f;

    return diffuseColor;
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
        SetRasterizerState(gRS_NoCulling);
        SetDepthStencilState(gDSS_EnableDepthAndWrite, 0);
        SetBlendState(gBS_NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

