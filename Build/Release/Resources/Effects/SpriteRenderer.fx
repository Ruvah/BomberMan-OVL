float4x4 gTransform : WorldViewProjection;
Texture2D gSpriteTexture;
float2 gTextureSize;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

BlendState EnableBlending 
{     
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

DepthStencilState Depth
{
	DepthEnable = false;
	DepthWriteMask = ZERO;

};

RasterizerState BackCulling 
{ 
	CullMode = BACK; 
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
	uint TextureId: TEXCOORD0;
	float4 TransformData : POSITION; //PosX, PosY, Depth (PosZ), Rotation
	float4 TransformData2 : POSITION1; //PivotX, PivotY, ScaleX, ScaleY
	float4 Color: COLOR;	
};

struct GS_DATA
{
	float4 Position : SV_POSITION;
	float4 Color: COLOR;
	float2 TexCoord: TEXCOORD0;
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
	return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, float rotation, float2 rotCosSin, float2 offset, float2 pivotOffset)
{
	if (rotation != 0)
	{
		//Step 3.
		//Do rotation calculations
		//Transform to origin
		float2 origPos = float2(pos.x,pos.y);
		origPos.x -= offset.x;
		origPos.y -= offset.y;
		origPos.x -= pivotOffset.x;
		origPos.y -= pivotOffset.y;
		
		//Rotate
		pos.x = (origPos.x *rotCosSin.x) - (origPos.y*rotCosSin.y);
		pos.y = (origPos.y *rotCosSin.x) + (origPos.x*rotCosSin.y);
		//Retransform to initial position
		pos.x += offset.x;
		pos.y +=offset.y;
	
	}   
	else
	{
		//Step 2.
		//No rotation calculations (no need to do the rotation calculations if there is no rotation applied > redundant operations)
		//Just apply the pivot offset
		
		pos.x -= pivotOffset.x;
		pos.y -= pivotOffset.y;
		
	}

	//Geometry Vertex Output
	GS_DATA geomData = (GS_DATA) 0;
	geomData.Position = mul(float4(pos, 1.0f), gTransform);
	geomData.Color = col;
	geomData.TexCoord = texCoord;
	triStream.Append(geomData);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	//Given Data (Vertex Data)
	float3 position = float3(vertex[0].TransformData.x,vertex[0].TransformData.y,vertex[0].TransformData.z); //Extract the position data from the VS_DATA vertex struct
	float2 offset = vertex[0].TransformData.xy; //Extract the offset data from the VS_DATA vertex struct (initial X and Y position)
	float rotation = vertex[0].TransformData.w; //Extract the rotation data from the VS_DATA vertex struct
	float2 pivot = float2(vertex[0].TransformData2.x,vertex[0].TransformData2.y); //Extract the pivot data from the VS_DATA vertex struct
	float2 scale = float2(vertex[0].TransformData2.z,vertex[0].TransformData2.w); //Extract the scale data from the VS_DATA vertex struct
	float2 texCoord = vertex[0].TextureId; //Initial Texture Coordinate
	float2 rotCosSin = float2(0,0);
	if(rotation != 0.0f)
	{
		rotCosSin = float2(cos(rotation),sin(rotation));
	}
	
	//...

	// LT----------RT //TriangleStrip (LT > RT > LB, LB > RB > RT)
	// |          / |
	// |       /    |
	// |    /       |
	// | /          |
	// LB----------RB
	
	
	
	float halfSizeX = (gTextureSize.x)*scale.x;
	float halfSizeY = (gTextureSize.y)*scale.y;
	
	pivot.x *= halfSizeX;
	pivot.y *= halfSizeY;
	
	float3 LT = float3(position.x,position.y,position.z);
	float3 RT = float3(position.x+halfSizeX ,position.y,position.z);
	float3 RB = float3(position.x+halfSizeX,position.y+halfSizeY,position.z);
	float3 LB = float3(position.x,position.y+halfSizeY,position.z);
	
	float2 LTTex=float2(0,0);
	float2 RTTex=float2(1,0);
	float2 RBTex=float2(1,1);
	float2 LBTex=float2(0,1);

	
	//VERTEX 1 [LT]
	CreateVertex(triStream, LT, vertex[0].Color, LTTex, rotation, rotCosSin, offset, pivot); //Change the color data too!

	//VERTEX 2 [RT]
	CreateVertex(triStream, RT, vertex[0].Color, RTTex, rotation, rotCosSin, offset, pivot); //Change the color data too!

	//VERTEX 3 [LB]
	CreateVertex(triStream, LB, vertex[0].Color, LBTex, rotation, rotCosSin, offset, pivot); //Change the color data too!
	

	//VERTEX 4 [RB]
	CreateVertex(triStream, RB, vertex[0].Color, RBTex, rotation, rotCosSin, offset, pivot); //Change the color data too!
	

}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET {

	return gSpriteTexture.Sample(samPoint, input.TexCoord) * input.Color;	
}

// Default Technique
technique10 Default {

	pass p0 {
		SetRasterizerState(BackCulling);
		SetBlendState(EnableBlending,float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetDepthStencilState(Depth,0);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}
