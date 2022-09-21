#pragma once

namespace Client
{
	const unsigned int	g_iWinCX = 1280;
	const unsigned int	g_iWinCY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_DUNGEON, LEVEL_BOSSTOWER, LEVEL_BOSSDISCO, LEVEL_END };

	enum WEAPONS { RIFLE0 = 0, RIFLE1 = 2, RIFLE2 = 4, SHOTGUN0 = 6, SHOTGUN1 = 8, SHOTGUN2 = 10, BOW0 = 15, BOW1 = 16, BOW2 = 20};
	//38
	enum POSE_TYPE { TYPE_0, TYPE_1, TYPE_2, POSE_END };

	enum WEAPON_TYPE { TYPE_RIFLE, TYPE_SHOTGUN, TYPE_BOW, TYPEWEAPON_END };
	
	typedef struct tagWeaponStateDesc {
		float fAttackSpeed;
		float fReroadSpeed;
		float fWeaponDamage;
		float fDamageRange;		//공격사거리
		float fCritical_Rate;
		float fRebound;		//총 반동
		float fReboundTime;
	}WEAPONDESC;

}

#include <process.h>

#define		VK_MAX		0xff

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
