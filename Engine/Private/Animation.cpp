#include "..\Public\Animation.h"
#include "Channel.h"
#include "HierarchyNode.h"

CAnimation::CAnimation()
{

}

CAnimation::CAnimation(const CAnimation & rhs)
	: m_Channels(rhs.m_Channels)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_Duration(rhs.m_Duration)
	, m_TickPerSecond(rhs.m_TickPerSecond)
	, m_TimeAcc(rhs.m_TimeAcc)
	, m_isFinished(rhs.m_isFinished)
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);

	strcpy_s(m_szName, rhs.m_szName);

	m_isCloned = true;

}

HRESULT CAnimation::NativeConstruct_Prototype(aiAnimation* pAIAnimation, vector<CHierarchyNode*> Nodes)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_Duration = pAIAnimation->mDuration;
	m_TickPerSecond = pAIAnimation->mTicksPerSecond;


	if (FAILED(Ready_Channels(pAIAnimation, Nodes)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimation::NativeConstruct_Prototype(ifstream & In, vector<class CHierarchyNode*> Nodes)
{
	if (!In)
		return E_FAIL;

	In.getline(m_szName, MAX_PATH);
	In >> m_Duration;
	In >> m_TickPerSecond;

	int iChannelSize = 0;
	In >> iChannelSize;

	m_iNumChannels = iChannelSize;

	char temp[MAX_PATH];
	In.getline(temp, MAX_PATH);

	for (int i = 0; i < iChannelSize; ++i) {
		m_Channels.push_back(CChannel::Create(In));
	}

	for (int i = 0; i < iChannelSize; ++i) {
		m_Channels[i]->SetUp_HierarchyNodePtr(Nodes[m_Channels[i]->Get_RelatedIndex()]);
	}

	return S_OK;
}

HRESULT CAnimation::NativeConstruct(aiAnimation* pAIAnimation, vector<CHierarchyNode*> Nodes)
{
	if (FAILED(Clone_Channels(pAIAnimation, Nodes)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimation::NativeConstruct(vector<class CHierarchyNode*> Nodes)
{
	if (FAILED(Clone_Dat_Channels(Nodes)))
		return E_FAIL;

	return S_OK;
}


void CAnimation::Update(_double TimeDelta, _double Speed, _double LinearSpeed, _bool isRepeat, vector<class CChannel*> PrevChannel, _bool* isLinear)
{
	/* 1. 현재 이 애니메이션이 재생되고 있는 시간을 계산한다. */
	/* 2. 이 시간대에 맞는 키프레임을 찾는다. */
	/* 3. 해당 키프레임사이의 뼈의 행렬값을 선형보간하여 구한다.  */
	/* 4. 이렇게 구한 행렬을 노드에 저장해둔다. */

	if (*isLinear == true) {
		m_LinearDesc.TimeAcc += (TimeDelta * LinearSpeed);

		if (m_LinearDesc.TimeAcc >= m_LinearDesc.Duration) {
			m_LinearDesc.isLinear = false;
			m_LinearDesc.TimeAcc = 0.0;
			*isLinear = false;
			return;
		}

		for (_uint i = 0; i < m_iNumChannels; ++i)
		{
			if (m_LinearDesc.isLinear == false)
				m_Channels[i]->Set_CurrentKeyFrame(0);
			m_Channels[i]->Linear_TransMatrix(m_LinearDesc.TimeAcc, m_LinearDesc.Duration, PrevChannel[i]->Get_KeyFrame(PrevChannel[i]->Get_CurrentKeyFrameNum()));
		}

	}
	else {
		m_TimeAcc += m_TickPerSecond * TimeDelta * Speed;

		if (m_TimeAcc >= m_Duration) {
			m_isFinished = true;
			if (isRepeat == true) {
				m_TimeAcc = 0.0;
			}
		}

		for (_uint i = 0; i < m_iNumChannels; ++i)
		{
			if (m_isFinished == true)
				m_Channels[i]->Set_CurrentKeyFrame(0);

			m_Channels[i]->Compute_TransformationMatrix(m_TimeAcc);
		}
	}
}


KEYFRAME* CAnimation::Get_KeyFrameData(_uint iChannelIndex, _uint iKeyFrameIndex)
{
	return m_Channels[iChannelIndex]->Get_KeyFrame(iKeyFrameIndex);
}

void CAnimation::Set_FinishFalse(_bool FrameZero)
{
	m_isFinished = false;
	m_TimeAcc = 0.0;

	if (FrameZero == true)
		Set_KeyFrameZero();
}

void CAnimation::Set_KeyFrameZero()
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
		m_Channels[i]->Set_CurrentKeyFrame(0);

}

_int CAnimation::Get_CurrentKeyFrame()
{
	_uint temp = 0;

	if (m_Channels.size() > 0) {
		for (int i = 0; i < m_Channels.size(); i++) {
			if (temp < m_Channels[i]->Get_CurrentKeyFrameNum()) {
				temp = m_Channels[i]->Get_CurrentKeyFrameNum();
			}

		}

		return temp;
	}

	return -1;
}

HRESULT CAnimation::Save_AnimationInfo(ostream & Out)
{
	if (!Out)
		return E_FAIL;

	// 애니메이션의 이름, 지속시간, 스피드값 저장
	Out << m_szName << endl;
	Out << m_Duration << endl;
	Out << m_TickPerSecond << endl;

	// 채널의 갯수 저장
	Out << m_Channels.size() << endl;

	// 채널의 정보 저장
	for (auto& Channel : m_Channels) {
		Channel->Save_ChannelInfo(Out);
	}

	return S_OK;
}

HRESULT CAnimation::Ready_Channels(aiAnimation * pAIAnimation, vector<CHierarchyNode*> Nodes)
{
	m_iNumChannels = pAIAnimation->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		aiNodeAnim*			pAIChannel = pAIAnimation->mChannels[i];

		CChannel*			pChannel = CChannel::Create(pAIChannel);
		if (nullptr == pChannel)
			return E_FAIL;

		auto	iter = find_if(Nodes.begin(), Nodes.end(), [&](CHierarchyNode* pNode)
		{
			return !strcmp(pAIChannel->mNodeName.data, pNode->Get_Name());
		});

		if (iter == Nodes.end())
			return E_FAIL;

		pChannel->SetUp_HierarchyNodePtr(*iter);

		pChannel->Set_RelatedIndex(_uint(distance(Nodes.begin(), iter)));

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Clone_Channels(aiAnimation * pAIAnimation, vector<CHierarchyNode*> Nodes)
{
	m_iNumChannels = pAIAnimation->mNumChannels;

	vector<CChannel*>		Channels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		aiNodeAnim*			pAIChannel = pAIAnimation->mChannels[i];

		CChannel*			pChannel = m_Channels[i]->Clone();
		if (nullptr == pChannel)
			return E_FAIL;

		auto	iter = find_if(Nodes.begin(), Nodes.end(), [&](CHierarchyNode* pNode)
		{
			return !strcmp(pAIChannel->mNodeName.data, pNode->Get_Name());
		});

		if (iter == Nodes.end())
			return E_FAIL;

		pChannel->SetUp_HierarchyNodePtr(*iter);

		pChannel->Set_RelatedIndex(_uint(distance(Nodes.begin(), iter)));

		Channels.push_back(pChannel);

		Safe_Release(m_Channels[i]);
	}

	m_Channels.clear();

	m_Channels = Channels;

	return S_OK;
}

HRESULT CAnimation::Clone_Dat_Channels(vector<CHierarchyNode*> Nodes)
{
	vector<CChannel*>		Channels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		// 채널 복사
		CChannel*			pChannel = m_Channels[i]->Clone();
		if (nullptr == pChannel)
			return E_FAIL;

		// 채널과 관련된 상속노드 연결
		if (0 != pChannel->Get_RelatedIndex())
			pChannel->SetUp_HierarchyNodePtr(Nodes.at(pChannel->Get_RelatedIndex()));

		// 채널 추가
		Channels.push_back(pChannel);

		Safe_Release(m_Channels[i]);
	}

	// 기존 채널 날리고
	m_Channels.clear();

	// 새로운 채널 넣기
	m_Channels = Channels;

	return S_OK;
}

CAnimation * CAnimation::Create(aiAnimation* pAIAnimation, vector<CHierarchyNode*> Nodes)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->NativeConstruct_Prototype(pAIAnimation, Nodes)))
	{
		MSG_BOX(TEXT("Failed to Created CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Create(ifstream & In, vector<CHierarchyNode*> Nodes)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->NativeConstruct_Prototype(In, Nodes)))
	{
		MSG_BOX(TEXT("Failed to Created CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Clone(aiAnimation * pAIAnimation, vector<CHierarchyNode*> Nodes)
{
	CAnimation*	pInstance = new CAnimation(*this);

	if (FAILED(pInstance->NativeConstruct(pAIAnimation, Nodes)))
	{
		MSG_BOX(TEXT("Failed to Created CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Clone(vector<CHierarchyNode*> Nodes)
{
	CAnimation*	pInstance = new CAnimation(*this);

	if (FAILED(pInstance->NativeConstruct(Nodes)))
	{
		MSG_BOX(TEXT("Failed to Created CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	if (!m_isCloned) {
		for (auto& pChannel : m_Channels)
			Safe_Release(pChannel);

		m_Channels.clear();
	}
}

