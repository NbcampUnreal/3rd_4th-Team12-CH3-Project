# 2084:MindHack

Uneral Engine 5를 활용하여 개발한 **사이버펑크 슈팅 게임**입니다.  
<img width="1280" height="720" alt="LoadingMainMenu" src="https://github.com/user-attachments/assets/b2496641-8652-4e9a-89e1-477ceee6e4e4" />


---

## 📑 목차 (Table of Contents)  
1. [프로젝트 소개 (Project Introduction)](#-프로젝트-소개-project-introduction)
2. [개발 환경 (Development Environment)](#-개발-환경-development-environment)
3. [시연 영상 (Demo Video)](#-시연-영상-demo-video)  
4. [설치 및 실행 방법 (Installation & Run)](#-설치-및-실행방법-Installation--Run)
5. [팀원 및 역할 (Team & Roles)](#-팀원-및-역할-Team--Roles)
6. [주요 기능 (Feature)](#-주요-기능-Feature)
8. [트러블슈팅 (Troubleshooting)](#-트러블슈팅-Troubleshooting)
9. [Source](#-Source)

---

## 🎮 프로젝트 소개 (Project Introduction)  

* **게임 이름** : 2084 MindHack
* **장르/세계관/목표** : 슈팅게임/사이버펑크/보스처치
* **주요 특징** : 싱글플레이, 보스전, 스토리, 엔딩크레딧
<img width="2393" height="1336" alt="image" src="https://github.com/user-attachments/assets/d5247179-b86e-4401-a47a-cb896c54c81b" />
<img width="2383" height="1326" alt="image" src="https://github.com/user-attachments/assets/5541381c-b674-44d1-9abc-e83446cd3352" />
<img width="2404" height="1337" alt="image" src="https://github.com/user-attachments/assets/008d6e5e-a130-4a0c-adcf-ec52fb05c048" />
<img width="2297" height="1324" alt="image" src="https://github.com/user-attachments/assets/d18b3a73-51c7-4b73-95d6-5dbf1c74453a" />

---

## 🛠 개발 환경 (Development Environment)  

* **게임엔진** : Unreal Engine 5.5.4
* **개발언어** : C++ / Blueprint
* **IDE** : Rider 2025.1.4
* **타겟 플랫폼** : Windows
* **협업 툴** : GitHub LFS / [Notion](https://www.notion.so/teamsparta/12-21b2dc3ef5148025973ad0e8eed1f1e2) / Slack / Google Drive / Canva / Zep

---

## 📽 시연 영상 (Demo Video)  

* **Demo vedeio**(이미지 클릭) <br>
  [![Demo Vedio](https://img.youtube.com/vi/mScI6zura8E/0.jpg)](https://youtu.be/mScI6zura8E?si=rWdxcrzzRXNpKgBg)

* **Ending cinematic**(이미지 클릭) <br>
  [![Ending Cinematic](https://img.youtube.com/vi/Rgv1000JS2A/0.jpg)](https://youtu.be/Rgv1000JS2A)

* [Presentation](https://www.canva.com/design/DAGwZt0LrCc/GL8y4SU1SyphVAYU6WoJKA/view?utm_content=DAGwZt0LrCc&utm_campaign=designshare&utm_medium=link2&utm_source=uniquelinks&utlId=hf214a0aa36)

---

## 🏗 설치 및 실행 방법 (Installation & Run)  

* [실행파일 다운로드(5GB) : Google Drive](https://drive.google.com/file/d/1Nge0wU42NFZZTgqNaB6WhaE4wZeQj7Mm/view)
* 압축 해제 후, ST.exe 실행 <br>
  <img width="278" height="286" alt="image" src="https://github.com/user-attachments/assets/63939b58-784a-4a7d-8284-ce8e4c020941" />

---

## 👨‍💻 팀원 및 역할 (Team & Roles)  

| 이름   | 역할 (Role)       | 담당 파트 | Blog | Github
|--------|------------------|-----------|-------|-------
| 장주만 | Game System, Packaging    | 게임의 핵심 시스템 및 전반적인 구조 설계 | [NaverBlog](https://blog.naver.com/letterb926) | [Github](https://github.com/JangJuMan/)
| 박준범 | Player           | 플레이어 캐릭터의 움직임, 상호작용 및 기능 구현 | [Tistory](https://pjb990.tistory.com/) | [Github](https://github.com/Parkjunbeom99) |
| 안지호 | Weapon           | 게임 내 무기 시스템, 발사 및 재장전 메커니즘 개발 | [Tistory](https://zksnwstarcraft.tistory.com/) | [Github](https://github.com/anyway3) |
| 주상현 | User Interface   | 메뉴, HUD 등 사용자 인터페이스(UI) 디자인 및 구현 | [Tistory](https://shju9803.tistory.com/) | [Github](https://github.com/SangHyunJu9803) |
| 이서영 | Level Design     | 게임 스테이지, 배경 및 오브젝트 배치 등 레벨 디자인 | [Velog](https://velog.io/@qsdhkd12/posts) | [Github](https://github.com/Ep1bite) |
| 홍승조 | Enemy            | 적 캐릭터의 AI, 행동 패턴 및 로직 구현 | [Velog](https://velog.io/@hydisk77/posts) | [Github](https://github.com/hongjo77) |

---

## 🏗 주요 기능 (Feature)

### Game System
* GameMode/ GameState/ GameInstance/ PlayerState : 데이터의 범위, 수명이 다르고 유지보수를 위해 역할을 구분
* Game Narrative : 플레이어의 몰입과 동기부여 제공(Loading Screen, Ending Cinematic)
* Project Packaging : Cooking + Shipping Build를 통해 실행 가능한 형태로 포장(.exe)

### Level Design
* Stage 1 ~ 3으로 레벨을 구성/디자인
<img width="1737" height="977" alt="image" src="https://github.com/user-attachments/assets/848573be-882e-44a1-81c7-374362063c45" />

### UI
* 컨트롤러 단위의 위젯 컨트롤
* 모듈화 : 상황에 따른 HUD를 전용 위젯 모듈에서 관리

### Player Character
* 1인칭(FPS), 3인칭(TPS) 모두 지원 : TPS, FPS 메시 분리구조, 독립 애니메이션 적용
* 모듈화 : 캐릭터의 기능을 Component로 분리 (Health, Movement, Weapon, AnimInstance)
* 데이터 기반 설계 : 하드코딩(X), 데이터 에셋, 데이터 테이블을 통한 값 참조

### Weapon
* 클래스 다이어그램
<img width="990" height="473" alt="image" src="https://github.com/user-attachments/assets/be86d6f0-ac1d-4171-8b51-43ed70e21fbb" />

* HitScan(라인 트레이스), Projectile(투사체) 방식의 무기 구현
* 데이터 기반 설계 : 데이터 에셋을 활용하여 무기 세부사항 조정
* **ASTWeaponBase**: 연사 속도, 재장전, 탄약 관리, 카메라 흔들림, 사운드/이펙트 등 무기의 공통 기능 담당
* **FireWeapon**: 실제 발사 방식은 각 자식 클래스에서 구체화
*  **ASTWeapon_Hitscan**: 화면 중앙을 기준으로 라인 트레이스를 사용해 즉시 피격 판정
*  **ASTWeapon_Projectile**: 투사체(Projectile) 액터를 생성하여 발사, 이동 후 충돌 시 피해 적용
*  **USTWeaponDataAsset**: 데미지, 연사력, 장탄수 등 무기 스펙을 데이터로 분리 관리
*  **FSTWeaponData**: 무기 스펙(데미지, 연사력, 장탄수 등)을 담는 구조체  

### Enemy AI
* AI 상태 시스템 : Behavior Tree, State Component, AnimInstance를 통한 상태변화 처리
* AI 인지 게이지 : 시각, 청각을 활용한 인지 게이지 알고리즘 적용
<img width="1748" height="655" alt="image" src="https://github.com/user-attachments/assets/7b348116-7470-4999-a9ac-709a603de177" />

* 데이터 기반 보스 스킬 : 코드 수정 없이 데이터만으로 보스 스킬 제작 가능(스킬 순서 조합)

---

## 🚑 트러블슈팅 (Troubleshooting)  

### 1. FTimer & Destroy  
- **문제 상황**  
  - Launcher 투사체가 5초 후 사운드를 정지하고 Destroy 되도록 구현.  
  - 하지만 객체가 먼저 Destroy될 경우 `StopSound()` 접근 불가 → 사운드가 중첩되는 문제 발생.  
- **원인**  
  - `SetTimer` 콜백이 객체 파괴 이후에도 실행됨.  
  - 이미 Destroy된 객체에 접근하면서 Crash 또는 Access Violation 가능.  
- **해결 방법**  
  - 타이머 해제를 적절히 수행.  
  - `IsValid()` 체크 및 `TWeakObjectPtr` 활용.  
  - 사운드 정지 후 안전하게 객체 파괴.  

### 2. 캐릭터 트러블슈팅  
- **문제 상황**  
  - 무기 스폰 시 `Sync Load`로 인해 끊김 발생.  
  - 무기 Pivot 차이로 캐릭터 손 위치에 제대로 부착되지 않음.  
- **해결 방법**  
  - `StreamableManager`를 활용하여 `Async Load`로 스폰 처리.  
  - 무기별 Position/Rotation 보정을 `DataTable`에 저장.  
  - 장착 시 DataTable 데이터를 참조하여 정확한 부착 구현.  

### 3. Level Design  
- **문제 상황**  
  - Nav Mesh Bounds가 비어 있는 공간에서는 적이 플레이어를 인식하지 못함.  
- **해결 방법**  
  - Nav Mesh Bounds 크기를 통일.  
  - 웨이포인트 기반의 보조 탐색 시스템 도입.  

### 4. 캐릭터 관리 방식  
- **문제 상황**  
  - 레벨전환시마다 캐릭터의 생성/파괴 반복 -> 성능낭비
- **해결 방법**  
  - Game Instance에 캐릭터를 보관 -> skeletal mesh 만 교체하여 캐릭터 교체
  - GC 영향을 덜 받는 구조 설계

### 5. Serialization Failed  
- **문제 상황**  
  - Save 기능 중, 내용이 저장되지 않는 문제 발생
  - UPROPERTY() 누락으로 리플렉션 등록 X -> Serialization 미지원으로 데이터 보존 실패
  - (데이터는 메모리상 유지되지만 실제 저장되지 않음)
- **해결 방법**  
  - 클래스 멤버변수에 UPROPERTY()를 붙여 리플렉션 등록
  - Serialization 적용하여 데이터 정상 유지 / 리플렉션 누락 주의

### 6. Shipping Build  
- **문제 상황**  
  - Development 환경에서는 GC, 초기화 순서 등이 느슨하게 처리되어 우연히 성공하던 기능이 Shipping 빌드에서는 작동하지 않음
  - ex) 생성이 완료되기 전에 바인딩하여 바인딩 실패 현상 발생
- **해결 방법**  
  - Shipping 빌드에서도 안정적으로 동작하는 코드 작성(우연한 성공은 최적화/타이밍 차이로 재현되지 않음)
  - 초기화, 생성 순서에 맞는 정확한 프로그래밍 하기

---

## 🚑 Source
* 캐릭터 : https://www.fab.com/ko/listings/e560a4c9-fab0-4e0d-9482-2e5b4006c69a
* Stage1 : https://cosmos.leartesstudios.com/environments/cyberpunk-kyiv
* Stage2 : https://cosmos.leartesstudios.com/environments/stylized-cyberpunk-street
* Stage3 : https://www.fab.com/ko/listings/e55bb035-8720-487f-8a36-4ce50d16f344
* 무기(라이플) : https://cosmos.leartesstudios.com/assets/cs1-krusher
* 무기(샷건) : https://cosmos.leartesstudios.com/assets/cs-1-ts12
* 무기(스나이퍼) : https://cosmos.leartesstudios.com/assets/cs-1ballista
* 무기(중력포) : https://www.fab.com/ko/listings/a054bcf9-167c-43aa-a89e-c704387f057d
* 적(근거리) : https://www.fab.com/ko/listings/ec8f2cb8-f904-4473-902f-67ade18bd225
* 적(원거리) : https://www.fab.com/ko/listings/17e0840f-8651-4933-bd6d-211a34b1dd17
* 보스 : https://www.fab.com/ko/listings/c23ee3a7-4a73-4a83-9061-30b682d269f8
* Plugin : Async Loading Screen (로딩화면)

--- 
