#include "stdafx.h"
#include "Event_Manager.h"

CEvent_Manager*	CEvent_Manager::m_pInstance = nullptr;

CEvent_Manager::CEvent_Manager()
{

}


CEvent_Manager::~CEvent_Manager()
{
	
}

void CEvent_Manager::Set_Air(_bool flag)
{
	m_OnAir = flag;

	if (flag == false) {
		m_OnAir = flag;
		Take[10] = { false };
		Nowtake = 0;
		TakeStart = false;
		BossAction = false;
		isSlowTime = false;
		SlowTime = 1.0f;
		TimeAcc = 0.0;
	}

}

