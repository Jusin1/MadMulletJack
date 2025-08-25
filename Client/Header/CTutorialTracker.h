#pragma once

// 진행 단계
enum class TStep : int {
    Move = 0, Fire, Jump, Dash, Finish, Door, Soda,
    Count
};

class CTutorialTracker {
public:
    static CTutorialTracker& Get() { static CTutorialTracker s; return s; }

    bool  IsDone(TStep s) const { return (m_flags & (1u << (int)s)) != 0; }
    bool  HasPending()    const { return FirstPendingIndex() >= 0; }
    int   FirstPendingIndex() const {
        for (int i = 0; i < (int)TStep::Count; ++i)
            if ((m_flags & (1u << i)) == 0) return i;
        return -1;
    }
    
    // 튜토리얼 알림
    void Notify_Move() { setDone(TStep::Move); }
    void Notify_Fire() { setDone(TStep::Fire); }
    void Notify_Jump() { setDone(TStep::Jump); }
    void Notify_Dash() { setDone(TStep::Dash); }
    void Notify_Door() { setDone(TStep::Door); }
    void Notify_Finish() { setDone(TStep::Finish); }
    void Notify_Soda() { setDone(TStep::Soda); }

    void ResetRuntime() { m_flags = 0; } 

private:
    CTutorialTracker() = default;
    ~CTutorialTracker() = default;
    CTutorialTracker(const CTutorialTracker&) = delete;
    CTutorialTracker& operator=(const CTutorialTracker&) = delete;

    void setDone(TStep s) { m_flags |= (1u << (int)s); }

private:
    unsigned m_flags = 0; 
};
