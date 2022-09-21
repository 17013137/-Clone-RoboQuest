#pragma once

#include "Client_Defines.h"
#include "Base.h"

class CWeapon_Effect_Manager
{
private:
	CWeapon_Effect_Manager();
	~CWeapon_Effect_Manager();

public:
	static CWeapon_Effect_Manager*		Get_Instance(void)
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CWeapon_Effect_Manager;

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

	void Set_MuzzlePos(_fvector flag) { m_MuzzlePos = flag; }
	_vector Get_MuzzlePos() { return m_MuzzlePos; }

private:
	_vector m_MuzzlePos = _vector();


private:
	static	CWeapon_Effect_Manager*	m_pInstance;
};

