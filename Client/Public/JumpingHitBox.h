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

class CJumpingHitBox final : public CBullet
{
public:
	typedef struct JUMPDESC {
		_float fPower = 10.f;
		_float fTime = 0.0f;
		_float fSpeed = 1.0f;
		_vector vOriginPos;
	}JUMPDESC;

private:
	explicit CJumpingHitBox(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CJumpingHitBox(const CJumpingHitBox& rhs);
	virtual ~CJumpingHitBox() = default;

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
	_vector m_vDir;
	CTransform* m_Owner = nullptr;
	JUMPDESC m_JumpDesc;

public:
	static CJumpingHitBox* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END