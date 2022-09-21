#pragma once

#include "Client_Defines.h"
#include "Bullet.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect2;
class CCollider;
END

BEGIN(Client)

class CRifleBullet final : public CBullet
{
private:
	explicit CRifleBullet(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CRifleBullet(const CRifleBullet& rhs);
	virtual ~CRifleBullet() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Bound_Bullet();
	void Collison_Bullet(const _tchar* LayerTag);

private:
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CVIBuffer_Rect2*	m_pVIBufferCom = nullptr;
	CCollider*			m_pSphereCom = nullptr;
	
private:
	_double m_TimeTotal = 0.0;
	_float3 m_Color = _float3(0.5f, 0.5f, 0.5f);
	_float m_Distance = 0.f;
	_float m_Alpha = 1.f;
public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();


public:
	static CRifleBullet* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END