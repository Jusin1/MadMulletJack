#pragma once
#include "CUI.h"

class CEffectUI :
    public CUI
{
private:
    explicit CEffectUI(LPDIRECT3DDEVICE9 pGraphicDev);  
    explicit CEffectUI(const CEffectUI& rhs);            
    virtual ~CEffectUI();                             

public:
    virtual HRESULT Ready_GameObject() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int    Update_GameObject(const _float& fTimeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void    Render_GameObject() override;

public:
    // 몬스터 위에서 생성되는 UI
    void Show(const wchar_t* text,
        const wchar_t* texTag,
        float seconds,
        float centerX = WINCX * 0.5f,   // 기본 중앙 X
        float centerY = WINCY * 0.33f,  // 기본 중앙 Y
        float angleDeg = 0.f,
        float bgAlpha = 0.85f,
        const wchar_t* fontTag = L"DefaultFont",
        D3DXCOLOR fontColor = D3DXCOLOR(1, 1, 1, 1));


    // 좌측에 생성되는 배너 이펙트
    void ShowBanner(const wchar_t* text,
        float seconds,
        float x, float y,
        float scaleStart = 1.80f,
        float scaleEnd = 1.00f,
        const wchar_t* fontTag = L"DefaultFont",
        D3DXCOLOR fontColor = D3DXCOLOR(1, 1, 1, 1),
        float bgAlpha = 0.85f,
        float angleDeg = -10.f);

    // UI 숨김
    void Hide();

    // ===== 옵션 설정 함수들 =====
    void SetImageSize(float w, float h) { m_iconW = w; m_iconH = h; }     // 아이콘 크기
    void SetImageOffset(float ox, float oy) { m_iconOff.x = ox; m_iconOff.y = oy; } // 아이콘 오프셋
    void SetBoxSize(float w, float h = 30.f) { m_boxW = w; m_boxH = h; }  // 배경 박스 크기

    // 플로팅 텍스트 전용 옵션
    void SetMoveSpeed(float pxPerSec, bool computeLifeBySpeed = false) { m_moveSpeed = pxPerSec; m_useSpeed = computeLifeBySpeed; }
    void SetTargetBounds(float leftX, float rightX) { m_targetLeftX = leftX; m_targetRightX = rightX; }

    // 배너 전용 옵션
    void SetBannerExtraWidth(float w) { m_bannerExtraW = w; }   // 텍스트 폭 + 추가 여유폭
    void SetBannerAngle(float deg) { m_bannerAngleDeg = deg; }  // 배너 기울기 각도

    // 텍스처 변경
    HRESULT Change_Texture(const _tchar* tag);

    // 메인 UI 객체 반환 (싱글턴식 접근)
    static CEffectUI* GetMain() { return s_pMain; }

private:
    // 모드: 플로팅 or 배너
    enum MODE { FLOAT_MOVE, BANNER_FIXED };
    MODE   m_mode;

    // 상태 및 타이밍
    bool   m_bVisible;      // 보이는 상태 여부
    float  m_time;          // UI가 살아있는 총 경과 시간
    float  m_totalLife;     // UI 전체 생명 주기
    float  m_fRemain;       // 남은 시간

    // 현재 위치
    float  m_cx, m_cy;
    // 시작 / 종료 좌표
    float  m_startX, m_startY;
    float  m_endX, m_endY;

    // 진행률 (0~1)
    float  m_progress;
    // 글자 스케일 애니메이션
    float  m_scaleStart;
    float  m_scaleEnd;

    int    m_dirX;        
    float  m_liftMin, m_liftMax;
    float  m_targetLeftX, m_targetRightX; 
    float  m_moveSpeed;   
    bool   m_useSpeed;    


    float  m_bgAlpha;    


    float  m_boxW, m_boxH;
    float  m_padL, m_gap;

    wstring m_Text;
    wstring m_FontTag;
    D3DXCOLOR    m_FontColor;

    // 아이콘 정보
    float  m_iconW, m_iconH;
    _vec2  m_iconOff;
    bool   m_hasIcon;

    // 페이드/깜빡임
    float  m_fadeOutDur;
    float  m_blinkSpeed;

    // 배너 관련
    float  m_bannerExtraW;
    float  m_bannerAngleDeg;

    // 내부 컴포넌트
    VIBuffer_Color* m_pBgBufferCom;                   // 배경 박스 버퍼
    std::map<const _tchar*, CTexture*> m_mapTextures; // 사용 가능한 텍스처들

    // 전역 접근용 메인 UI
    static CEffectUI* s_pMain;

private:
    // 텍스처 미리 로드
    HRESULT PreloadTexture(const _tchar* mapTag, const _tchar* protoTag);

    // 내부 렌더링 함수
    void RenderBox(float cx, float cy, float w, float h, float alpha, float angleDeg);
    void RenderIcon(float cx, float cy, float aMul, float scale);
    void RenderText(float leftX, float centerY, float aMul);

    // Ease 함수 (부드러운 애니메이션 보간)
    static float EaseExpoOut(float t) {
        return (t >= 1.f) ? 1.f : (t <= 0.f) ? 0.f : (1.f - powf(2.f, -10.f * t));
    }

public:
    // 객체 생성 / 복제 / 해제
    static CEffectUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void        Free() override;
};

