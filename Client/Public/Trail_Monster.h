#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Trail;
class CVIBuffer_Trail;
END

BEGIN(Client)

class CTrail_Monster final : public CGameObject
{
private:
	explicit CTrail_Monster(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CTrail_Monster(const CTrail_Monster& rhs);
	virtual ~CTrail_Monster() = default;

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
	CGameObject*			m_pTarget = nullptr;
	_float3 m_Color = _float3(1.f, 0.f, 0.f);
	_float m_fAlpha = 1.f;
public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	
public:
	static CTrail_Monster* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END