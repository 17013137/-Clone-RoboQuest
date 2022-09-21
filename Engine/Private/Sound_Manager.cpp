#include"../Public/fmod/fmod.hpp"
#include"../Public/fmod/fmod_errors.h"
#include "../Public/Sound_Manager.h"

IMPLEMENT_SINGLETON(CSound_Manager)

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize()
{
	
	//FMOD 시스템 디바이스 생성
	FMOD_System_Create(&m_pSystem, FMOD_VERSION);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	LoadSoundFile();

	return S_OK;
}

HRESULT CSound_Manager::VolumeUp(_uint eID, _float _vol)
{
	if (m_volume < SOUND_MAX) {
		m_volume += _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], m_volume);

	return S_OK;
}

HRESULT CSound_Manager::VolumeDown(_uint eID, _float _vol)
{
	if (m_volume > SOUND_MIN) {
		m_volume -= _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], m_volume);

	return S_OK;
}

HRESULT CSound_Manager::BGMVolumeUp(_float _vol)
{
	if (m_BGMvolume < SOUND_MAX) {
		m_BGMvolume += _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[BGM], m_BGMvolume);

	return S_OK;
}

HRESULT CSound_Manager::BGMVolumeDown(_float _vol)
{
	if (m_BGMvolume > SOUND_MIN) {
		m_BGMvolume -= _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[BGM], m_BGMvolume);

	return S_OK;
}

HRESULT CSound_Manager::Pause(_uint eID)
{
	m_bPause = !m_bPause;
	FMOD_Channel_SetPaused(m_pChannelArr[eID], m_bPause);

	return S_OK;
}




HRESULT CSound_Manager::SoundPlay(TCHAR * pSoundKey, _uint eID, _float _vol) //사운드 파일 이름, 채널 32개까지  , 볼륨크기
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return E_FAIL;

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr,FALSE, &m_pChannelArr[eID]);
		if (_vol >= SOUND_MAX)
			_vol = 1.f;
		else if (_vol <= SOUND_MIN)
			_vol = 0.f;
		FMOD_Channel_SetVolume(m_pChannelArr[eID], _vol);
	}
	FMOD_System_Update(m_pSystem);

	return S_OK;
}

HRESULT CSound_Manager::PlayBGM(TCHAR * pSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return E_FAIL;
	
	FMOD_System_PlaySound(m_pSystem,iter->second,nullptr, FALSE, &m_pChannelArr[BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[BGM], FMOD_LOOP_NORMAL);
	FMOD_System_Update(m_pSystem);

	return S_OK;
}

HRESULT CSound_Manager::StopSound(_uint eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);

	return S_OK;
}

HRESULT CSound_Manager::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);

	return S_OK;
}

HRESULT CSound_Manager::LoadSoundFile()
{
	WIN32_FIND_DATAW fd;
	HANDLE handle = FindFirstFileW(L"../../Client/Bin/Sound/*.*", &fd);
	if (handle == INVALID_HANDLE_VALUE)
		return E_FAIL;

	int iResult = 1;

	char szCurPath[128] = "../../Client/Bin/Sound/";
	char szFullPath[128] = "";
	char szFilename[MAX_PATH];
	while (iResult == 1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.cFileName, -1, szFilename, sizeof(szFilename), NULL, NULL);
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, szFilename);
		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
		if (eRes == FMOD_OK)
		{
			size_t iLength = strlen(szFilename) + (size_t)1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
			MultiByteToWideChar(CP_ACP, 0, szFilename, (int)iLength, pSoundKey, (int)iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}


		iResult = FindNextFile(handle, &fd);
		
	}
	FMOD_System_Update(m_pSystem);
	FindClose(handle);



	return S_OK;	
}

void CSound_Manager::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	FMOD_System_Close(m_pSystem);
	FMOD_System_Release(m_pSystem);
	
}
