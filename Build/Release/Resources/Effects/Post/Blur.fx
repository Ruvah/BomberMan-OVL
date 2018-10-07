//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
/// Create Rasterizer State (Backface culling) 



DepthStencilState EnableWrite
{
	DepthEnable = true;
	DepthWriteMask = ALL;
};

/// Create Rasterizer State (Backface culling) 
RasterizerState BackCull
{
	CullMode = Back;
};


//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	// Set the Position
	output.Position = float4(input.Position,1.f);
	// Set the TexCoord
	output.TexCoord = input.TexCoord;
	
	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
	float4 color = (float4)0;
	int passes = 5;
	// Step 1: find the dimensions of the texture (the texture has a method for that)
	float2 dimensions;
	gTexture.GetDimensions(dimensions.x,dimensions.y);
	// Step 2: calculate dx and dy (UV space for 1 pixel)	
	float2 d = float2(1 / dimensions.x, 1 / dimensions.y);
	// Step 3: Create a double for loop (5 iterations each)
	//		   Inside the loop, calculate the offset in each direction. Make sure not to take every pixel but move by 2 pixels each time
	//			Do a texture lookup using your previously calculated uv coordinates + the offset, and add to the final color
	float2 currUV = input.TexCoord;
	currUV.x -= (passes/2) *(d*2) ;
	currUV.y += (passes / 2) *(d * 2);

	float2 offset = float2(0,0);
	
	for (int i = 0; i< passes; ++i)
	{
		for (int j = 0;j < passes; ++j)
		{
			color+=gTexture.Sample(samPoint, currUV + offset);
			offset.x += 2 * d;

		}
		offset.x = 0;
		offset.y -= 2 * d;
	}
	// Step 4: Divide the final color by the number of passes (in this case 5*5)
	int totalPasses = passes* passes ;
	color = float4(color.r/(totalPasses),color.g/totalPasses,color.b/totalPasses,1.f);
	// Step 5: return the final color

	return float4(color.r, color.g, color.b, 1.0f);
	
}


//TECHNIQUE
//---------
technique11 Blur
{
    pass P0
    {
		// Set states...
		SetRasterizerState(BackCull);
		SetDepthStencilState(EnableWrite, 0);
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}