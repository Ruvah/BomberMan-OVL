#include "../States.fx"

float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gLightVP;
float3 gLightDirection;

Texture2D gDiffuseMap;
Texture2D gShadowMap;

struct VS_INPUT
{
    float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD1;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
    float4 lPos : TEXCOORD0;
    float2 texCoord : TEXCOORD1;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
    output.pos = mul(float4(input.pos, 1.0f), gWorldViewProj);
	output.normal = normalize(mul(input.normal, (float3x3)gWorld));
    output.lPos = mul(float4(input.pos, 1.0f), mul(gWorld, gLightVP));
    output.texCoord = input.texCoord;
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
        input.lPos.z < 0.0f  || input.lPos.z > 1.0f)
        return float4(diffuseColor.rgb, 1.0f);

    input.lPos.x = input.lPos.x / 2 + 0.5f;
    input.lPos.y = input.lPos.y / -2 + 0.5f;

    float shadowMapDepth = gShadowMap.Sample(gSS_PointWrap, input.lPos.xy).r;

    float Epsilon = 1e-4;
    if (shadowMapDepth + Epsilon < input.lPos.z)
        diffuseColor *= 0.5f;

    return float4(diffuseColor.rgb, 1.0f);
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
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}

