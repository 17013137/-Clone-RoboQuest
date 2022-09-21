#pragma once
#include "fmod/fmod.h"
#include "Base.h"

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

BEGIN(Engine)

class ENGINE_DLL CSound_Manager final : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)

public:
	enum CHANNELID { BGM, PLAYER, PLAYER2, MOSNTER, MONSTER1, MONSTER2, MONSTER3, EFFECT, EFFECT1, EFFECT2, EFFECT3, EFFECT4, UI, SYSTEM_EFFECT, SYSTEM_EFFECT2, SYSTEM_EFFECT3,SYSTEM_EFFECT4, SYSTEM_EFFECT5, MAXCHANNEL };

private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();

public:
	HRESULT  VolumeUp(_uint eID, _float _vol);
	HRESULT  VolumeDown(_uint eID, _float _vol);
	HRESULT  BGMVolumeUp(_float _vol);
	HRESULT  BGMVolumeDown(_float _vol);
	HRESULT  Pause(_uint eID);
	HRESULT SoundPlay(TCHAR* pSoundKey, _uint eID, _float _vol);
	HRESULT PlayBGM(TCHAR* pSoundKey);
	HRESULT StopSound(_uint eID);
	HRESULT StopAll();

	_uint Get_ChannelSize() { return m_ChannelCnt; }
	void ReSet_Channel() { m_ChannelCnt = 0; }

private:
	float m_volume = SOUND_DEFAULT;
	float m_BGMvolume = SOUND_DEFAULT;
	FMOD_BOOL m_bool;
	_uint m_ChannelCnt = 0;
private:
	HRESULT LoadSoundFile();

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem = nullptr;

	_bool		m_bPause = false;

public:
	virtual void Free() override;
};

END

