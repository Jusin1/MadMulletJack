#pragma once
#include "CScene.h"
#include "Engine_Define.h"
#include "Clinet_Define.h"

class CImageUI;
class CButtonUI;
class CTextUI;

// ·Îµù ¾À
class CLoading_Scene : public Engine::CScene
{
private:
	explicit CLoading_Scene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoading_Scene();

public:
	HRESULT Ready_Loading(SCENE eID);
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene(const _float& fTimeDelta) override;

public:
	static CLoading_Scene* Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENE eID);
	virtual void Free() override;

private: 
	HRESULT ready_ui();
	void    update_progress_ui(float p);

private:
    SCENE          m_eNextScene;
    class CLoader* m_pLoader;

private:
    CImageUI* m_bg;
    CButtonUI* m_barBg;
    CButtonUI* m_barFill;
    CTextUI* m_txtPct;

private:
    float m_barWMax;
    float m_barH;
    float m_barX;
    float m_barY;

private:
    float m_visProg;
    bool  m_switchRequested;
    bool  m_skipVisual;
};
