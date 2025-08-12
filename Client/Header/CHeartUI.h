#pragma once
#include "CUI.h"
class CHeartUI :
    public CUI
{
private:
    explicit CHeartUI(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CHeartUI(const CHeartUI& rhs);
    virtual ~CHeartUI();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int Update_GameObject(const _float& fTimeDelta) override;
    virtual void LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void Render_GameObject() override;

public:
    HRESULT Change_Texture(const _tchar* pTextureTag);

private:
    HRESULT Texture_Clone();


private:
    map<const _tchar*, CTexture*> m_mapTextures; // 애니메이션 텍스쳐
    wstring m_CurrentAnimTag; // 현재 애니메이션 태그
    
public:
    static CHeartUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void Free() override;
};

