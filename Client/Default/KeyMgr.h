#pragma once

#include "Client_Defines.h"

class CKeyMgr
{
private:
	CKeyMgr();
	~CKeyMgr();

public:
	bool		Key_Pressing(int iKey);
	bool		Key_Down(int iKey);
	bool		Key_Up(int iKey);

public:
	static CKeyMgr*		Get_Instance(void)
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CKeyMgr;

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

private:
	static		CKeyMgr*	m_pInstance;
	bool		m_bKeyState[VK_MAX];
};

