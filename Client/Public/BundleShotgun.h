#pragma once

#include "Client_Defines.h"
#include "WeaponObj.h"
#include "Weapon.h"
#include "Player_Manager.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CBundleShotgun final : public CWeaponObj
{
public:

private:
	explicit CBundleShotgun(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CBundleShotgun(const CBundleShotgun& rhs);
	virtual ~CBundleShotgun() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;


	void Anim_Tick(_double TimeDelta);
	HRESULT Update_WeaponState();
	void Update_Growing();

private:
	CCollider*			m_pAABBCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;	
	CModel*				m_pModelCom = nullptr;
	CWeapons*			m_pWeaponCom = nullptr;

	CModel*				m_pHuntingCom= nullptr;
	CModel*				m_pTacticalCom = nullptr;
	CModel*				m_pTripleCom = nullptr;

private:
	WEAPONDESC m_WeaponDesc;
	WEAPONS m_Weapons = SHOTGUN0;
	_vector m_MuzzlePos[3] = { XMVectorSet(0.f, 0.166f, -1.14f, 1.f),  XMVectorSet(0.f, 0.085f, -1.65f, 1.f) , XMVectorSet(0.f, 0.2f, -1.1f, 1.f) };
	_uint m_BulletCnt = 5;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	CPlayer_Manager::PLAYER_STATE m_PlayerState = CPlayer_Manager::PLAYER_IDLE;

public:
	static CBundleShotgun* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END