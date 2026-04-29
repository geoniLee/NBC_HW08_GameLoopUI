# NBC_HW08_GameLoopUI

## 1. 멀티 웨이브 / 레벨 시스템

- `GameState` 기반 Wave 진행 관리
    - `StartWave()`, `EndWave()` 구조
    - `currentWave`, `MaxWave` 관리
- 웨이브별 난이도 변화
    - 아이템 스폰 개수 증가
    - 제한 시간 증가 (`LevelDuration * Wave`)
- 웨이브 시작 시
    - 디버그 메시지 출력
    - UI 애니메이션 (`StartWaveAnim`) 실행

- 웨이브 종료 시 기존 아이템 제거 로직 추가

---

## 2. UI / HUD 시스템

### 기본 UI 구성
- HUD 위젯 (`WBP_HUDWidget`)
    - 시간 / 점수 / 레벨 / 웨이브 / HP 표시 / 디버프
- 메인 메뉴 / 게임 오버 UI 분리
---
- `PlayerController`에서 HUD 생성 및 관리
- `UpdateHUD()`를 통한 실시간 UI 갱신
- 타이머 색상 변화 (5초 이하 시 빨간색)
- 디버프 UI 표시 시스템
    - 디버프 발생 시 알림 출력
    - UI 애니메이션 실행 (ProcessEvent)
  
---

## 3. 디버프 시스템

### 공통
- 시간 누적
- UI 아이콘 + 메시지 출력

### Slow
- 이동 속도 감소 (`SlowSpeedMultiplier`)

### ReverseControl
- 입력 방향 반전 (`moveInput *= -1`)

### Blind
- 화면 시야 제한 (Border UI 활성화)

---

## 4. 웨이브 이벤트 시스템

### Wave 2
- 일정 시간마다 랜덤 아이템 생성

### Wave 3
- 랜덤 아이템 생성 후 즉시 발동

---
## 시연 영상

### [유튜브 링크](https://www.youtube.com/watch?v=adbrYpEH6pM)