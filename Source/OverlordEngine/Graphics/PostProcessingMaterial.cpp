//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "PostProcessingMaterial.h"
#include "RenderTarget.h"
#include "../Base/OverlordGame.h"
#include "../Content/ContentManager.h"

ID3D11Buffer* PostProcessingMaterial::m_pVertexBuffer = nullptr;
unsigned int PostProcessingMaterial::m_VertexBufferStride = sizeof(VertexPosTex);

ID3D11InputLayout *PostProcessingMaterial::m_pInputLayout = nullptr;
vector<ILDescription> PostProcessingMaterial::m_pInputLayoutDescriptions = vector<ILDescription>();
UINT PostProcessingMaterial::m_pInputLayoutSize = 0;
UINT PostProcessingMaterial::m_InputLayoutID = 0;

PostProcessingMaterial::PostProcessingMaterial(wstring effectFile, const wstring& technique)
	: m_IsInitialized(false),
	m_pEffect(nullptr),
	m_effectFile(effectFile),
	m_pRenderTarget(nullptr),
	m_pTechnique(nullptr),
	m_TechniqueName(technique)
{
}


PostProcessingMaterial::~PostProcessingMaterial()
{
	//Delete/Release NON-STATIC objects
    delete m_pRenderTarget;
}

void PostProcessingMaterial::Destroy()
{
	//Delete/Release/Clear STATIC objects/lists
    SafeRelease(m_pVertexBuffer);
    SafeRelease(m_pInputLayout);
}

void PostProcessingMaterial::Initialize(const GameContext& gameContext)
{
	if(!m_IsInitialized)
	{
		//1. LoadEffect (LoadEffect(...))
        LoadEffect(gameContext, m_effectFile);
		//2. CreateInputLaytout (CreateInputLayout(...))
        CreateInputLayout(gameContext, m_pTechnique);
		//   CreateVertexBuffer (CreateVertexBuffer(...)) > As a TriangleStrip (FullScreen Quad)
        CreateVertexBuffer(gameContext);
		//3. Create RenderTarget (m_pRenderTarget)
        m_pRenderTarget = new RenderTarget{ gameContext.pDevice };
        
		//		Take a look at the class, figure out how to initialize/create a RenderTarget Object
		//		GameSettings > OverlordGame::GetGameSettings()
        RENDERTARGET_DESC desc{};
        desc.Width = OverlordGame::GetGameSettings().Window.Width;
        desc.Height = OverlordGame::GetGameSettings().Window.Height;
		desc.EnableColorSRV = true;
        m_pRenderTarget->Create(desc);
	}
}

bool PostProcessingMaterial::LoadEffect(const GameContext& gameContext, const wstring& effectFile)
{
	UNREFERENCED_PARAMETER(gameContext);

	//Load Effect through ContentManager
    m_pEffect = ContentManager::Load<ID3DX11Effect>(effectFile);
	//Check if m_TechniqueName (default constructor parameter) is set
	// If SET > Use this Technique (+ check if valid)
	// If !SET > Use Technique with index 0
    if (!m_TechniqueName.empty())
    {
        auto techString = string(m_TechniqueName.begin(), m_TechniqueName.end());
        m_pTechnique = m_pEffect->GetTechniqueByName(techString.c_str());
    }
    else
    {
        m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
    }

	//Call LoadEffectVariables
    LoadEffectVariables();

	return true;
}

//Static Function
void PostProcessingMaterial::Draw(const GameContext& gameContext, RenderTarget* previousRendertarget)
{
	//1. Clear the object's RenderTarget (m_pRenderTarget) [Check RenderTarget Class]
    FLOAT color[4] = { 1,0,0,1 };
    m_pRenderTarget->Clear(gameContext, color);
	//2. Call UpdateEffectVariables(...)
    UpdateEffectVariables(previousRendertarget);
	//3. Set InputLayout
    gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);
	//4. Set VertexBuffer
    UINT offset{ 0 };
    gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &offset);
	//5. Set PrimitiveTopology (TRIANGLESTRIP!!)
    gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//6. Draw (NOT Indexed!)
    D3DX11_TECHNIQUE_DESC techDesc;
    m_pTechnique->GetDesc(&techDesc);
    for (UINT p = 0; p< techDesc.Passes; ++p)
    {
        m_pTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
        gameContext.pDeviceContext->Draw(4, 0);
    }
    
	//7. Unbind the PreviousRendertarget's ShaderResource view from the pipeline. Because we'll need to use it as RenderTarget later on. 
    //   A resource can't be bound as RenderTarget & ShaderResource at the same time. See what happens when you comment these last lines.


    // Unbind SRV from PS-Stage > So it can be used as a RenderTarget later on
    ID3D11ShaderResourceView *const pSRV[] = { nullptr };
    gameContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);

}

//Static Function
void PostProcessingMaterial::CreateVertexBuffer(const GameContext& gameContext)
{
	//Static Buffer Object (Created once, shared between other PPMaterials)
	if (m_pVertexBuffer)
		return;

	//Create a vertex buffer for a full screen quad. Use the VertexPosTex struct (m_pVertexBuffer)
	//Primitive Topology: TriangleStrip
	//Vertex Amount: NUM_VERTS (4)
	//Vertex Struct: VertexPosTex
	//Use NDC to define your vertex positions
    vector<VertexPosTex> vertices
    {
        VertexPosTex{ XMFLOAT3{-1,  1, 0}, XMFLOAT2{0, 0} },
        VertexPosTex{ XMFLOAT3{ 1,  1, 0}, XMFLOAT2{1, 0} },
        VertexPosTex{ XMFLOAT3{-1, -1, 0}, XMFLOAT2{0, 1} },
        VertexPosTex{ XMFLOAT3{ 1, -1, 0}, XMFLOAT2{1, 1} },
    };

    D3D11_BUFFER_DESC buffer = {};
    buffer.Usage = D3D11_USAGE_DEFAULT;
    buffer.ByteWidth = vertices.size() * sizeof(VertexPosTex);
    buffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer.CPUAccessFlags = 0;
    buffer.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = vertices.data();
    HRESULT hr = gameContext.pDevice->CreateBuffer(&buffer, &initData, &m_pVertexBuffer);

    //+ Check for errors (HRESULT)
    Logger::LogHResult(hr, L"Failed to Create VertexBuffer");

	//Set 'm_VertexBufferStride' (Size in bytes of VertexPosTex)
    m_VertexBufferStride = sizeof(VertexPosTex);

	// Create vertex array containing three elements in system memory

}

void PostProcessingMaterial::CreateInputLayout(const GameContext& gameContext, ID3DX11EffectTechnique* pTechnique)
{
	//Static InputLayout Object (Created once, shared between other PPMaterials)
	if (m_pInputLayout)
		return;

	//Create a simple InputLayout for these effects
    bool ok = EffectHelper::BuildInputLayout(gameContext.pDevice, pTechnique, &m_pInputLayout, m_pInputLayoutDescriptions, m_pInputLayoutSize, m_InputLayoutID);
 
	//+ Check for errors (HRESULT)
    if(!ok)
    {
        Logger::LogWarning(L"PostProcessingMaterial::CreateInputLayout: Failed to CreateInputLayout");
    }
}

RenderTarget* PostProcessingMaterial::GetRenderTarget() const
{
	return m_pRenderTarget;
}