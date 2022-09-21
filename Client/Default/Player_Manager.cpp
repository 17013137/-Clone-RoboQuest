#include "stdafx.h"
#include "Player_Manager.h"

CPlayer_Manager*	CPlayer_Manager::m_pInstance = nullptr;

CPlayer_Manager::CPlayer_Manager()
{
	//ZeroMemory(m_bKeyState, sizeof(m_bKeyState));
}


CPlayer_Manager::~CPlayer_Manager()
{
	
}

