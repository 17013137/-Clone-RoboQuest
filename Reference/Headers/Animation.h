#pragma once

#include "Base.h"


BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CBase
{
public:
	typedef struct LINEARDESC {
		_bool isLinear = false;
		_double Duration = 0.1;
		_double TimeAcc = 0.0;

	}LINEARDESC;

private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT NativeConstruct_Prototype(aiAnimation* pAIAnimation, vector<class CHierarchyNode*> Nodes);
	HRESULT NativeConstruct_Prototype(ifstream& In, vector<class CHierarchyNode*> Nodes);
	HRESULT NativeConstruct(aiAnimation* pAIAnimation, vector<class CHierarchyNode*> Nodes);
	HRESULT NativeConstruct(vector<class CHierarchyNode*> Nodes);
	void Update(_double TimeDelta, _double Speed, _double LinearSpeed, _bool isRepeat, vector<class CChannel*> PrevChannel, _bool* isLinear);

public:
	HRESULT Save_AnimationInfo(ostream& Out);

public:
	const _bool& Get_isFinished() { return m_isFinished; }
	vector<class CChannel*> Get_Channel() { return m_Channels; }
	KEYFRAME* Get_KeyFrameData(_uint iChannelIndex, _uint iKeyFrameIndex);
	const _uint& Get_ChannelNum() { return m_iNumChannels; }
	void Set_FinishFalse(_bool FrameZero);
	void Set_KeyFrameZero();
	const LINEARDESC Get_LinearDesc() { return m_LinearDesc; }

	_int Get_CurrentKeyFrame();

private:
	char					m_szName[MAX_PATH] = "";
	_double					m_Duration = 0.0; /* 애니메이션 하나를 재생하는데 걸리는 총 시간. */
	_double					m_TickPerSecond = 0.0;
	_double					m_TimeAcc = 0.0;
	_bool					m_isFinished = false;
	_double					m_TimeAccNext = 0.0;
	LINEARDESC				m_LinearDesc;

private:
	_uint								m_iNumChannels = 0;

	vector<class CChannel*>				m_Channels;
	typedef vector<class CChannel*>		CHANNELS;

private:
	_bool m_isCloned = false;

private:
	HRESULT Ready_Channels(aiAnimation* pAIAnimation, vector<CHierarchyNode*> Nodes);
	HRESULT Clone_Channels(aiAnimation* pAIAnimation, vector<CHierarchyNode*> Nodes);
	HRESULT Clone_Dat_Channels(vector<CHierarchyNode*> Nodes);

public:
	static CAnimation* Create(aiAnimation* pAIAnimation, vector<CHierarchyNode*> Nodes);
	static CAnimation* Create(ifstream& In, vector<CHierarchyNode*> Nodes);
	CAnimation* Clone(aiAnimation* pAIAnimation, vector<CHierarchyNode*> Nodes);
	CAnimation* Clone(vector<CHierarchyNode*> Nodes);
	virtual void Free() override;
};

END