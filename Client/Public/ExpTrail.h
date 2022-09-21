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

class CExpTrail final : public CGameObject
{
private:
	explicit CExpTrail(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CExpTrail(const CExpTrail& rhs);
	virtual ~CExpTrail() = default;

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
	_float3 m_Color = _float3(0.f, 0.705f, 0.98f);
	_float m_fAlpha = 1.f;
public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	
public:
	static CExpTrail* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END