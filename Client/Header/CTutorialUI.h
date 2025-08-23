#pragma once
#include "CUI.h"
class CTextUI;
class CImageUI;
class CTutorialUI :
    public CUI
{
public:
    explicit CTutorialUI(LPDIRECT3DDEVICE9 gd);
    CTutorialUI(const CTutorialUI& rhs);
    ~CTutorialUI() override = default;

    HRESULT Ready_GameObject() override { return __super::Ready_GameObject(); }
    HRESULT Initialize(void* pArg) override;
    _int    Update_GameObject(const _float& dt) override;
    void    Render_GameObject() override;
    void    Free() override { __super::Free(); }

    static CTutorialUI* Create(LPDIRECT3DDEVICE9 gd);
    CGameObject* Clone(void* pArg = nullptr) override;

private:
    void applyStepText(int stepIndex);
    void applyStepImage(int stepIndex);
    void applyLayoutForStep(int stepIndex);
    void beginSlideIn();
    void updateSlide(const _float& dt);

private:
    CTextUI* m_textMain;
    CTextUI* m_textEmph;
    CImageUI* m_icon;
    int   m_curStep;

    float m_targetMainX, m_targetMainY;
    float m_targetEmphX, m_targetEmphY;
    float m_targetIconX, m_targetIconY;

    float m_iconW, m_iconH;
    float m_emphW, m_emphH;
    float m_mainW, m_mainH;

    bool  m_sliding;
    float m_slideT;
    float m_slideDuration;
    float m_slideStartOffset;

    bool  m_autoHide;
    float m_autoHideTimer;
    float m_autoHideDelay;
};

