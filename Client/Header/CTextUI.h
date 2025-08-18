#pragma once
#include "CUI.h"
class CTextUI :
    public CUI
{
protected:
	explicit CTextUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTextUI(const CTextUI& rhs);
	virtual ~CTextUI();

public: // CGameObject 인터페이스
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int    Update_GameObject(const _float& fTimeDelta) override;
	virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual void    Render_GameObject() override;

public: // 텍스트 설정 관련
	void SetText(const std::wstring& ws);          // 텍스트 설정
	void AppendText(const std::wstring& ws);       // 텍스트 이어붙이기
	void SetFontTag(const std::wstring& tag);      // 폰트 종류 설정
	void SetColor(D3DXCOLOR c);                    // 색상 설정
	void SetScale(float s);                        // 크기 배율 설정
	void SetRotation(float deg);                   // 회전 각도 설정
	void SetCentered(bool on);                     // 중앙 정렬 여부
	void SetLetterSpacing(float px);               // 글자 간격 설정
	void SetAutoSize(bool on);                     // 텍스트 크기 자동 맞춤
	void FitToText();                              // 텍스트에 맞게 크기 조정

public:
	void PlayAppear(float duration = 0.6f, float startMul = 0.8f, float overMul = 1.25f); // Text 등장 애니메이션
public:
	bool IsAppearFinished() const { return !m_appearPlaying; }
	float GetAppearProgress01() const { return min(1.f, m_appearT / max(0.0001f, m_appearDur)); }
private:
	float CurrentRenderScale() const;

private: // 텍스트 관련 멤버 변수
	std::wstring m_text;         // 출력할 텍스트
	std::wstring m_fontTag;      // 폰트 태그
	D3DXCOLOR    m_color;        // 텍스트 색상

	float m_scale;               // 크기 배율
	float m_angle;               // 회전 각도
	bool  m_centered;            // 중앙 정렬 여부

	float m_letterSpacing;       // 글자 간격(px)
	bool  m_autoSize;            // 자동 크기 맞춤 여부
	bool  m_dirtyMeasure;        // 사이즈 재계산 필요 여부

private:
	// 등장 애니메이션 상태
	bool  m_appearPlaying = false;
	float m_appearT = 0.f;        // 경과시간
	float m_appearDur = 0.6f;     // 총시간
	float m_appearStart = 0.8f;   // 시작 배율(상대)
	float m_appearOver = 1.25f;  // 오버슈트 배율(상대)

public: // 생성/복제/해제
	static CTextUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

