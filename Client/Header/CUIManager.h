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
    void CreateEnterUI();
    void Update(const _float& dt);

    // 새: X/Y 모두 슬라이드
    void AddSlideIn(CUI* ui,
        float x, float yTarget, float w, float h,
        float offsetX, float offsetY, float delay, float dur);

    // 구버전 호환(세로만). 내부에서 AddSlideIn 호출
    void AddSlideInY(CUI* ui,
        float x, float yTarget, float w, float h,
        float offsetY, float delay, float dur)
    {
        AddSlideIn(ui, x, yTarget, w, h, /*offsetX*/0.f, offsetY, delay, dur);
    }

public:
    virtual void Free() override;

private:
    CUIBase* m_pEnterUI = nullptr;
    CUIBase* m_pMonsterDieEffect = nullptr;

    struct SlideTask {
        CUI* ui;
        float w, h;

        float xStart, xEnd;
        float yStart, yEnd;

        float elapsed, delay, dur;
        bool  done;
    };
    std::vector<SlideTask> m_slideTasks;
};

