//DX10 - FLAT SHADER
//Digital Arts & Entertainment


//GLOBAL VARIABLES
//****************
float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
float4x4 gWorld : WORLD;
float4x4 gView : VIEW;
float4x4 gViewInverse : VIEWINVERSE;
float4x4 gProj: PROJECTION;
float4 gColorDiffuse : COLOR = float4(1.0, 1.0, 1.0, 1.0);
float3 gLightDir = { 0.2f,-1.f,0.2f };
float3 gGravity = float3(0, -1, 0);
float gGravityForce = 0.2;
float gFurLength = 2;
float gLayers = 60;
float gUVScale = 1.f;
float3 gForceDirection = float3(0, 0, 0);
float gForceStrength;
float gFinToShellRatio = 1.0;
float gHairThickness = 1;
float gLengthMinimumPercentage = 0.8;

Texture2D gFurTexture;
Texture2D gShellTexture;
Texture2D gFinTexture;
//STATES
//******
RasterizerState NoCulling { CullMode = NONE; };
SamplerState samFur
{
	Texture = (gFurTexture);
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;// or Mirror or Clamp or Border
	AddressV = Wrap;// or Mirror or Clamp or Border
};

RasterizerState FrontCull
{
	CullMode = Front;
};
BlendState AlphaBlend
{
	BlendEnable[0] = true;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};
BlendState NoBlend
{
	BlendEnable[0] = false;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};
DepthStencilState NoDepthWrite
{
	DepthEnable = false;
	DepthWriteMask = ZERO;

};
DepthStencilState DepthWrite
{
	DepthEnable = true;
	DepthWriteMask = ALL;
};

struct VS_DATA
{
	float3 pos : POSITION;
	float4 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};
struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal: TEXCOORD1;
};

struct GS_OUTPUT {
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal: TEXCOORD1;
	float2 layer: LAYER;
};

struct GS_FINOUTPUT {
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal: TEXCOORD1;
	float2 texCoord2 : TEXCOORD2;
};
//Normal
//******************
VS_OUTPUT MainVS(VS_DATA input) {

	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos = mul(float4(input.pos, 1.f), gWorldViewProj);
	output.normal = normalize(mul(input.normal, (float3x3)gWorld));
	output.texCoord = input.texCoord;
	return output;
}

float4 MainPS(VS_OUTPUT input) :SV_TARGET{
	float4 color = gFurTexture.Sample(samFur,input.texCoord);
	float4 finalCol = color;

	//halflambert diffuse
	float strength = dot(input.normal, -gLightDir);
	strength = strength * 0.5 + 0.5;
	strength = saturate(strength);

	finalCol.rgb = color.rgb*strength;

	return finalCol;

}


//SHELL
//******************
VS_DATA ShellVS(VS_DATA input) {

	return input;

}

float nrand(float2 uv)
{
	return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

void CreateShellVertex(inout TriangleStream<GS_OUTPUT> triStream, float3 pos, float3 normal, float2 texCoord, float layer)
{
	GS_OUTPUT output = (GS_OUTPUT)0;
	float randHairFactor = (nrand(texCoord) % (1 - gLengthMinimumPercentage)) + gLengthMinimumPercentage;
	//extrusion around the object
	//pos *= 1+(gFurLength*layer);
	output.pos = float4(pos + normal * ((gFurLength*randHairFactor)* layer), 1);
	//making normal face the correct direction for lighting
	output.normal = normalize(mul(normal, (float3x3)gWorld));
	//bit of gravity on the end of the hair
	float3 gravity = mul((gGravity*gGravityForce + (gForceDirection*gForceStrength)), gWorld);
	float bend = pow(layer, 3);//stay between 0 and 1 but increase exponentionally
	output.pos = output.pos + float4(gravity*bend, 0);

	output.pos = mul(output.pos, gWorldViewProj);
	//output.normal = normal;
	output.texCoord = texCoord * gUVScale;//stretch or shrink the fur over the object
	output.layer = layer;
	triStream.Append(output);
}

[maxvertexcount(90)]
void ShellGS(triangle VS_DATA ver[3], inout TriangleStream<GS_OUTPUT> triStream)
{
	float layer = 0;

	[loop] for (int i = 1; i < gLayers; ++i)
	{
		layer = i / gLayers;
		CreateShellVertex(triStream, ver[0].pos, ver[0].normal, ver[0].texCoord, layer);
		CreateShellVertex(triStream, ver[1].pos, ver[1].normal, ver[1].texCoord, layer);
		CreateShellVertex(triStream, ver[2].pos, ver[2].normal, ver[2].texCoord, layer);
		triStream.RestartStrip();

	}
}



float4 ShellPS(GS_OUTPUT input) :SV_TARGET{

float randHairFactor = (nrand(input.texCoord) % (1 - gLengthMinimumPercentage)) + gLengthMinimumPercentage;
float4 color = gFurTexture.Sample(samFur,input.texCoord);
float2 shellTC = float2(input.texCoord.x *gHairThickness,input.texCoord.y*gHairThickness);
float4 shellColor = gShellTexture.Sample(samFur,shellTC);
float shadow = lerp(0.0,1,input.layer);
float4 finalCol = color;

//halflambert diffuse
float strength = dot(input.normal, -gLightDir);
strength = strength * 0.5 + 0.5;
strength = saturate(strength);

finalCol.rgb = finalCol.rgb*strength;
finalCol *= shadow;
float bend = pow(input.layer, 3);//stay between 0 and 1 but increase exponentionally
								 
finalCol.a = (shellColor.r*randHairFactor)*(1 - input.layer);
if (shellColor.r>0.2) finalCol.a = ( 1-bend);
else finalCol.a = 0;
return finalCol;

}



//Fin
//******************

VS_DATA FinVS(VS_DATA input) {

	return input;

}

void CreateFinVertex(inout TriangleStream<GS_FINOUTPUT> triStream, float3 pos, float3 normal, float2 texCoord, float2 texCoord2)
{
	GS_FINOUTPUT output = (GS_FINOUTPUT)0;

	float3 gravity = mul((gGravity*gGravityForce + (gForceDirection*gForceStrength)), gWorld);

	float3 gravityPos = pos + gravity;

	output.pos = mul(float4(gravityPos, 1.f), gWorldViewProj);
	output.normal = mul(normal, (float3x3)gWorld);
	output.texCoord = texCoord;
	output.texCoord2 = texCoord2;
	triStream.Append(output);

}


[maxvertexcount(4)]
void FinGS(line VS_DATA input[2], inout TriangleStream<GS_FINOUTPUT> triStream)
{
	float3 midPoint = (input[0].pos + input[1].pos) / 2.f;
	float3 averageN = (input[0].normal + input[1].normal) / 2.f;
	float3 viewDirection = normalize(mul(float4(midPoint, 1.f), gWorld).xyz - gViewInverse[3].xyz);

	float isSide = dot(averageN, -viewDirection);
	float tresh = 0.1f;

	if (abs(isSide) < tresh)
	{
		CreateFinVertex(triStream, input[0].pos, input[0].normal, input[0].texCoord, float2(0, 1));
		CreateFinVertex(triStream, input[1].pos, input[1].normal, input[1].texCoord, float2(1, 1));
		float randHairFactor0 = (nrand(input[0].texCoord) % (1 - gLengthMinimumPercentage)) + gLengthMinimumPercentage;
		float randHairFactor1 = (nrand(input[1].texCoord) % (1 - gLengthMinimumPercentage)) + gLengthMinimumPercentage;
		CreateFinVertex(triStream, input[0].pos + input[0].normal*((gFurLength*randHairFactor0)*gFinToShellRatio), input[0].normal, input[0].texCoord, float2(0, 0));
		CreateFinVertex(triStream, input[1].pos + input[1].normal*((gFurLength*randHairFactor1)*gFinToShellRatio), input[1].normal, input[1].texCoord, float2(1, 0));
	}
}

float4 FinPS(GS_FINOUTPUT input) :SV_TARGET{
	float4 color = gFurTexture.Sample(samFur,input.texCoord);
	float4 finColor = gFinTexture.Sample(samFur,input.texCoord2);
	float4 finalCol = color;

	//halflambert diffuse
	float strength = dot(input.normal, -gLightDir);
	strength = strength * 0.5 + 0.5;
	strength = saturate(strength);

	finalCol.rgb = color.rgb*strength;

	float randHairFactor = (nrand(input.texCoord) % (1 - gLengthMinimumPercentage)) + gLengthMinimumPercentage;


	finalCol.a = finColor.r;//*randHairFactor;

	return finalCol;
}

//TECHNIQUES
//**********

technique10 DefaultTechnique {
	pass NoFur {
		SetBlendState(NoBlend, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
		SetDepthStencilState(DepthWrite, 0);
		SetRasterizerState(FrontCull);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}




	pass Shell {
		SetDepthStencilState(DepthWrite, 0);
		SetRasterizerState(NoCulling);
		SetBlendState(AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_4_0, ShellVS()));
		SetGeometryShader(CompileShader(gs_4_0, ShellGS()));
		SetPixelShader(CompileShader(ps_4_0, ShellPS()));
	}

	pass Fin {
		SetDepthStencilState(NoDepthWrite, 0);
		SetRasterizerState(NoCulling);
		SetBlendState(AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_4_0, ShellVS()));
		SetGeometryShader(CompileShader(gs_4_0, FinGS()));
		SetPixelShader(CompileShader(ps_4_0, FinPS()));
	}


}

