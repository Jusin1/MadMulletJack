
#include "CUI.h"

class CImageUI;
class CTextUI;

class CTextEffectUI : public CUI
{
	private:
    explicit CTextEffectUI(LPDIRECT3DDEVICE9 dev);
    explicit CTextEffectUI(const CTextEffectUI& rhs);
    virtual ~CTextEffectUI();

public: 
    static  CTextEffectUI* Create(LPDIRECT3DDEVICE9 dev);
    virtual CGameObject*   Clone(void* pArg = nullptr) override;
    virtual void           Free() override;
    virtual HRESULT        Ready_GameObject() override;

public: 
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int    Update_GameObject(const _float& dt) override;

public: 
    void AddImage(float localX, float localY, D3DCOLOR color, float width, float height);


    void SetupText(const std::wstring& text, float localX, float localY);


    void  SetTextBaseScale(float s);
    float GetTextBaseScale() const { return m_textBaseScale; }

    void SetAutoKill(float seconds);
    void CancelAutoKill();


    void  PlayTextOvershootMove(float totalDur = 0.40f,
                                float overMul  = 1.25f,
                                float offsetX  = 0.f, float offsetY = 40.f);


    void StartRainbow(float intervalSec = 0.2f) { m_rainbowOn = true;  m_interval = intervalSec; m_timer = 0.f; }
    void StopRainbow()                           { m_rainbowOn = false; }

private:
    void applyColorStep();

private:
    std::vector<CImageUI*> m_imgs;
    CTextUI*  m_label;


    bool   m_rainbowOn;
    float  m_interval;
    float  m_timer;
    int    m_idx;


    bool   m_moveOn;
    float  m_moveT;
    float  m_moveDur;
    float  m_moveStartX, m_moveStartY;
    float  m_moveEndX,   m_moveEndY;

    
    float  m_textBaseScale;

    
    D3DXCOLOR m_rainbow[7] = {
        D3DXCOLOR(1.f, 0.f,   0.f,   1.f),
        D3DXCOLOR(1.f, 0.5f,  0.f,   1.f),
        D3DXCOLOR(1.f, 1.f,   0.f,   1.f),
        D3DXCOLOR(0.f, 1.f,   0.f,   1.f),
        D3DXCOLOR(0.f, 0.f,   1.f,   1.f),
        D3DXCOLOR(0.f, 0.2f,  0.6f,  1.f),
        D3DXCOLOR(0.6f,0.f,   1.f,   1.f),
    };

private:
    bool  m_autoKill;   
    float m_lifeSec;    
    float m_lifeT;   
};