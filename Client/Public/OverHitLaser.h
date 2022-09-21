#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class COverHitLaser final : public CGameObject
{
private:
	explicit COverHitLaser(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit COverHitLaser(const COverHitLaser& rhs);
	virtual ~COverHitLaser() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:	
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;	
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pAABB = nullptr;

private:
	void Collider_Enemy();

private:
	_float m_Damage = 10.f;

	_float m_Ambient = 0.f;
	_bool m_Damaged = false;
	_float m_AccTime = 0.f;
	_float m_ScailSize = 5.f;

	_float m_Duration = 3.f;
	_float m_AccDuration = 0.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	
public:
	static COverHitLaser* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END