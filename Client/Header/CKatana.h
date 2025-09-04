#pragma once
#include "CMainWeapon.h"
class CImageUI;
class CKatana :
    public CMainWeapon
{
private:
    explicit CKatana(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CKatana(const CKatana& rhs);
    virtual ~CKatana();

public:
    virtual         HRESULT     Ready_GameObject();
    virtual         HRESULT     Initialize(void* pArg)override;
    virtual         _int        Update_GameObject(const _float& fTimeDelta);
    virtual         void        LateUpdate_GameObject(const _float& fTimeDelta);
    virtual         void        Render_GameObject();

private:
    virtual HRESULT Set_Texture() override;
    virtual HRESULT Texture_Clone()  override;
    virtual HRESULT Change_Texture(const _tchar* pTextureTag) override;
    // ÄÞº¸/ÀÌÆåÆ® ÇÔ¼ö
    void OnAttackInput();
    void StartComboStep(int step);
    void SpawnSlashFx(int step); // ÀÌÆåÆ® »ý¼º
    void CleanupFinishedFx(); // ÀÌÆåÆ® Á¤¸®
    void  BeginHit(float seconds);

    // »ç¿îµå
    void CreateKatanaSound();

public:
    void Set_SelfActive(bool b) { m_bSelfActive = b; }
public:
    static CKatana* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void Free() override;

private:
    float m_fWaitTimer = 0.f;          

private:
    class CTransform* m_pPlayerTr;
    class CColider_Sphere* m_pHitCol;
    float                  m_fHitTimer;
    int   m_iComboStep;
    bool  m_bInCombo;
    float m_fComboTimer;
    float m_fComboWindow;
    vector<CImageUI*> m_vSlashFx;
    bool m_bSelfActive;
    map<const _tchar*, CTexture*> m_mapTextures;
    wstring m_CurrentAnimTag;
    CUIBase* m_pBase;
    CImageUI* m_pSheathUI;
    CImageUI* m_pKnifeHandleUI;
    CImageUI* m_pKnifeUI;
    CImageUI* m_pShineKnife;

};

