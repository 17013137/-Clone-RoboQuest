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

class CMortarHitBox final : public CBullet
{
private:
	explicit CMortarHitBox(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CMortarHitBox(const CMortarHitBox& rhs);
	virtual ~CMortarHitBox() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Create_OverHit();

private:
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CCollider*			m_pSphere = nullptr;

private:
	_float4x4			m_ProjMatrix;
	_double				m_Duration = 7.f;
	_double				m_AccDuration = 0.f;

	_bool				m_Falling = false;

private:
	HRESULT SetUp_Components();
	CTransform* m_Player = nullptr;

public:
	static CMortarHitBox* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END