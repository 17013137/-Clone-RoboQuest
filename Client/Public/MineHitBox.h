#pragma once

#include "Client_Defines.h"
#include "Bullet.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CCollider;
class CStatus;
END

BEGIN(Client)

class CMineHitBox final : public CBullet
{
public:
	typedef struct JUMPDESC {
		_float fPower = 10.f;
		_float fTime = 0.0f;
		_float fSpeed = 1.0f;
		_vector vOriginPos;
	}JUMPDESC;

private:
	explicit CMineHitBox(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CMineHitBox(const CMineHitBox& rhs);
	virtual ~CMineHitBox() = default;

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
	CCollider*			m_pSphereCom = nullptr;
	CStatus*			m_pStatus = nullptr;

public:
	_int Status_Tick(_double TimeDelta);
	void Create_Trail();
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();



private:
	CTransform* m_Owner = nullptr;
	JUMPDESC m_JumpDesc;
	_float3 m_MyColor = _float3(0.748f, 0.f, 1.f); //200, 0, 255
	_float m_fAlpha = 1.f;

	_double				m_TotalTime = 0.0;
public:
	static CMineHitBox* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END