#pragma once
#include "Engine_Define.h"
#include "CBase.h"
#include "CUI.h"


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

    CUIBase* m_pEnterUI = nullptr;         // 현재 표시 중인 진입 UI
    CUIBase* m_pMonsterDieEffect = nullptr;// 몬스터 사망 이펙트 UI

    vector<SlideTask> m_slideTasks;        // 현재 진행 중인 슬라이드 작업 목록
    bool m_exitingEnter = false;           // UI 종료 연출 중 여부
};

