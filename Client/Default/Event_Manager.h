#pragma once

#include "Client_Defines.h"
#include "Base.h"

class CEvent_Manager
{
private:
	CEvent_Manager();
	~CEvent_Manager();

public:
	enum EVENTTYPE{EVENT_TOWER, EVENT_DISCO, EVENT_END};

	static CEvent_Manager*		Get_Instance(void)
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CEvent_Manager;

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
	void Set_Air(_bool flag);
	_bool Get_Air() { return m_OnAir; }

	void Set_TakeStart(_bool flag) { TakeStart = flag; }
	_bool Get_TakeStart() { return TakeStart; }

	void Set_Take(_int TakeIndex, _bool flag) { Take[TakeIndex] = flag; }
	_bool Get_TakeState(_int TakeIndex) { return Take[TakeIndex]; }

	_int Get_NowTake() { return Nowtake; }
	void Set_NowTake(_int flag) { Nowtake = flag; }

	void Set_BossAction(_bool flag) { BossAction= flag; }
	_bool Get_BossAction() { return BossAction; }

	_bool Get_isSlowTime() { return isSlowTime; }
	void Set_isSlowTime(_bool flag) { isSlowTime = flag; }

	void Set_SlowTime(_float flag) { SlowTime = flag; }
	_double Get_SlowTime() { return SlowTime; }
	
	void Add_TimeAcc(_double Timedelta) { TimeAcc += Timedelta; }
	_double Get_TimeAcc() { return TimeAcc; }
	void ReSet_TimeAcc() { TimeAcc = 0.f; }

	void Set_EventType(_int flag) { EventType = (EVENTTYPE)flag; }
	_int Get_EventType() { return EventType; }

	void Set_Fade(_bool flag) { Fade= flag; }
	_bool Get_Fade() { return Fade; }


private:
	_bool m_OnAir = false;
	_bool Take[10] = { false };
	_uint Nowtake = 0;
	_bool TakeStart = false;
	_bool BossAction = false;
	_bool isSlowTime = false;
	_double SlowTime = 1.0f;
	_bool Fade = false;
	_double TimeAcc = 0.0;
	EVENTTYPE EventType = EVENT_TOWER;
private:
	static	CEvent_Manager*	m_pInstance;
};

