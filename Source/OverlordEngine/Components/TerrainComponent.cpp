//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "TerrainComponent.h"

#include "..\Base\GeneralStructs.h"
#include "../Content/ContentManager.h"
#include "../Helpers/EffectHelper.h"
#include "TransformComponent.h"
#include "../Graphics/TextureData.h"
#include "../Physx/PhysxManager.h"
#include "../Physx/PhysxProxy.h"
#include "../Scenegraph/GameObject.h"
#include "../Scenegraph/GameScene.h"
#include "../Components/Components.h"


TerrainComponent::TerrainComponent(const wstring& heightMapFile, const wstring& textureFile, unsigned int nrOfRows, unsigned int nrOfColumns, float width, float depth, float maxHeight) :
	m_HeightMapFile(heightMapFile),
	m_TextureFile(textureFile),
	m_NrOfRows(nrOfRows),
	m_NrOfColumns(nrOfColumns),
	m_NrOfVertices(nrOfRows*nrOfColumns),
	m_NrOfTriangles(2 * (nrOfRows - 1) * (nrOfColumns - 1)),
	m_Width(width),
	m_Depth(depth),
	m_MaxHeight(maxHeight)
{
	
}

TerrainComponent::~TerrainComponent()
{
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
}

void TerrainComponent::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	//Load Effect
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"Resources/Effects/PosNormTex3D.fx");
	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);

	//TODO: Error handling
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_pMatWorldVariable = m_pEffect->GetVariableByName("gWorld")->AsMatrix();
	m_pDiffuseVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();

	UINT ilSize = 0;
	if (!EffectHelper::BuildInputLayout(gameContext.pDevice, m_pTechnique, &m_pInputLayout, ilSize))
		Logger::LogError(L"TerrainComponent::Initialize >> BuildInputLayout failed!");

	//Texture
	m_pTextureData = ContentManager::Load<TextureData>(m_TextureFile);

	//Load Height Map
	ReadHeightMap();

	//Create Vertices & Triangles
	CalculateVerticesAndIndices();

	//Build Vertexbuffer
	BuildVertexBuffer(gameContext);

	//Build Indexbuffer
	BuildIndexBuffer(gameContext);

	//Create PhysX Heightfield
	CreatePxHeightField();

}

void TerrainComponent::Update(const GameContext& gameContext)
{ 
	UNREFERENCED_PARAMETER(gameContext);
}

void TerrainComponent::Draw(const GameContext& gameContext)
{ 
	XMMATRIX world = XMLoadFloat4x4(&GetTransform()->GetWorld());
	XMMATRIX viewProj = XMLoadFloat4x4(&gameContext.pCamera->GetViewProjection());
	XMMATRIX wvp = XMMatrixMultiply(world, viewProj);
	m_pMatWorldVariable->SetMatrix(reinterpret_cast<float*>(&world));
	m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<float*>(&wvp));
	m_pDiffuseVariable->SetResource(m_pTextureData->GetShaderResourceView());

	// Set vertex buffer
	UINT stride = sizeof(VertexPosNormTex);
	UINT offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set index buffer
	gameContext.pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the input layout
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);

	// Set primitive topology
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render a cube
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p< techDesc.Passes; ++p)
	{
		m_pTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

//Exercise - Heightmap
void TerrainComponent::ReadHeightMap()
{
    m_VecHeightValues.resize(m_NrOfVertices, 0);

    std::ifstream inFile{};
    inFile.open(m_HeightMapFile, std::ios_base::binary);
    if(!inFile)
    {
        Logger::LogError(L"Loading terrain failed");
        return;
    }
    inFile.read(reinterpret_cast<char*>(&m_VecHeightValues[0]), static_cast<std::streamsize>(m_NrOfVertices * sizeof(uint16_t)));
}

//Exercise - Flat Grid
void TerrainComponent::CalculateVerticesAndIndices()
{
	//**VERTICES
	//Reserve spots in the buffer
    m_VecVertices.resize(m_NrOfVertices);

	//Calculate the Initial Position (Terrain centered on the origin)
	//Reset the cellXPos Position for each Column

    float cellZPos = m_Depth / 2.0f;
    for (unsigned row = 0; row < m_NrOfRows; ++row)
    {
        float cellXPos = -m_Width / 2.0f;
        for (unsigned col = 0; col < m_NrOfColumns; ++col)
        {
            //1. Position -- Partially Exercise - Heightmap --
            int vertexId = row * m_NrOfColumns + col;
            m_VecVertices[vertexId].Position.x = cellXPos;
            m_VecVertices[vertexId].Position.y = m_VecHeightValues[vertexId] / static_cast<float>(std::numeric_limits<unsigned short>::max()) * m_MaxHeight;
            m_VecVertices[vertexId].Position.z = cellZPos;

            //2. Normal
            m_VecVertices[vertexId].Normal = { 0,1,0 };

            //3. TexCoord -- Exercise - UV --
            m_VecVertices[vertexId].TexCoord.x = col / static_cast<float>(m_NrOfColumns - 1);
            m_VecVertices[vertexId].TexCoord.y = row / static_cast<float>(m_NrOfRows - 1);

            //Move the cellXPos Position (Down)
            cellXPos += m_Width / m_NrOfColumns;
        }
        //Move the cellZPos Position (Right)
        cellZPos -= m_Depth / m_NrOfRows;
    }


    // Indices
    int nrQuadRows = m_NrOfRows - 1;
    int nrQuadCols = m_NrOfColumns - 1;
    for (int row = 0; row < nrQuadRows; ++row)
    {
        for (int col = 0; col < nrQuadCols; ++col)
        {
            int a = row * m_NrOfColumns + col;
            int b = a + 1;
            int c = a + m_NrOfColumns;
            int d = c + 1;
            AddQuad(a, b, c, d);
        }
    }
	

	//Exercise - Normals
	//For each face...
	//Get the positions of 6 vertices
    for (size_t i = 0; i < m_VecIndices.size(); i += 6)
    {
        XMVECTOR vec[6];
        for (int j = 0; j < 6; ++j)
        {
            vec[j] = XMLoadFloat3(&m_VecVertices.at(m_VecIndices.at(i + j)).Position);
        }

        //first triangle
        XMVECTOR v1 = vec[1] - vec[0];
        XMVECTOR v2 = vec[2] - vec[0];
        XMVECTOR normal = XMVector3Cross(v1, v2);
        normal = XMVector3Normalize(normal);
        XMFLOAT3 normalFloat3;
        XMStoreFloat3(&normalFloat3, normal);
        m_VecFaceNormals.emplace_back(normalFloat3);

        //second triangle
        v1 = vec[3] - vec[5];
        v2 = vec[4] - vec[5];
        normal = XMVector3Cross(v1, v2);
        normal = XMVector3Normalize(normal);
        XMStoreFloat3(&normalFloat3, normal);
        m_VecFaceNormals.emplace_back(normalFloat3);
    }

	//iterate through the vertices and calculate a normal for each one of them using the average of the 6 adjacent faces
    int numFacesPerRow = (m_NrOfColumns - 1) * 2;
    XMFLOAT3 normals[6];
    int index[6];

    for (unsigned row = 0; row < m_NrOfRows; ++row)
    {
        for (unsigned col = 0; col < m_NrOfColumns; ++col)
        {
            int centerIndex = numFacesPerRow * row + col * 2;
            index[0] = centerIndex - 1;
            index[1] = centerIndex;
            index[2] = centerIndex + 1;
            index[3] = centerIndex - numFacesPerRow - 2;
            index[4] = centerIndex - numFacesPerRow - 1;
            index[5] = centerIndex - numFacesPerRow - 0;

            //from left front to right back
            //if col==0 is on left edge, there are 
            //no vertices on the left, fill in a illegal index
            if(col == 0)
            {
                index[0] = -1;
                index[3] = -1;
                index[4] = -1;
            }

            //if col==m_NumColumns-1 is on right edge, there are 
            //no vertices on the right, fill in a illegal index
            if(col == m_NrOfColumns)
            {
                index[1] = -1;
                index[2] = -1;
                index[5] = -1;
            }

            //if index<0 or out of range: front or back edge 
            //it may not be used to calculate the average
            XMVECTOR sum{};
            for (int i = 0; i < 6; ++i)
            {
                if(index[i] >= 0 && index[i] < static_cast<int>(m_VecFaceNormals.size()))
                {
                    sum += XMLoadFloat3(&m_VecFaceNormals.at(index[i]));
                }
            }

            //calculate average by normalizing
            int vertexId = row * m_NrOfColumns + col;
            sum = XMVector3Normalize(sum);
            XMStoreFloat3(&m_VecVertices[vertexId].Normal, sum);
        }
    }	
}

//Exercise - Flat Grid
void TerrainComponent::AddTriangle(unsigned int a, unsigned int b, unsigned c)
{
    m_VecIndices.emplace_back(a);
    m_VecIndices.emplace_back(b);
    m_VecIndices.emplace_back(c);
}

//Exercise - Flat Grid
void TerrainComponent::AddQuad(unsigned int a, unsigned int b, unsigned c, unsigned d)
{
    AddTriangle(a, d, c);
    AddTriangle(a, b, d);
}

void TerrainComponent::BuildVertexBuffer(const GameContext& gameContext)
{
	//Vertexbuffer
	D3D11_BUFFER_DESC bd = {};
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(VertexPosNormTex) * m_NrOfVertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = m_VecVertices.data();
	HRESULT hr = gameContext.pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	Logger::LogHResult(hr, L"Failed to Create Vertexbuffer");
}

void TerrainComponent::BuildIndexBuffer(const GameContext& gameContext)
{
	D3D11_BUFFER_DESC bd = {};
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(DWORD) * m_VecIndices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = m_VecIndices.data();
	HRESULT hr = gameContext.pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	Logger::LogHResult(hr, L"Failed to Create Indexbuffer");
	m_NumIndices = m_VecIndices.size();
}

//Exercise - PhysX
void TerrainComponent::CreatePxHeightField()
{
    PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();
    PxMaterial* defaultMaterial = physX->createMaterial(0.5f, 0.5f, 0.1f);

	//Implement PhysX HeightField
    vector<PxHeightFieldSample> samples(m_NrOfColumns * m_NrOfRows);
    
    for (size_t i = 0; i < samples.size(); ++i)
    {
        samples[i].height = static_cast<PxI16>(m_VecHeightValues[i] / static_cast<double>(numeric_limits<unsigned short>::max()) * m_MaxHeight);
    }

    PxHeightFieldDesc desc{};
    desc.format = PxHeightFieldFormat::eS16_TM;
    desc.nbColumns = m_NrOfColumns;
    desc.nbRows = m_NrOfRows;
    desc.samples.data = samples.data();
    desc.samples.stride = sizeof(PxHeightFieldSample);

    PxHeightField* heightField = PhysxManager::GetInstance()->GetPhysics()->createHeightField(desc);
  
    shared_ptr<PxGeometry> geom{ make_shared<PxHeightFieldGeometry>(heightField, PxMeshGeometryFlags(), 1.0f, m_Width / m_NrOfColumns, m_Depth / m_NrOfRows) };
    ColliderComponent* collider{ new ColliderComponent{ geom, *defaultMaterial, PxTransform{PxVec3{-m_Width / 2, 0, m_Depth / 2}, PxQuat{PxPiDivTwo, PxVec3{0,1,0}}} } };
    GetGameObject()->AddComponent(collider);

    RigidBodyComponent* rigidBody = new RigidBodyComponent{ true };
    GetGameObject()->AddComponent(rigidBody);

	//After implementing the physX representation - Fix the lagginess from the PhysX Debugger!!!
}
