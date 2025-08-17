#pragma once
#include "CUI.h"
class CImageUI;
class CLisaUI :
    public CUI
{
public:
    enum class AnimState { Default, Bye }; 

protected:
    explicit CLisaUI(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CLisaUI(const CLisaUI& rhs);
    virtual ~CLisaUI();

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
    static CLisaUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void Free() override;

protected: 
    HRESULT Create_HairPart();             
    void    SetState(AnimState st);          
    void    ApplyStateToParts(AnimState st);   

protected: 
    AnimState m_state;                      
    CImageUI* m_pHair;                         

    map<const _tchar*, CTexture*> m_mapTextures; 
    wstring m_CurrentAnimTag;              
};

