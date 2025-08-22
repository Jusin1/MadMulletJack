#pragma once
#include "CImageUI.h"
class CItemUI :
    public CImageUI
{
protected:
    explicit CItemUI(LPDIRECT3DDEVICE9 dev);
    explicit CItemUI(const CItemUI& rhs);
    virtual ~CItemUI();

public:
    HRESULT Ready_GameObject() override;
    HRESULT Initialize(void* pArg) override;
    _int    Update_GameObject(const _float& dt) override;
    void    LateUpdate_GameObject(const _float& dt) override;
    void    Render_GameObject() override;
    void    Free() override;

public:
    static CItemUI* Create(LPDIRECT3DDEVICE9 dev);
    CGameObject* Clone(void* pArg = nullptr) override;

public:
    void PlayAppear(float x, float y, float w, float h, float duration = 0.35f);

    void StartBlink(float periodSec = 1.0f, float onRatio = 0.5f,
        bool alphaMode = true, BYTE onAlpha = 255, BYTE offAlpha = 0);
    void StopBlink();

private:
    bool  m_bAppearing;
    float m_fAppearTime;
    float m_fAppearDuration;
    float m_startW, m_startH;
    float m_targetW, m_targetH;
    float m_targetX, m_targetY;

    bool  m_bBlink;           
    bool  m_blinkArmed;       
    float m_blinkPeriod;      
    float m_blinkOnRatio;     
    float m_blinkT;           
    bool  m_blinkAlphaMode;   
    BYTE  m_blinkAlphaOn;     
    BYTE  m_blinkAlphaOff;    
};

