#pragma once
#include "stdafx.h"
#include "Graphics\Material.h"

class TextureData;

class FurMaterial : public Material
{
public:
	FurMaterial();
	~FurMaterial();

	//light
	void SetLightDir(XMFLOAT3 direction);

	//diffuse
	void SetDiffuseTexture(const wstring& assetFile);
	void SetDiffuseColor(XMFLOAT4 color);
	void SetUVScale(float UVScale);

	//furspecific
	void SetFurLength(float length);
	void SetGravity(XMFLOAT3 gravity);
	void SetGravityStrength(float strength);
	void SetForceDirection(XMFLOAT3 Direction);
	void SetForceStrength(float strength);
	void SetMinHairPercent(float min);

	void SetLayer(float layer);
	//shell
	void SetThickness(float thickness);
	void SetShellTex(const wstring& assetFile);
	//fins
	void SetFinToShellRatio(float ratio);
	void SetFinTexture(const wstring& assetFile);


protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent);

private:
	//Light
	XMFLOAT3 m_LightDir;
	static ID3DX11EffectVectorVariable* m_pLightDirVariable;

	//diffuse
	TextureData * m_pDiffuseTexture;
	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;
	XMFLOAT4 m_ColorDiffuse;
	static ID3DX11EffectVectorVariable* m_pDiffuseColorVariable;
	float m_UVScale;
	static ID3DX11EffectScalarVariable* m_pUVVar;

	//furspecific
	float m_Length;
	static ID3DX11EffectScalarVariable* m_pHairLengthVariable;

	XMFLOAT3 m_Gravity;
	static ID3DX11EffectVectorVariable* m_pGravityVariable;
	float m_GravStrength;
	static ID3DX11EffectScalarVariable* m_pGravStrVar;

	XMFLOAT3 m_Force;
	static ID3DX11EffectVectorVariable* m_pForceVariable;
	float m_ForceStrength;
	static ID3DX11EffectScalarVariable* m_pForceStrVar;

	float m_MinHairPercent;
	static ID3DX11EffectScalarVariable* m_pMinLengthVar;

	//shell

	TextureData * m_pShellTexture;
	static ID3DX11EffectShaderResourceVariable* m_pShellSRVvariable;
	float m_FurThickness;
	static ID3DX11EffectScalarVariable* m_pFThickVar;

	//fin
	float m_FinToShellRatio;
	static ID3DX11EffectScalarVariable* m_pFinToShellVar;
	TextureData * m_pFinTexture;
	static ID3DX11EffectShaderResourceVariable* m_pFinSRVvariable;

	int m_Layers;
	static ID3DX11EffectScalarVariable* m_pLayerVar;

	FurMaterial(const FurMaterial & obj) = delete;
	FurMaterial& operator=(const FurMaterial& obj) = delete;

};

