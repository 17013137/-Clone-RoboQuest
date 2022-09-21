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

class CEnemyBullet final : public CBullet
{
private:
	explicit CEnemyBullet(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CEnemyBullet(const CEnemyBullet& rhs);
	virtual ~CEnemyBullet() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	_int Collide_Enemy(_double Timedelta);
	void Create_Trail();

public:
	void Bound_Bullet();

private:
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CCollider*			m_pSphereCom = nullptr;

private:
	_double m_TimeTotal = 0.0;
	_float3 m_MyColor = _float3(1.f, 0.f, 0.f);
	_float m_fAlpha = 1.0f;
public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CEnemyBullet* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END