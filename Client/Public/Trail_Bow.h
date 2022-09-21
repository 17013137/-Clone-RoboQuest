#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Trail;
END

BEGIN(Client)

class CTrail_Bow final : public CGameObject
{
private:
	explicit CTrail_Bow(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CTrail_Bow(const CTrail_Bow& rhs);
	virtual ~CTrail_Bow() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CVIBuffer_Trail*		m_pVIBufferCom = nullptr;
	CVIBuffer_Trail*		m_pVIBufferCom2 = nullptr;

private:

	CGameObject* m_Target = nullptr;
	_float3 m_Color = _float3(1.f, 0.818f, 0.f);
	_float m_fAlpha = 1.f;
public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	
public:
	static CTrail_Bow* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END