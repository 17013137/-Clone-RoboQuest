#pragma once

#include "Base.h"

/* ���� ȭ�鿡 ��������ψd ������ �ּҸ� ������ �ִ´�. */
/* ���� ������ ƽ ����ȣ��. */
/* ���� ������ ���� ����ȣ��. */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT OpenLevel(_uint iLevelIndex, class CLevel* pLevel);
	void Tick(_double TimeDelta);
	HRESULT Render();

public:
	const _uint& Get_CurrentLevelIndex() { return m_CurreintLevelIndex; }
	void Set_CurrentLevel(_uint LevelIndex) { m_CurreintLevelIndex = LevelIndex; }
private:
	_uint					m_iLevelIndex = 0;
	_uint					m_CurreintLevelIndex = 0;
	class CLevel*			m_pCurrentLevel = nullptr;

public:
	virtual void Free() override;
};

END