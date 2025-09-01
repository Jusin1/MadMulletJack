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
class CButtonUI;
class CTalkUI;
// UI 생성 및 관리
class CUIManager :
    public CBase
{
#pragma region 카드
    enum class UpgradeId { SlowMo, FIRE, SHOTGUN, HEADSHOT,};

    struct ShopItemDef {
        UpgradeId           id;
        std::wstring        backTag;
        std::wstring        backProto;
        std::wstring        artTag;
        std::wstring        artProto;

        // ── 아이콘 옵션 ──
        float               iconW = 140.f;
        float               iconH = 50.f;
        float               iconYOffset = -30.f;
    };

    struct ShopCardUI {
        CImageUI* pBack = nullptr;   // 배경 이미지
        CButtonUI* btn = nullptr;     // 버튼 (클릭 영역)
        CImageUI* icon = nullptr;    // 아이콘
        bool        bought = false;    // 구매 여부
        UpgradeId   id{};
    };

    // 씬별로 어떤 카드들이 등장하는지 지정
    std::unordered_map<int, std::vector<UpgradeId>> gSceneShopCards = {
        { SCENE_DEV, { UpgradeId::SlowMo, UpgradeId::FIRE, UpgradeId::SHOTGUN } },
        { SCENE_TUTORIAL, { UpgradeId::SHOTGUN, UpgradeId::SlowMo, UpgradeId::HEADSHOT} },
        { SCENE_STAGE_2,   { UpgradeId::FIRE, UpgradeId::SHOTGUN, UpgradeId::HEADSHOT } },
        { SCENE_SNIPE,   { UpgradeId::FIRE, UpgradeId::SHOTGUN } },
    };

    // ── 카드 데이터 풀 ──
    inline static const std::vector<ShopItemDef> kShopPool = {
        { UpgradeId::SlowMo,
          L"Com_Tex_BackGround", L"Prototype_Component_Texture_Monster_Bullet_Slow_Back",
          L"Com_Tex_SlowMoArt",  L"Prototype_Component_Texture_Monster_Bullet_Slow_Bullet",
          160.f, 210.f, 50.f },

        { UpgradeId::FIRE,
          L"Com_Tex_BackGround", L"Prototype_Component_Texture_Monster_Bullet_Slow_Back",
          L"Com_Tex_FireArt",    L"Prototype_Component_Texture_Fire_Ex",
          160.f, 210.f, 50.f },

        { UpgradeId::SHOTGUN,
          L"Com_Tex_BackGround", L"NONE",
          L"Com_Tex_SniperArt",  L"Prototype_Component_Texture_Sniper_Art",
          160.f, 280.f, 10.f },

        { UpgradeId::HEADSHOT,
          L"Com_Tex_BackGround", L"Prototype_Component_Texture_Monster_Bullet_Slow_Back",
          L"Com_Tex_SniperArt",  L"Prototype_Component_Texture_HeadSHOT",
          160.f, 210.f, 50.f }


    };

    static const ShopItemDef* FindShopDef(UpgradeId id) {
        for (auto& d : kShopPool)
            if (d.id == id) return &d;
        return nullptr;
    }

#pragma endregion 카드


    DECLARE_SINGLETON(CUIManager)

private:
    explicit CUIManager();
    virtual ~CUIManager();

public:
    void CreateClearUI();
    void CreateClearTextUI();
    void CreateTimeTextUI(const std::wstring& timeStr);
    void CreatePhoneUI();
    void CreatePhoneScreen();

    // 작은 이펙트 생성
    void CreateEffectUI(const std::wstring& str);
    void CreateItemUI();
    void CreateReloadUI();

    void DestroyEnterUI();
    void DestroyItemUI();
    void DestroyEffectUI();
    void DestroyReloadUI();
    bool IsEnterUIBusy() const { return (m_pEnterUI != nullptr) || m_exitingEnter; }

    // player effect
    void Create_PlayerEff(PLAYEREFF _eEffect); // dash, blood, cure(함수 호출로?)
    void Create_CureEff();

    void Destory_PlayerEff(PLAYEREFF _eEffect);
    void Destory_CureEff();
    void Destory_PlayerEff_ALL();

    void Update(const _float& dt);

    // Slide
    void AddSlideIn(CUI* ui,
        float x, float yTarget, float w, float h,
        float offsetX, float offsetY, float delay, float dur);

    void AddSlideInY(CUI* ui,
        float x, float yTarget, float w, float h,
        float offsetY, float delay, float dur)
    {
        AddSlideIn(ui, x, yTarget, w, h, 0.f, offsetY, delay, dur);
    }

    void AddSlideTo(CUI* ui,
        float xEnd, float yEnd, float delay, float dur);


    void AddScaleIn(CUI* ui,
        float xStart, float yStart, float wStart, float hStart,
        float xEnd, float yEnd, float wEnd, float hEnd,
        float delay, float dur);

    // Phone sequence
    void SliderPhoneUI();      // 아래에서 위로 슬라이드 인
    void StartPhonePullAnim(); // 중앙으로 확대(당겨오기)

private:
    // 폰 당김 애니메이션 완료 감지/후처리용
    bool m_phonePullFinished = false;

    // 애니 끝난 뒤에 스크린 텍스처를 바꾸도록 큐잉
    bool m_changeScreenOnPullFinish = false;
    std::wstring m_nextPhoneScreenTexTag;

    bool PhoneScaleDone() const;           // 스케일(당김) 작업 끝났는지
public:
    void ChangePhoneScreenAfterPull(const std::wstring& texTag); // 외부에서 큐잉

public:
    void OpenShop();             // 상점 열기(3장 생성)
    void CloseShop();            // 상점 닫기/정리

private:
    void CreateShopCardAt(int poolIdx, float cx, float cy, ShopCardUI& outCard);
    void OnShopCardClicked(int slot); // 0,1,2 중 하나
    void LayoutShopCard(ShopCardUI& card);
public:
        void ClearAllUI();

public:
    virtual void Free() override;

private:
    bool PhoneSlidesDone() const;   // 슬라이드 완료 체크 

    struct SlideTask {
        CUI* ui = nullptr;
        float w = 0.f, h = 0.f;
        float xStart = 0.f, xEnd = 0.f;
        float yStart = 0.f, yEnd = 0.f;
        float elapsed = 0.f, delay = 0.f, dur = 0.f;
        bool  done = false;
    };

    struct ScaleTask {
        CUI* ui = nullptr;
        float elapsed = 0.f, delay = 0.f, dur = 0.f;
        bool  done = false;
        float xStart, yStart, wStart, hStart;
        float xEnd, yEnd, wEnd, hEnd;
    };

private:
    CUIBase* m_pEnterUI = nullptr;
    CUIBase* m_pFlooroUI = nullptr;
    CUIBase* m_pItemUI = nullptr;
    CUIBase* m_pEffectUI = nullptr;
    CUIBase* m_pReloadUI = nullptr;

    CUIBase* m_pPlayerEffUI = nullptr;

    // Clear text
    CTextUI* m_pVictoryText = nullptr;
    CTextUI* m_pFloorTimeText = nullptr;
    bool     m_spawnedTimeUI = false;
    CBlackGackGround* m_pTimeBlack = nullptr;
    CPanelUI* m_pTimeFrame = nullptr;
    CTextUI* m_pTimeText = nullptr;
    CLisaUI* m_pLisaUI = nullptr;

    // 대화 UI
    CTalkUI* m_pTalkUI = nullptr;

    // Phone parts
    CPhoneUI* m_pPhone = nullptr;
    CImageUI* m_pLeftHand = nullptr;
    CImageUI* m_pRightHand = nullptr;
    CImageUI* m_pPhoneScreen = nullptr;
    CBlackGackGround* m_pPhoeScreenBackGround = nullptr;

    // Timers/flags
    bool  m_timeAutoRemoveArmed = false;
    float m_timeAutoRemoveTimer = 0.f;

    bool m_bRemoveUI = false;
    float m_timeUIRemoveTimer = 0.f;

    bool  m_phoneSlideActive = false; // 슬라이드가 실제 시작되었는가?
    bool  m_phonePullArmed = false;   // 완료 후 딜레이 카운트 중?
    bool  m_phonePullStarted = false; // 중앙 확대 시작했는가(1회용)
    float m_phonePullTimer = 0.f;
    float m_phonePullDelay = 0.6f;    // 슬라이드 종료 뒤 대기시간

    // 딜레이 후 PhoneScreen 생성 타이머
    bool  m_createPhoneScreenPending = false;
    float m_createPhoneScreenTimer = 0.f;
    float m_createPhoneScreenDelay = 0.2f; 

    std::vector<SlideTask> m_slideTasks;
    std::vector<ScaleTask> m_scaleTasks;

    // 상점 관련 UI
    CUIBase* m_pShopRoot = nullptr;
    std::vector<ShopCardUI> m_shopCards;
    std::vector<int>        m_shopIndices;
    bool    m_shopOpen = false;

    bool m_exitingEnter = false;
};

