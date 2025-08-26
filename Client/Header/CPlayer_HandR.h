#pragma once
#include "CUI.h"
class CPlayer_HandR :public CUI
{
protected:
    explicit CPlayer_HandR(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CPlayer_HandR(const CPlayer_HandR& rhs);
    virtual ~CPlayer_HandR();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int Update_GameObject(const _float& fTimeDelta) override;
    virtual void LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void Render_GameObject() override;

public:
    HRESULT Change_Texture(const _tchar* pTextureTag);

private:
    virtual HRESULT Set_Texture() override; // palyerInfo에 따라 texture 셋팅
    HRESULT Texture_Clone();
  

private:
    map<const _tchar*, CTexture*> m_mapTextures;    // 애니메이션 텍스쳐
    wstring m_CurrentAnimTag;                       // 현재 애니메이션 태그

    //getter setter func
public:
    PlayerStateInfo Get_Info() const { return m_tInfo; }
    void Set_Info(PlayerStateInfo _tPlayerStateInfo) { m_tInfo = _tPlayerStateInfo; }

private:
    PlayerStateInfo m_tInfo;
    
public:
    static CPlayer_HandR* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void Free() override;
};

