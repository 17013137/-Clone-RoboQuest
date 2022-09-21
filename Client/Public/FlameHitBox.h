#pragma once

#include "Client_Defines.h"
#include "Bullet.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CFlameHitBox final : public CBullet
{
private:
	explicit CFlameHitBox(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CFlameHitBox(const CFlameHitBox& rhs);
	virtual ~CFlameHitBox() = default;

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
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	//CCollider*			m_pAABBCom = nullptr;
	CCollider*			m_pSphereCom = nullptr;


private:
	_float4x4			m_ProjMatrix;
	_double				m_TotalTime = 0.0;

public:
	HRESULT SetUp_Components();
	CTransform* m_Owner = nullptr;

public:
	static CFlameHitBox* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END