#pragma once
#include "Engine_Define.h"
#include "CBase.h"
#include "CUI.h"

class CLisaUI;
class CTextUI;
class CBlackGackGround;
class CPanelUI;
class CPhoneUI;
class CImageUI;
// UI 생성 및 관리
class CUIManager :
    public CBase
{
    DECLARE_SINGLETON(CUIManager)

private:
    explicit CUIManager();
    virtual ~CUIManager();

public:
    void CreateClearUI();         // 진입 UI 생성
    void CreateClearTextUI();
    void CreateTimeTextUI(const std::wstring& timeStr);
    void CreatePhoneUI();
    void CreatePhoneScreen();

    void DestroyEnterUI();        // 종료 연출
    bool IsEnterUIBusy() const { return (m_pEnterUI != nullptr) || m_exitingEnter; } //UI가 동작 중인지 확인

    void Update(const _float& dt);  

    // 애니메이션(슬라이드 효과)
    void AddSlideIn(CUI* ui,
        float x, float yTarget, float w, float h,
        float offsetX, float offsetY, float delay, float dur); // 슬라이드 인

    void AddSlideInY(CUI* ui,
        float x, float yTarget, float w, float h,
        float offsetY, float delay, float dur) // 슬라이드 인(Y축 전용)
    {
        AddSlideIn(ui, x, yTarget, w, h, 0.f, offsetY, delay, dur);
    }

    void AddSlideTo(CUI* ui,
        float xEnd, float yEnd, float delay, float dur); // 임의 위치로 슬라이드 이동

    void SliderPhoneUI();

public:
    virtual void Free() override; 

private:
    void CancelSlidesForSubtree(CUIBase* root); // 특정 UI 트리의 슬라이드 취소
    // 슬라이드 애니메이션 데이터 구조체
    struct SlideTask {
        CUI* ui = nullptr;   // 대상 UI
        float  w = 0.f, h = 0.f;
        float  xStart = 0.f, xEnd = 0.f;
        float  yStart = 0.f, yEnd = 0.f;
        float  elapsed = 0.f;  // 진행된 시간
        float  delay = 0.f;    // 시작 지연
        float  dur = 0.f;      // 지속 시간
        bool   done = false;   // 완료 여부
    };

private:
    CUIBase* m_pEnterUI = nullptr;         
    CUIBase* m_pMonsterDieEffect = nullptr;// 몬스터 사망 이펙트 UI


private:
    CTextUI* m_pVictoryText = nullptr; // 승리 TextUI
    CTextUI* m_pFloorTimeText = nullptr; // Floor TextUI
    bool     m_spawnedTimeUI = false; // TimeUI 스폰여부

    CBlackGackGround* m_pTimeBlack = nullptr; // 배경
    CPanelUI* m_pTimeFrame = nullptr; // timeFrame
    CTextUI* m_pTimeText = nullptr; // timeText
    CLisaUI* m_pLisaUI = nullptr;


    // 핸드폰 관련 UI
    CPhoneUI* m_pPhone = nullptr;
    CImageUI* m_pLeftHand = nullptr;
    CImageUI* m_pRightHand = nullptr;
    CImageUI* m_pPhoneScreen = nullptr;

    bool   m_timeAutoRemoveArmed = false; // 삭제 타이머 작동 중인지
    float  m_timeAutoRemoveTimer = 0.f;   // 경과 시간


    vector<SlideTask> m_slideTasks;        // 현재 진행 중인 슬라이드 작업 목록
    bool m_exitingEnter = false;           // UI 종료 연출 중 여부
};

