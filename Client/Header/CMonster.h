#pragma once

#include "CCharacter.h"
#include "CComponentMgr.h"
#include "Clinet_Define.h"
#include "Client_Global.h"
#include "CPlayer.h"
#include "CManagement.h"
#include "CObjectManager.h"

// 추가예정
// 1. 2개의 Door WorldPosition을 기준(z기준 min, max를 key값)으로 하는 unordered_multimap을 생성하고 해당 반경내에 있는 Monster들 캐싱
// + room내에 있는 PANELDATA(벽 들) 또한 캐싱
// 2. Player WorldPosition기준 캐싱된 unordered_map에 있는 Monster들만 반경 검사
// 3. Player와 Monster 사이에 벽이 있는지 없는지 검사 후 추적 State

namespace {
    inline CPlayer* GetPlayerObj()
    {
        const auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
        if (auto* obj = CObjectManager::GetInstance()->Find_Object(sceneIdx, L"Player_Layer", 0))
            return dynamic_cast<CPlayer*>(obj);
        return nullptr;
    }
}

struct DeathUIConfig { // 몬스터 마다 메시지 교체용 구조체
    const wchar_t* killTextNormal = L"처치";
    const wchar_t* killTextHead = L"헤드샷";
    const wchar_t* rightTextNormal = L"2sec";
    const wchar_t* rightTextHead = L"3sec";
    const wchar_t* iconTexTag = L"Com_Tex_Heal";

    float  followSecsNormal = 2.0f;
    float  followSecsHead = 3.0f;

    // 아래는 고정
    float  bannerBoxW = 360.f, bannerBoxH = 50.f;
    float  bannerX = 175.f, bannerY = 180.f, bannerDownSpeed = 130.f;
    float  labelPopStart = 100.f, labelPopDur = 0.25f;
    float  iconW = 40.f, iconH = 40.f, iconOffX = 30.f, iconOffY = 5.f;
    float  textOffX = 30.f, textOffY = 5.f, textAngleDeg = 0.f;
    float  rightFixedScale = 1.0f;
    const wchar_t* emDigits = L"2345";
    float  emScale = 1.35f;
};


struct HeadSpawnArg {
    const _tchar* texTag = L"Com_Texture_Monster_HEAD";
    const _tchar* protoTag = L"Prototype_Component_Texture_Monster_HEAD";


    int   endFrame = 11;
    float animSpeed = 16.f;
    bool  loop = false;

    // 물리 파라미터
    float fallSpeed = 0.8f;
    float gravity = 3.5f;
    float backDrift = 0.0f;
};

class CMonster : public CCharacter
{
protected:
    explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev, MonsterType _eType);
    explicit CMonster(const CMonster& rhs, MonsterType _eType);
    virtual ~CMonster();

public:
    virtual HRESULT Ready_GameObject();
    virtual HRESULT Initialize(void* pArg);
    virtual _int    Update_GameObject(const _float& fTimeDelta);
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta);
    virtual void    Render_GameObject();

public:
	ObjectCategory GetCategory() const { return m_eCategory; }
	MonsterType GetType() const { return m_eType; }
	_bool IsMoved()
	{
		_bool bReturn;
		static constexpr _float fEpsilon{ 1e-6f };
		const _float fMoved =
			(m_vPosition.x - m_vPrevPosition.x) * (m_vPosition.x - m_vPrevPosition.x) +
			(m_vPosition.y - m_vPrevPosition.y) * (m_vPosition.y - m_vPrevPosition.y) +
			(m_vPosition.z - m_vPrevPosition.z) * (m_vPosition.z - m_vPrevPosition.z);

		bReturn = fMoved > fEpsilon;
		m_vPrevPosition = m_vPosition;
		return bReturn;
	}
protected:
    HRESULT  Set_Component();

protected:
    virtual _bool Picking(_vec3* PickingPoint) override;
    virtual void  PickingTrue() override;
    HRESULT Create_Weapon(_int iRate);

    // 몬스터 죽음 이펙트 관련 함수
    virtual void GetDeathUIConfig(DeathUIConfig& cfg, bool isHeadshot) const; // 몬스터별 문구 변경
    void QueueDeathUI(bool isHeadshot);     // UI 출력 예약
    void TrySpawnDeathUI_Common();          // UI 출력

    void SpawnFollowUI_Common(bool isHeadshot, const DeathUIConfig& cfg); // 몬스터 위에 생성되는 이펙트
    void SpawnBannerUI_Common(bool isHeadshot, const DeathUIConfig& cfg); // 옆에 생성되는 배너
    void SpawnComboUI_Common(); // 콤보 UI

    void DisableAllCollisionAndPicking(); // 몬스터가 죽을 때 픽킹 OFF
    void SetUp_BillBoard();            
    void Set_Collider_With_Wall();

    // 텍스처
    virtual HRESULT Texture_Clone();
    virtual HRESULT Change_Texture(const _tchar* LayerTag);

protected:
    // 컴포넌트
    Engine::CColider_Sphere* m_pColiderCom;
    Engine::CTexture* m_pTextureCom;
    map<const _tchar*, CTexture*> m_mapTexture;

    // 상태
    bool        m_pendingDeathUI;   // UI 출력 대기 여부
    bool        m_lastWasHeadshot;  // 마지막 처치가 헤드샷/급소였는지
    bool        m_bPickable;        // 피킹 가능 여부
    CTransform* m_pPlayerTr;

    // 처치 부위기록
    enum class KillKind : uint8_t { Normal, Head, Balls };
    KillKind    m_lastKillKind;


public:
    ObjectCategory m_eCategory;
    MonsterType    m_eType;
    _float m_fComputeTime{ 0.f };
    _vec3 m_vPrevPosition;

    // 콤보 공통
    static ULONGLONG s_lastKillTimeMs;
    static int       s_comboCount;

    // 유틸
    CTransform* GetPlayerTransform();
    float       DistanceToPlayer() const;

    // 이펙트 생성 함수
    void Spawn_Eletric_Effect(const _vec3& vPos);
    void Spawn_Explosion_Effect(const _vec3& vPos);
    void Spawn_HeadExplosion_Effect(const _vec3& vPos);
    void Spawn_Hit_Effect(const _vec3& vPos);
    void Spawn_Hit_Vent(const _vec3& vPos);

public:
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void Free() override;
};
