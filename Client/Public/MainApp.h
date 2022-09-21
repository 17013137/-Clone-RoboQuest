#pragma once

#include "Client_Defines.h"
#include "Base.h"


BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
public:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT NativeConstruct();
	void Tick(_double TimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = nullptr;

	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

	CRenderer*				m_pRenderer = nullptr;
	_uint					m_iNumRender = 0;

public:
	HRESULT Open_Level(LEVEL eLevelIndex);
	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Prototype_Component();	


public:
	RECT m_Rect;
	_bool m_Clip = false;
	static CMainApp* Create();
	virtual void Free() override;
};

END



