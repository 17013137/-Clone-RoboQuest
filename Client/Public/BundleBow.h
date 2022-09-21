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

class CBundleBow final : public CWeaponObj
{
public:

private:
	explicit CBundleBow(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CBundleBow(const CBundleBow& rhs);
	virtual ~CBundleBow() = default;

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

	CModel*				m_pLongBowCom = nullptr;
	CModel*				m_pCrossBowCom = nullptr;
	CModel*				m_pRocket = nullptr;

private:
	WEAPONDESC m_WeaponDesc;
	WEAPONS m_Weapons = BOW0;


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();


private:
	CPlayer_Manager::PLAYER_STATE m_PlayerState = CPlayer_Manager::PLAYER_IDLE;

public:
	static CBundleBow* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END