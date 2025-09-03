#pragma once
#include "CImageUI.h"
class CBoss;
class CBossHpBar :
    public CImageUI
{
public:
    explicit CBossHpBar(LPDIRECT3DDEVICE9 dev);
    explicit CBossHpBar(const CBossHpBar& rhs);
    virtual ~CBossHpBar();

public:
    HRESULT Ready_GameObject() override;
    HRESULT Initialize(void* pArg) override;
    _int    Update_GameObject(const _float& dt) override;
    void    LateUpdate_GameObject(const _float& dt) override;
    void    Render_GameObject() override;
    void    Free() override;

public:
    static CBossHpBar* Create(LPDIRECT3DDEVICE9 dev);
    CGameObject* Clone(void* pArg = nullptr) override;

    void BindBoss(CBoss* pBoss); // 보스와 연결
    void SetMaxHp(float hp) { m_fMaxHp = hp; }
    void SetIconTexture(const _tchar* tag);
    void SetTextTexture(const _tchar* tag);

private:
    void RenderHpBar(); // 흰색 직사각형 출력

private:
    CBoss* m_pBoss = nullptr;
    float  m_fMaxHp = 100.f;

    // 아이콘 & 텍스트
    CImageUI* m_pIcon = nullptr;
    CImageUI* m_pText = nullptr;

    // HP 바 위치/크기
    float m_fBarX = 200.f;
    float m_fBarY = 50.f;
    float m_fBarW = 400.f;
    float m_fBarH = 24.f;
};

