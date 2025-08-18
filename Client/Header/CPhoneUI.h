#pragma once
#include "CUI.h"

class CImageUI;
class CPhoneUI :
    public CUI
{
public:
    enum class AnimState { READY, ENTER, SHOP, EXIT};

protected:
    explicit CPhoneUI(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CPhoneUI(const CPhoneUI& rhs);
    virtual ~CPhoneUI();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int    Update_GameObject(const _float& fTimeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void    Render_GameObject() override;

public:
    HRESULT Change_Texture(const _tchar* pTextureTag);
    HRESULT Texture_Clone();

public:
    static CPhoneUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void Free() override;

public:
    void    SetState(AnimState st);


protected:
    AnimState m_state;
    map<const _tchar*, CTexture*> m_mapTextures;
    wstring m_CurrentAnimTag;
};

