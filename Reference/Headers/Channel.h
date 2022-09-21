#pragma once

#include "Base.h"

/* 특정 애니메이션을 동작하는데 필요한 뼈의 애니메이션 정보. */
/* 애니메이션 마다 하나의 뼈에 대한 Channel이 여러개 만들어질 수 잇다. */

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	CChannel(const CChannel& rhs);
	virtual ~CChannel() = default;

public:
	void SetUp_HierarchyNodePtr(class CHierarchyNode* pNode) {
		m_pHierarchyNode = pNode;
		Safe_AddRef(m_pHierarchyNode);
	}

	void Set_CurrentKeyFrame(_uint iKeyFrame) {
		m_iCurrentKeyFrame = iKeyFrame;
	}

	CHierarchyNode* Get_HierarchyNode();
public:
	HRESULT NativeConstruct_Prototype(aiNodeAnim* pAIChannel);
	HRESULT NativeConstruct_Prototype(ifstream & In);
	HRESULT NativeConstruct();
	void Compute_TransformationMatrix(_double PlayTime);

public:
	void Linear_TransMatrix(_double PlayTime, _double Duration, KEYFRAME* PrevKeyFrame);
	_uint Get_KeyFrameSize() { return (_uint)m_KeyFrames.size(); }
	KEYFRAME* Get_KeyFrame(_uint iIndex) { return m_KeyFrames[iIndex]; }
	const _uint& Get_CurrentKeyFrameNum() { return m_TempKeyFrame; }

public:
	HRESULT Save_ChannelInfo(ostream& Out);

private:
	char				m_szName[MAX_PATH] = "";

private:
	_uint							m_iNumKeyFrames = 0;
	_uint							m_iCurrentKeyFrame = 0;
	_uint							m_TempKeyFrame = 0;

	vector<KEYFRAME*>				m_KeyFrames;
	typedef vector<KEYFRAME*>		KEYFRAMES;

	class CHierarchyNode*			m_pHierarchyNode = nullptr;

private:
	_matrix							m_TransformationMatrix;
	_bool							m_isCloned = false;

public:
	_uint Get_RelatedIndex(void) { return m_RelatedNodeIndex; }
	void Set_RelatedIndex(_uint iIndex) { m_RelatedNodeIndex = iIndex; }

private:
	_uint							m_RelatedNodeIndex = 0;

private:
	HRESULT Ready_KeyFrames(aiNodeAnim* pAIChannel);

public:
	static CChannel* Create(aiNodeAnim* pAIChannel);
	static CChannel* Create(ifstream& In);
	CChannel* Clone();
	virtual void Free() override;
};

END