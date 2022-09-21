#pragma once

#include "Client_Defines.h"
#include "Model.h"

class CPlayer_Manager
{
public:
	enum PLAYER_STATE{ PLAYER_REROAD, PLAYER_ATTACK, PLAYER_IDLE, PLAYER_END};
	
private:
	CPlayer_Manager();
	~CPlayer_Manager();

public:
	static CPlayer_Manager*		Get_Instance(void)
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CPlayer_Manager;

		return m_pInstance;
	}
	static void			Destroy_Instance(void)
	{
		if (nullptr != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

public:
	void Set_PlayerState(PLAYER_STATE State) { m_PlayerState = State; }
	const PLAYER_STATE& Get_PlayerState() { return m_PlayerState; }

	void Set_Weapon(WEAPONS Weapon) { m_NowWeapon = Weapon; }
	const WEAPONS& Get_NowWeapon() { return m_NowWeapon; }

	void Set_PoseType(POSE_TYPE TypeIndex) { m_Type = TypeIndex; }
	const POSE_TYPE& Get_PoseType() { return m_Type; }

	void Set_WeaponType(WEAPON_TYPE TypeIndex) { m_WeaponType = TypeIndex; }
	const WEAPON_TYPE& Get_WeaponType() { return m_WeaponType; }

	void Set_WeaponDesc(WEAPONDESC Weapondesc) { m_WeaponDesc = Weapondesc; }
	const WEAPONDESC& Get_WeaponDesc() { return m_WeaponDesc; }

	void Set_Level(int iLevel, int iWeaponTypeIndex) { m_Level[iWeaponTypeIndex] = iLevel; }
	const int& Get_Level(int iWeaponTypeIndex) { return m_Level[iWeaponTypeIndex]; }
	
	void Set_BulletCnt(_uint Bulletcnt) { m_BulletCnt = Bulletcnt; }
	const _uint& Get_BulletCnt() { return m_BulletCnt; }

	void Set_SkillAttack(_bool flag) { isSkillAttack = flag; }
	const bool& Get_isSkillAttack() { return isSkillAttack; }
	
	void Set_Attack(_bool flag) { isAttack = flag; }
	const bool& Get_isAttack() { return isAttack; }

	void Set_Reroad(_bool flag) { isReroad= flag; }
	const bool& Get_Reroad() { return isReroad; }

	void Set_Exp(_float Exp) { m_Exp += Exp; }
	_float Get_Exp() { return m_Exp; }

	_float Get_NowWeaponExp() { return m_NowWeaponExp;}
	void Set_NowWeaponExp(_float flag) { m_NowWeaponExp = flag; }

	void Set_Hit(_bool flag) { isHit= flag; }
	const bool& Get_Hit() { return isHit; }

	void Set_Sprint(_bool flag) { isSprint = flag; }
	const bool& Get_Sprint() { return isSprint; }

	void Set_Damaged(_bool flag) { isDamaged = flag; }
	const bool& Get_Damaged() { return isDamaged; }

	void Set_RifleBullet(_int flag) { m_RifleBullet = flag; }
	const _int& Get_RifleBullet () { return m_RifleBullet; }

	void Set_ShotgunBullet(_int flag) { m_ShotBullet = flag; }
	const _int& Get_ShotgunBullet() { return m_ShotBullet; }

	void Set_WeaponLevel(_int Type, _int flag) {
		m_WeaponLevel[Type] = flag;
	}
	_int Get_WeaponLevel(_int Type) {
		return m_WeaponLevel[Type];
	}

	void Set_WeaponExp(_int Type, _int flag) { m_WeaponExp[Type] = flag; }
	_int Get_WeaponExp(_int Type) {
		return m_WeaponLevel[Type];
	}

private:
	PLAYER_STATE m_PlayerState = PLAYER_END;
	WEAPONS m_NowWeapon = RIFLE0;
	WEAPON_TYPE m_WeaponType = TYPE_RIFLE;

	int m_Level[WEAPON_TYPE::TYPEWEAPON_END] = { 0 };
	POSE_TYPE m_Type = TYPE_1;
	WEAPONDESC m_WeaponDesc;

private:
	_bool isAttack = false;
	_bool isDamaged = false;
	_bool isSkillAttack = false;
	_bool isReroad = false;
	_bool isHit = false;
	_bool isSprint = false;
	_uint m_BulletCnt = 5;
	_float m_Exp = 0.f;
	_float m_NowWeaponExp = 0.f;
	_int m_RifleBullet = 0;
	_int m_ShotBullet = 0;
	_int m_WeaponLevel[3] = { 0 };
	_int m_WeaponExp[3] = { 0 };
private:
	CModel* m_pNowModel = nullptr;
	static		CPlayer_Manager*	m_pInstance;
};

