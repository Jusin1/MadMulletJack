#pragma once
#include "CCamera.h"
class CCameraFPS : public CCamera
{
public:
    enum FPSCAMMODE { CAM_NORMAL, CAM_LEFT, CAM_RIGHT, CAM_ZOOM, CAM_END };
private:
    explicit CCameraFPS(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CCameraFPS(const CCameraFPS& rhs);
    virtual ~CCameraFPS();

public:
    virtual HRESULT Ready_GameObject();
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int    Update_GameObject(const _float& fTimeDelta);
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta);
    virtual void    Render_GameObject() {}

    //getter stter func
public:
    _bool Get_Fix() { return m_bFix; }
    void Set_Fix(_bool _bFix) { m_bFix = _bFix; }

    _bool Get_Shaking() { return m_bShaking; }
    void Set_Shaking(_bool _bShaking) { m_bShaking = _bShaking; }

    _bool Get_Recoil() { return m_bRecoil; }
    void Set_Recoil(_bool _bRecoil) { m_bRecoil = _bRecoil; }

    _bool Get_Zoom() { return m_bZoom; }
    void Set_Zoom(_bool _bZoom) { m_bZoom = _bZoom; }

    FPSCAMMODE Get_FPSCAMMODE() { return m_eCamMode; }
    void Set_FPSCAMMODE(FPSCAMMODE _eCammode) { m_eCamMode = _eCammode; }

private:
    HRESULT Set_PlayerPos();
    void    Move_Shaking();
    void    Mouse_Move(); //바라보는 방향
    void    Mouse_Fix(); // 마우스 화면 안에서 움직임

    // 줌 시스템
    void    TickZoom(const _float dt);
    _bool   IsZoomWanted() const;     // 플레이어 상태로 줌 의도 판단

private:
    _bool   m_bFix;
    _bool   m_bShaking;   // 플레이어 좌우 움직일때 쉐이킹
    _bool   m_bRecoil;    // 총 반동을 위함 -> 뺄지도
    _bool   m_bZoom;
    FPSCAMMODE m_eCamMode;
    _float  m_fOffset;   

    _float  m_fZoomTime;  

    _float     m_fDefaultFov;
    _float     m_fZoomFov_Default;
    _float     m_fZoomFov_Sniper;

    _float     m_fCurFov;

    _float     m_fZoomInSpeed;
    _float     m_fZoomOutSpeed;

    _float     m_fTargetOffset;
    _float     m_fCurOffset;
    _float     m_fZoomOffset_Default;
    _float     m_fZoomOffset_Sniper;

public:
    static CCameraFPS* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CCamera* Clone(void* pArg);
    virtual void       Free();
};
