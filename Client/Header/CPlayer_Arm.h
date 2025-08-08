#pragma once
#include "CUI.h"
class CPlayer_Arm :public CUI
{
private:
    explicit CPlayer_Arm(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CPlayer_Arm(const CPlayer_Arm& rhs);
    virtual ~CPlayer_Arm();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int Update_GameObject(const _float& fTimeDelta) override;
    virtual void LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void Render_GameObject() override;

    

public:
    HRESULT Change_Texture(const _tchar* pTextureTag);

private:
    virtual HRESULT Set_Texture() override;
    HRESULT Texture_Clone();

private:
    map<const _tchar*, CTexture*> m_mapTextures;    // 애니메이션 텍스쳐
    wstring m_CurrentAnimTag;                       // 현재 애니메이션 태그

private:
    PlayerStateInfo m_tInfo;

    _bool m_bChange; // 기믹.. -> 사실 texture 이름 가져오면 되긴함

public:
    static CPlayer_Arm* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void Free() override;
};
