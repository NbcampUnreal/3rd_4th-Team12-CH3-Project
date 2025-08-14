#include "Enemy/STEnemyBoss.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Enemy/STEnemySkillComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/STBossAnimInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ASTEnemyBoss::ASTEnemyBoss()
{
    PrimaryActorTick.bCanEverTick = false;
    SkillComponent = CreateDefaultSubobject<USTEnemySkillComponent>(TEXT("SkillComponent"));
}

void ASTEnemyBoss::BeginPlay()
{
    Super::BeginPlay();
    CurrentPhase = EBossPhase::Phase1;

    // 체력 변화 델리게이트 바인딩
    OnEnemyHealthChanged.AddDynamic(this, &ASTEnemyBoss::CheckPhase);

    InitializePhaseSkills();
}

void ASTEnemyBoss::InitializePhaseSkills()
{
    PhaseSkills.Empty();

    // Phase 1 스킬 설정
    TArray<EEnemySkillID> Phase1Skills = {
        EEnemySkillID::BossMelee1,
        EEnemySkillID::BossMelee2,
        EEnemySkillID::BossBomb,
        EEnemySkillID::BossHook,
        EEnemySkillID::BossDashUltimate
    };
    PhaseSkills.Add(EBossPhase::Phase1, Phase1Skills);

    // Phase 2 스킬 설정
    TArray<EEnemySkillID> Phase2Skills = {
        EEnemySkillID::BossMelee1,
        EEnemySkillID::BossMelee2,
        EEnemySkillID::BossFireAttack,
        EEnemySkillID::BossHookUltimate,
        EEnemySkillID::BossDashUltimate,
        EEnemySkillID::BossUpperUltimate
    };
    PhaseSkills.Add(EBossPhase::Phase2, Phase2Skills);
}

TArray<EEnemySkillID> ASTEnemyBoss::GetCurrentPhaseSkills() const
{
    if (const TArray<EEnemySkillID>* SkillArray = PhaseSkills.Find(CurrentPhase))
    {
        return *SkillArray;
    }
    
    return TArray<EEnemySkillID>();
}

void ASTEnemyBoss::Attack()
{
    if (StateComponent->IsInState(EEnemyState::Dead) ||
		StateComponent->IsInState(EEnemyState::PhaseChanging) ||
       !CurrentTarget ||
       !SkillComponent)
    {
        return;
    }

    if (AAIController* AICon = Cast<AAIController>(GetController()))
    {
        if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
        {
            const uint8 SkillByte = BB->GetValueAsEnum(TEXT("EEnemySkillID"));
            const EEnemySkillID Picked = static_cast<EEnemySkillID>(SkillByte);
            
            TArray<EEnemySkillID> AvailableSkills = GetCurrentPhaseSkills();
            if (AvailableSkills.Contains(Picked))
            {
                SkillComponent->TryActivateSkill(Picked, CurrentTarget);
            }
        }
    }
}

void ASTEnemyBoss::CheckPhase(float CurrentHealth, float Max_Health, float HealthPercentage)
{
	if (StateComponent->IsInState(EEnemyState::PhaseChanging) || 
		StateComponent->IsInState(EEnemyState::Dead) ||
		CurrentPhase == EBossPhase::Phase2)
	{
		return;
	}

    if (HealthPercentage <= 50.0f && CurrentPhase == EBossPhase::Phase1)
    {
    	if (SkillComponent)
    	{
    		// 모든 스킬 강제 중단
    		SkillComponent->ForceStopAllSkills();
    		SkillComponent->CleanupActiveParticles();
    	 	
    		// 스킬 실행 상태 초기화
    		if (GetMesh() && GetMesh()->GetAnimInstance())
    		{
    			GetMesh()->GetAnimInstance()->StopAllMontages(0.1f);
    		}
    	}

    	 if (AAIController* AIController = Cast<AAIController>(GetController()))
    	 {
    	 	AIController->StopMovement();
    	 }
    	
    	StateComponent->SetState(EEnemyState::PhaseChanging);
    	CurrentPhase = EBossPhase::Phase2;

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple,
                FString::Printf(TEXT("[BOSS] Phase 2 Activated")));
        }
    	
    	int32 OldPhaseInt = static_cast<int32>(EBossPhase::Phase1) + 1;
    	int32 NewPhaseInt = static_cast<int32>(EBossPhase::Phase2) + 1;

        // 페이즈 변경 델리게이트 호출
        OnPhaseChanged.Broadcast(OldPhaseInt, NewPhaseInt);
    }
}

void ASTEnemyBoss::PhaseChangingNotify()
{
    if (!GetMesh()) return;

    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        AnimInstance->StopAllMontages(0.0f);
    }

    FTimerHandle MeshChangeTimer;
    GetWorld()->GetTimerManager().SetTimer(MeshChangeTimer, [this]()
    {
        if (!GetMesh()) return;
        
        // 메쉬를 Crunch_Titanium으로 변경 (이거 일단 임시로 이렇게 해놓고 나중에 변경예정)
        USkeletalMesh* NewMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/ParagonCrunch/Characters/Heroes/Crunch/Skins/Teir_1/Titanium/Meshes/Crunch_Titanium"));
        if (NewMesh)
        {
            // 기존 애니메이션 인스턴스 백업
            UClass* CurrentAnimClass = GetMesh()->GetAnimClass();
            
            GetMesh()->SetSkeletalMesh(NewMesh);
            
            // 애니메이션 인스턴스 재설정
            if (CurrentAnimClass)
            {
                GetMesh()->SetAnimInstanceClass(CurrentAnimClass);
            }
            
            if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
            {
                if (USTBossAnimInstance* EnemyAnimInstance = Cast<USTBossAnimInstance>(AnimInstance))
                {
                    EnemyAnimInstance->bIsPhaseChangingEnd = true;
                }
            }
        }
    }, 0.1f, false);

    // 추가적으로 파티클, 소리도 추가해야함

    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        if (USTBossAnimInstance* EnemyAnimInstance = Cast<USTBossAnimInstance>(AnimInstance))
        {
            EnemyAnimInstance->bIsPhaseChangingEnd = true;
        }
    }
}

void ASTEnemyBoss::PhaseChangingEndNotify()
{
    StateComponent->SetState(EEnemyState::Idle);
}

void ASTEnemyBoss::Die()
{
    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        AIController->StopMovement();
        if (AIController->BrainComponent)
        {
            AIController->BrainComponent->StopLogic(TEXT("Dead"));
        }
    }

    if (GetMesh()->GetAnimInstance())
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        
        AnimInstance->StopAllMontages(0.1f);
    }
    
    if (DeathSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
    }
    Super::Die();
}
