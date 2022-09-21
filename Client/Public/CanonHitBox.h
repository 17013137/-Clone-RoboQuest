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

class CCanonHitBox final : public CBullet
{
private:
	explicit CCanonHitBox(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CCanonHitBox(const CCanonHitBox& rhs);
	virtual ~CCanonHitBox() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Collide_Target(const _tchar* LayerTag);

private:
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CVIBuffer_Rect2*		m_pVIBufferCom = nullptr;
	CCollider*			m_pOBBCom = nullptr;

private:
	_double				m_TotalTime = 0.0;
	_float				m_AccTime = 0.f;
public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	CTransform* m_Owner = nullptr;

public:
	static CCanonHitBox* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END