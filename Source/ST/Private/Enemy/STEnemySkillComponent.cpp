#include "Enemy/STEnemySkillComponent.h"
#include "Enemy/STEnemyBase.h"
#include "Enemy/STEnemySkillProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Enemy/STEnemyBoss.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

USTEnemySkillComponent::USTEnemySkillComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USTEnemySkillComponent::BeginPlay()
{
    Super::BeginPlay();
    OwnerEnemy = Cast<ASTEnemyBase>(GetOwner());
    
    // 스킬 캐시 초기화
    InitializeSkillCache();

    bPositionModified = false;
    OriginalPosition = FVector::ZeroVector;
}

void USTEnemySkillComponent::InitializeSkillCache()
{
    if (!SkillDataTable) 
    {
        return;
    }

    // 데이터테이블의 모든 스킬 수집
    const TArray<FName> RowNames = SkillDataTable->GetRowNames();
    TArray<TPair<EEnemySkillID, const FEnemySkillData*>> AllSkills;
    
    for (const FName& RowName : RowNames)
    {
        const FEnemySkillData* Data = SkillDataTable->FindRow<FEnemySkillData>(RowName, TEXT("InitializeSkillCache"));
        if (Data && Data->SkillID != EEnemySkillID::None)
        {
            // 캐시에 저장
            CachedSkillData.Add(Data->SkillID, Data);
            AllSkills.Add(TPair<EEnemySkillID, const FEnemySkillData*>(Data->SkillID, Data));
        }
    }
    
    // 쿨타임 기준 내림차순 정렬
    AllSkills.Sort([](const TPair<EEnemySkillID, const FEnemySkillData*>& A, const TPair<EEnemySkillID, const FEnemySkillData*>& B)
    {
        return A.Value->Cooldown > B.Value->Cooldown;
    });
    
    // 정렬된 스킬 ID만 추출
    SkillsByPriority.Reset();
    for (const auto& SkillPair : AllSkills)
    {
        SkillsByPriority.Add(SkillPair.Key);
    }
}

const FEnemySkillData* USTEnemySkillComponent::GetCachedSkillData(EEnemySkillID SkillID) const
{
    return CachedSkillData.FindRef(SkillID);
}

const FEnemySkillData* USTEnemySkillComponent::GetSkillData(EEnemySkillID SkillID) const
{
    // 캐시에서 먼저 찾기
    if (const FEnemySkillData* const* CachedData = CachedSkillData.Find(SkillID))
    {
        return *CachedData;
    }
    
    if (!SkillDataTable) return nullptr;
    
    // 캐시에 없으면 데이터테이블에서 찾기
    const TArray<FName> RowNames = SkillDataTable->GetRowNames();
    for (const FName& RowName : RowNames)
    {
        const FEnemySkillData* Data = SkillDataTable->FindRow<FEnemySkillData>(RowName, TEXT("GetSkillData"));
        if (Data && Data->SkillID == SkillID)
        {
            return Data;
        }
    }
    return nullptr;
}

bool USTEnemySkillComponent::IsSkillReady(EEnemySkillID SkillID) const
{
    const FEnemySkillData* SkillDef = GetSkillData(SkillID);
    if (!SkillDef) return false;

    const float Now = GetWorld()->GetTimeSeconds();
    const float* LastUsed = SkillLastUsedTime.Find(SkillID);
    
    if (LastUsed && *LastUsed > 0.f && Now - *LastUsed < SkillDef->Cooldown)
    {
        return false;
    }
    
    return true;
}

bool USTEnemySkillComponent::CanExecuteAnySkill() const
{
	if (!OwnerEnemy) return false;

    // 이미 스킬 실행 중인지 체크
    if (bIsAnySkillExecuting)
    {
        return false;
    }
    
    // 스킬 간 최소 간격 체크
    float TimeSinceLastSkill = GetWorld()->GetTimeSeconds() - LastSkillExecutionTime;
    if (TimeSinceLastSkill < MinSkillInterval)
    {
        return false;
    }
    
    return true;
}

bool USTEnemySkillComponent::TryActivateSkill(EEnemySkillID SkillID, AActor* Target)
{
    if (!CanExecuteAnySkill() || OwnerEnemy->StateComponent->IsInState(EEnemyState::PhaseChanging))
    {
        return false;
    }

    if (!OwnerEnemy)
    {
        OwnerEnemy = Cast<ASTEnemyBase>(GetOwner());
        if (!OwnerEnemy) return false;
    }
	
    const FEnemySkillData* SkillDef = GetSkillData(SkillID);
    if (!SkillDef) return false;

    // 개별 스킬 체크
    if (!IsSkillReady(SkillID)) return false;

    // 거리 체크
    if (Target && SkillDef->Range > 0.f)
    {
        const float Distance = FVector::Dist(OwnerEnemy->GetActorLocation(), Target->GetActorLocation());
        if (Distance > SkillDef->Range) return false;
    }

    // 현재 스킬 정보 저장
    CurrentSkillID = SkillID;
    LastActivatedSkillID = SkillID;
    CurrentTarget = Target;

    // 개별 스킬 쿨타임 기록
    SkillLastUsedTime.Add(SkillID, GetWorld()->GetTimeSeconds());

    // 실행 상태 설정
    bIsAnySkillExecuting = true;
    LastSkillExecutionTime = GetWorld()->GetTimeSeconds();

    // Attack 상태로 변경
    if (OwnerEnemy && OwnerEnemy->StateComponent)
    {
        OwnerEnemy->StateComponent->SetState(EEnemyState::Attack);
    }

    // Action 시스템 실행
    if (SkillDef->Actions.Num() > 0)
    {
        ExecuteSkillActions(*SkillDef, Target);
    }

    return true;
}

void USTEnemySkillComponent::AttackNotify()
{
    if (!OwnerEnemy) return;

    // Action 시스템이 실행 중인 경우
    if (bExecutingActionSequence)
    {
        const FEnemySkillData* SkillDef = GetSkillData(CurrentSkillID);
        if (SkillDef)
        {
            ExecuteSkill(*SkillDef, CurrentTarget);
        }
        return;
    }

    const FEnemySkillData* SkillDef = GetSkillData(CurrentSkillID);
    if (!SkillDef)
    {
        SkillDef = GetSkillData(LastActivatedSkillID);
        if (!SkillDef) return;
    }

    ExecuteSkill(*SkillDef, CurrentTarget);
}

void USTEnemySkillComponent::LaunchProjectile()
{
    if (!OwnerEnemy) return;

    if (bExecutingActionSequence)
    {
        ExecuteNextAction();
        return;
    }
}

void USTEnemySkillComponent::AttackEndNotify()
{
    
    if (bExecutingActionSequence)
    {
        CompleteActionSequence();
    }
    else
    {
        // 실행 상태 해제
        bIsAnySkillExecuting = false;
        
        if (OwnerEnemy && OwnerEnemy->StateComponent)
        {
            OwnerEnemy->StateComponent->SetState(EEnemyState::Idle);
        }
    }
}

void USTEnemySkillComponent::ExecuteSkill(const FEnemySkillData& Skill, AActor* Target)
{
    if (!OwnerEnemy) return;

    // 사운드 재생
    if (Skill.CastSound)
    {
        if (Skill.CastSoundSockets.Num() == 0)
        {
            UGameplayStatics::PlaySoundAtLocation(this, Skill.CastSound, OwnerEnemy->GetActorLocation());
        }
        else
        {
            for (const FName& SocketName : Skill.CastSoundSockets)
            {
                if (OwnerEnemy->GetMesh() && OwnerEnemy->GetMesh()->DoesSocketExist(SocketName))
                {
                    FVector SocketLoc = OwnerEnemy->GetMesh()->GetSocketLocation(SocketName);
                    UGameplayStatics::PlaySoundAtLocation(this, Skill.CastSound, SocketLoc);
                }
            }
        }
    }

    // 데미지 적용
    if (Skill.AreaRadius > 0.f)
    {
        TArray<AActor*> IgnoreActors;
        IgnoreActors.Add(OwnerEnemy);
        
        // 범위 데미지
        UGameplayStatics::ApplyRadialDamage(
            GetWorld(),
            Skill.Damage,
            OwnerEnemy->GetActorLocation(),
            Skill.AreaRadius,
            nullptr,
            IgnoreActors,
            OwnerEnemy,
            OwnerEnemy->GetController(),
            true
        );
    }
    else if (Target && IsValid(Target))
    {
        // 단일 타겟 데미지
        UGameplayStatics::ApplyDamage(Target, Skill.Damage, OwnerEnemy->GetController(), OwnerEnemy, nullptr);
    }
}

void USTEnemySkillComponent::ExecuteSkillActions(const FEnemySkillData& SkillData, AActor* Target)
{
    if (SkillData.Actions.Num() == 0) return;

    if (bExecutingActionSequence)
    {
        GetWorld()->GetTimerManager().ClearTimer(ActionTimerHandle);
    }

    CurrentActionSequence = SkillData.Actions;
    CurrentTarget = Target;
    CurrentActionIndex = -1;
    bExecutingActionSequence = true;

    ExecuteNextAction();
}

void USTEnemySkillComponent::ExecuteNextAction()
{
    if (!bExecutingActionSequence || CurrentActionSequence.Num() == 0) return;

    CurrentActionIndex++;

    if (CurrentActionIndex >= CurrentActionSequence.Num())
    {
        CompleteActionSequence();
        return;
    }

    const FSkillAction& CurrentAction = CurrentActionSequence[CurrentActionIndex];

    if (CurrentAction.ExecutionDelay > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            ActionTimerHandle,
            [this, CurrentAction]() { ExecuteAction(CurrentAction, CurrentTarget); },
            CurrentAction.ExecutionDelay,
            false
        );
    }
    else
    {
        ExecuteAction(CurrentAction, CurrentTarget);
    }
}

void USTEnemySkillComponent::ExecuteAction(const FSkillAction& Action, AActor* Target)
{
    if (!OwnerEnemy) return;

    if (OwnerEnemy->StateComponent &&
        OwnerEnemy->StateComponent->IsInState(EEnemyState::Dead) ||
        OwnerEnemy->StateComponent->IsInState(EEnemyState::PhaseChanging))
    {
        CompleteActionSequence();
        return;
    }

    PlayActionEffects(Action);

    switch (Action.ActionType)
    {
    case ESkillActionType::PlayMontage:
        ExecutePlayMontage(Action);
        break;
    case ESkillActionType::SpawnProjectile:
        ExecuteSpawnProjectile(Action, Target);
        break;
    case ESkillActionType::ExecuteDash:
        ExecuteDashAction(Action, Target);
        break;
    case ESkillActionType::SetVisibility:
        ExecuteSetVisibility(Action);
        break;
    default:
        break;
    }

    if (!Action.bWaitForPreviousComplete)
    {
        ExecuteNextAction();
    }
}

void USTEnemySkillComponent::ExecutePlayMontage(const FSkillAction& Action)
{
    if (!Action.AnimMontage || !OwnerEnemy || !OwnerEnemy->GetMesh()) return;

    UAnimInstance* AnimInstance = OwnerEnemy->GetMesh()->GetAnimInstance();
    if (!AnimInstance) return;

    // 기존 몽타주 정지
    AnimInstance->StopAllMontages(Action.BlendOutTime);

    // 새 몽타주 재생
    float PlayLength = AnimInstance->Montage_Play(Action.AnimMontage, Action.PlayRate);

    // 파티클 및 사운드 재생
    if (Action.ParticleEffects.Num() > 0)
    {
        PlayMultipleParticleEffects(Action.ParticleEffects);
    }

    if (Action.SoundEffect)
    {
        if (Action.SoundSocketNames.Num() == 0)
        {
            UGameplayStatics::PlaySoundAtLocation(this, Action.SoundEffect, OwnerEnemy->GetActorLocation());
        }
        else
        {
            for (const FName& Socket : Action.SoundSocketNames)
            {
                if (OwnerEnemy->GetMesh()->DoesSocketExist(Socket))
                {
                    FVector SocketLoc = OwnerEnemy->GetMesh()->GetSocketLocation(Socket);
                    UGameplayStatics::PlaySoundAtLocation(this, Action.SoundEffect, SocketLoc);
                }
            }
        }
    }

    // 완료 처리
    if (Action.bWaitForPreviousComplete && PlayLength > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            ActionTimerHandle,
            this,
            &USTEnemySkillComponent::ExecuteNextAction,
            PlayLength - 0.1f,
            false
        );
    }
}

void USTEnemySkillComponent::ExecuteSpawnProjectile(const FSkillAction& Action, AActor* Target)
{
    if (!Action.ProjectileClass || !IsValid(Action.ProjectileClass) || !OwnerEnemy) return;

    // 스폰 위치 계산
    FVector SpawnLoc = OwnerEnemy->GetActorLocation();
    if (Action.SpawnSocketName != NAME_None && OwnerEnemy->GetMesh() && OwnerEnemy->GetMesh()->DoesSocketExist(Action.SpawnSocketName))
    {
        SpawnLoc = OwnerEnemy->GetMesh()->GetSocketLocation(Action.SpawnSocketName);
    }

    FVector TargetLoc = Target ? Target->GetActorLocation() : 
        (OwnerEnemy->GetActorLocation() + OwnerEnemy->GetActorForwardVector() * 1000.f);
    
    FVector Dir = (TargetLoc - SpawnLoc).GetSafeNormal();
    FRotator Rot = Dir.Rotation();

    // SpawnActorDeferred 사용
    if (AActor* SpawnedProjectile = GetWorld()->SpawnActorDeferred<AActor>(Action.ProjectileClass, FTransform(Rot, SpawnLoc)))
    {
        if (ASTEnemySkillProjectile* EnemyProjectile = Cast<ASTEnemySkillProjectile>(SpawnedProjectile))
        {
            EnemyProjectile->SetInstigator(Cast<APawn>(OwnerEnemy));
            // BeginPlay 전에 InitializeProjectile 호출
            EnemyProjectile->InitializeProjectile(
                Action.ProjectileDamage,
                Action.DamageRadius,
                Action.ProjectileTrailEffect,
                Action.ProjectileHitEffect,
                Action.ProjectileFireSound,
                Action.ProjectileHitSound,
                Action.bDelayedExplosion,
                Action.ExplosionDelay,
                Action.ProjectileNiagaraTrailEffect,
            Action.ProjectileNiagaraHitEffect   
            );
            
            // 외형, 물리 설정 (투사체 속도, 중력, 바닥에 닿으면 바운스 여부, 바운스 강도 설정)
            EnemyProjectile->SetProjectileAppearance(Action.ProjectileMesh, Action.ProjectileMaterial, Action.ProjectileMeshScale);
            EnemyProjectile->SetProjectileMovementSettings(Action.ProjectileSpeed, Action.ProjectileGravityScale, Action.bProjectileShouldBounce, Action.ProjectileBounciness);
            
            UGameplayStatics::FinishSpawningActor(EnemyProjectile, FTransform(Rot, SpawnLoc));
            
            EnemyProjectile->FireInDirection(Dir);
        }
    }

    if (Action.bWaitForPreviousComplete)
    {
        ExecuteNextAction();
    }
}

void USTEnemySkillComponent::ExecuteDashAction(const FSkillAction& Action, AActor* Target)
{
    if (!OwnerEnemy) return;

    FVector StartLocation = OwnerEnemy->GetActorLocation();
    FVector DashTarget = StartLocation;
    
    if (Action.bDashToTarget && Target)
    {
        FVector TargetLocation = Target->GetActorLocation();
        TargetLocation.Z = StartLocation.Z;
        DashTarget = TargetLocation;
    }
    else
    {
        FVector ForwardVector = OwnerEnemy->GetActorForwardVector();
        ForwardVector.Z = 0.0f;
        ForwardVector.Normalize();
        
        DashTarget = StartLocation + ForwardVector * Action.DashDistance;
        DashTarget.Z = StartLocation.Z;
    }

    // 대시 방향 계산
    FVector DashDirection = (DashTarget - StartLocation).GetSafeNormal();
    float TotalDistance = FVector::Dist(StartLocation, DashTarget);
    TotalDistance = FMath::Min(TotalDistance, Action.DashDistance);

    // 대시 몽타주
    if (Action.DashLoopMontage && OwnerEnemy->GetMesh())
    {
        UAnimInstance* AnimInstance = OwnerEnemy->GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->StopAllMontages(0.1f);
            
            float PlayLength = AnimInstance->Montage_Play(
                Action.DashLoopMontage, 
                Action.PlayRate, 
                EMontagePlayReturnType::MontageLength, 
                0.0f, 
                false
            );
        }
    }

    // 실제 대시 이동 실행
    float DashDuration = TotalDistance / Action.DashSpeed;
    float UpdateInterval = 0.02f;
    int32 UpdateSteps = FMath::Max(1, FMath::FloorToInt(DashDuration / UpdateInterval));
    FVector StepVector = DashDirection * (TotalDistance / UpdateSteps);

    // 대시 상태 저장 <- 한번에 이동하는게 아닌 부드럽게 연속적으로 이동해야 함 그래서 저장하면서 나눠서 이동
    struct FDashData
    {
        FVector CurrentLocation;
        FVector TargetLocation;
        FVector StepVector;
        int32 RemainingSteps;
        USTEnemySkillComponent* SkillComponent;
        const FSkillAction* ActionRef;
        float OriginalZ; 
    };

    FDashData* DashData = new FDashData();
    DashData->CurrentLocation = StartLocation;
    DashData->TargetLocation = DashTarget;
    DashData->StepVector = StepVector;
    DashData->RemainingSteps = UpdateSteps;
    DashData->SkillComponent = this;
    DashData->ActionRef = &Action;
    DashData->OriginalZ = StartLocation.Z;
    DashData->StepVector.Z = 0.0f;

    // 대시 업데이트 타이머 시작
    GetWorld()->GetTimerManager().SetTimer(
        ActionTimerHandle,
        [this, DashData]()
        {
            UpdateDashMovement(DashData);
        },
        UpdateInterval,
        true
    );
}

void USTEnemySkillComponent::ExecuteSetVisibility(const FSkillAction& Action)
{
    if (!OwnerEnemy || !OwnerEnemy->GetMesh()) return;

    if (!Action.bVisible)
    {
        if (bPositionModified) return;

        OriginalPosition = OwnerEnemy->GetActorLocation();
        bPositionModified = true;
        
        FVector InvisiblePosition = OwnerEnemy->GetActorLocation();
        InvisiblePosition.Z += 5000.0f;

        // 텔레포트로 즉시 이동 <- 이거는 히트스캔, 투사체나 범위 공격등 모든 공격 안 맞게하기 위해서 액터 자체를 잠깐 다른 곳으로 이동
        OwnerEnemy->SetActorLocation(InvisiblePosition, false, nullptr, ETeleportType::TeleportPhysics);
    }
    else
    {
        // 투명화 해제 시 원위치 복구
        if (!bPositionModified) return;

        // 원래 위치로 복구
        OwnerEnemy->SetActorLocation(OriginalPosition, false, nullptr, ETeleportType::TeleportPhysics);
        bPositionModified = false;
    }

    // 투명/불투명 설정
    OwnerEnemy->GetMesh()->SetVisibility(Action.bVisible);
    
    // 경고 파티클 재생 (투명화 시에만)
    if (!Action.bVisible && Action.ParticleEffects.Num() > 0)
    {
        PlayMultipleParticleEffects(Action.ParticleEffects);
    }

    // 지속시간이 있으면 타이머 설정
    if (Action.VisibilityDuration > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(
        ActionTimerHandle,
        this,
        &USTEnemySkillComponent::ExecuteNextAction,
        Action.VisibilityDuration,
        false
        );
    }
    else if (!Action.bWaitForPreviousComplete)
    {
        ExecuteNextAction();
    }
    else
    {
        ExecuteNextAction();
    }
}

void USTEnemySkillComponent::UpdateDashMovement(void* DashDataPtr)
{
    struct FDashData
    {
        FVector CurrentLocation;
        FVector TargetLocation;
        FVector StepVector;
        int32 RemainingSteps;
        USTEnemySkillComponent* SkillComponent;
        const FSkillAction* ActionRef;
        float OriginalZ;
    };

    FDashData* DashData = static_cast<FDashData*>(DashDataPtr);
    
    if (!OwnerEnemy || !DashData)
    {
        GetWorld()->GetTimerManager().ClearTimer(ActionTimerHandle);
        delete DashData;
        return;
    }

    if (!GetWorld())
    {
        delete DashData;
        return;
    }

	if (bForceStoppedByPhaseChange)
	{
		GetWorld()->GetTimerManager().ClearTimer(ActionTimerHandle);
        
		// 몽타주 정리
		if (OwnerEnemy->GetMesh())
		{
			if (UAnimInstance* AnimInstance = OwnerEnemy->GetMesh()->GetAnimInstance())
			{
				AnimInstance->StopAllMontages(0.0f);
			}
		}
        
		bForceStoppedByPhaseChange = false;
		delete DashData;
		return;
	}

	// 페이즈 변경 상태 체크 추가
	if (OwnerEnemy->StateComponent && 
		(OwnerEnemy->StateComponent->IsInState(EEnemyState::PhaseChanging) ||
		 OwnerEnemy->StateComponent->IsInState(EEnemyState::Dead)))
	{
		GetWorld()->GetTimerManager().ClearTimer(ActionTimerHandle);
        
		// 몽타주 정리
		if (OwnerEnemy->GetMesh())
		{
			if (UAnimInstance* AnimInstance = OwnerEnemy->GetMesh()->GetAnimInstance())
			{
				AnimInstance->StopAllMontages(0.0f);
			}
		}
        
		delete DashData;
		return;
	}


    // 다음 위치 계산, Z축 고정
    FVector NextLocation = DashData->CurrentLocation + DashData->StepVector;
    NextLocation.Z = DashData->OriginalZ;

    // 캐릭터 콜라이더 크기 기반으로 추적 범위 설정
    float CapsuleRadius = 50.0f;
    float CapsuleHalfHeight = 90.0f;
    
    if (OwnerEnemy && OwnerEnemy->GetCapsuleComponent())
    {
        CapsuleRadius = OwnerEnemy->GetCapsuleComponent()->GetScaledCapsuleRadius();
        CapsuleHalfHeight = OwnerEnemy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    }

    // 수평 추적을 위한 시작/끝점 설정
    FVector TraceStart = DashData->CurrentLocation;
    FVector TraceEnd = NextLocation;
    TraceStart.Z = DashData->OriginalZ;
    TraceEnd.Z = DashData->OriginalZ;

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwnerEnemy);
    
    bool bHitSomething = false;
    
    // 벽, 엄폐물 충돌 체크
    if (GetWorld()->SweepSingleByChannel(
        HitResult, 
        TraceStart, 
        TraceEnd, 
        FQuat::Identity,
        ECC_WorldStatic, 
        FCollisionShape::MakeSphere(CapsuleRadius * 0.8f),
        QueryParams))
    {
        if (FMath::Abs(HitResult.Normal.Z) < 0.7f)
        {
            bHitSomething = true;
            NextLocation = HitResult.Location;
            NextLocation.Z = DashData->OriginalZ;
            
        }
    }
    
    // 플레이어 충돌 체크
    if (!bHitSomething && GetWorld()->SweepSingleByChannel(
        HitResult, 
        TraceStart, 
        TraceEnd, 
        FQuat::Identity,
        ECC_Pawn, 
        FCollisionShape::MakeSphere(CapsuleRadius * 0.9f),
        QueryParams))
    {
        if (HitResult.GetActor())
        {
            bool bIsPlayer = false;
            
            if (HitResult.GetActor()->ActorHasTag("Player"))
            {
                bIsPlayer = true;
            }
            else if (APawn* HitPawn = Cast<APawn>(HitResult.GetActor()))
            {
                if (HitPawn->IsPlayerControlled())
                {
                    bIsPlayer = true;
                }
            }
            
            if (bIsPlayer)
            {
                bHitSomething = true;
                NextLocation = HitResult.Location;
                NextLocation.Z = DashData->OriginalZ;
            }
        }
    }

    // 위치 업데이트 - Z축 강제 고정
    NextLocation.Z = DashData->OriginalZ;
    bool bLocationSet = OwnerEnemy->SetActorLocation(NextLocation, true);
    
    if (!bLocationSet)
    {
        bHitSomething = true;
    }
    
    // 회전은 수평면에서만
    FVector HorizontalDirection = DashData->StepVector;
    HorizontalDirection.Z = 0.0f;
    if (!HorizontalDirection.IsNearlyZero())
    {
        OwnerEnemy->SetActorRotation(HorizontalDirection.Rotation());
    }
    
    DashData->CurrentLocation = NextLocation;
    DashData->RemainingSteps--;

    // 대시 완료 조건 수평 거리로만 체크
    FVector HorizontalDistance = DashData->TargetLocation - NextLocation;
    HorizontalDistance.Z = 0.0f;
    float DistanceLeft = HorizontalDistance.Size();
    
    if (bHitSomething || DashData->RemainingSteps <= 0 || DistanceLeft < 50.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(ActionTimerHandle);

        // 몽타주 정리
        if (OwnerEnemy->GetMesh())
        {
            if (UAnimInstance* AnimInstance = OwnerEnemy->GetMesh()->GetAnimInstance())
            {
                AnimInstance->StopAllMontages(0.0f);
            }
        }

        GetWorld()->GetTimerManager().SetTimer(
            ActionTimerHandle, 
            this,
            &USTEnemySkillComponent::ExecuteNextAction,
            0.05f,
            false
        );

        delete DashData;
        return;
    }
}

void USTEnemySkillComponent::PlayActionEffects(const FSkillAction& Action)
{
    if (!OwnerEnemy) return;

    if (Action.ParticleEffects.Num() > 0)
    {
        PlayMultipleParticleEffects(Action.ParticleEffects);
    }

    if (Action.SoundEffect)
    {
        if (Action.SoundSocketNames.Num() == 0)
        {
            UGameplayStatics::PlaySoundAtLocation(this, Action.SoundEffect, OwnerEnemy->GetActorLocation());
        }
        else
        {
            for (const FName& Socket : Action.SoundSocketNames)
            {
                if (OwnerEnemy->GetMesh()->DoesSocketExist(Socket))
                {
                    FVector SocketLoc = OwnerEnemy->GetMesh()->GetSocketLocation(Socket);
                    UGameplayStatics::PlaySoundAtLocation(this, Action.SoundEffect, SocketLoc);
                }
            }
        }
    }
}

void USTEnemySkillComponent::CompleteActionSequence()
{
    bExecutingActionSequence = false;
    CurrentActionIndex = -1;
    CurrentActionSequence.Empty();
    CurrentTarget = nullptr;

    GetWorld()->GetTimerManager().ClearTimer(ActionTimerHandle);
    CleanupActiveParticles();

    // 실행 상태 해제
    bIsAnySkillExecuting = false;

    // Idle 상태로 변경
    if (OwnerEnemy && OwnerEnemy->StateComponent)
    {
    	OwnerEnemy->StateComponent->SetState(EEnemyState::Idle);
    }
}

void USTEnemySkillComponent::PlayMultipleParticleEffects(const TArray<FParticleEffectSetting>& ParticleEffects)
{
    if (!OwnerEnemy || !OwnerEnemy->GetMesh()) return;

    for (const FParticleEffectSetting& ParticleEffect : ParticleEffects)
    {
        PlaySingleParticleEffect(ParticleEffect);
    }
}

void USTEnemySkillComponent::PlaySingleParticleEffect(const FParticleEffectSetting& ParticleEffect)
{
    if (!OwnerEnemy || !OwnerEnemy->GetMesh()) return;

    USkeletalMeshComponent* MeshComp = OwnerEnemy->GetMesh();

    // 각 소켓에 파티클 재생
    for (const FName& SocketName : ParticleEffect.SocketNames)
    {
        if (SocketName == NAME_None || SocketName == TEXT("None")) continue;

        USceneComponent* SpawnedComponent = nullptr;

        // Invisible 상태에서는 원래 위치 기준으로 월드에 직접 스폰
        if (bPositionModified && !OwnerEnemy->GetMesh()->IsVisible())
        {
            // 원래 위치를 기준으로 소켓 위치 계산
            FVector SpawnLocation = OriginalPosition;
            FRotator SpawnRotation = FRotator::ZeroRotator;
            
            if (MeshComp->DoesSocketExist(SocketName))
            {
                FTransform MeshTransform = FTransform(OwnerEnemy->GetActorRotation(), OriginalPosition);
                FTransform SocketLocalTransform = MeshComp->GetSocketTransform(SocketName, RTS_Component);
                
                FTransform WorldSocketTransform = SocketLocalTransform * MeshTransform;
                SpawnLocation = WorldSocketTransform.GetLocation();
            }
            else
            {
                // 소켓이 없으면 그냥 원래 위치 사용
                SpawnLocation = OriginalPosition;
            }
            
            // 파티클 오프셋 적용
            SpawnLocation += ParticleEffect.LocationOffset;
            SpawnRotation += ParticleEffect.RotationOffset;

            // Niagara 시스템
            if (ParticleEffect.NiagaraSystem)
            {
                UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                    GetWorld(),
                    ParticleEffect.NiagaraSystem,
                    SpawnLocation,
                    SpawnRotation,
                    ParticleEffect.Scale,
                    ParticleEffect.bAutoDestroy
                );
                SpawnedComponent = NiagaraComp;
            }
            // Cascade 시스템
            else if (ParticleEffect.ParticleSystem)
            {
                UParticleSystemComponent* ParticleComp = UGameplayStatics::SpawnEmitterAtLocation(
                    GetWorld(),
                    ParticleEffect.ParticleSystem,
                    SpawnLocation,
                    SpawnRotation,
                    ParticleEffect.Scale,
                    ParticleEffect.bAutoDestroy
                );
                SpawnedComponent = ParticleComp;
            }
        }
        else
        {
            // 일반 상태에서는 소켓에 부착
            if (!MeshComp->DoesSocketExist(SocketName))
            {
                continue;
            }

            // Niagara 시스템
            if (ParticleEffect.NiagaraSystem)
            {
                UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
                    ParticleEffect.NiagaraSystem,
                    MeshComp,
                    SocketName,
                    ParticleEffect.LocationOffset,
                    ParticleEffect.RotationOffset,
                    EAttachLocation::KeepRelativeOffset,
                    ParticleEffect.bAutoDestroy
                );

                SpawnedComponent = NiagaraComp;

                if (NiagaraComp)
                {
                    NiagaraComp->SetWorldScale3D(ParticleEffect.Scale);
                }
            }
            // Cascade 시스템
            else if (ParticleEffect.ParticleSystem)
            {
                UParticleSystemComponent* ParticleComp = UGameplayStatics::SpawnEmitterAttached(
                    ParticleEffect.ParticleSystem,
                    MeshComp,
                    SocketName,
                    ParticleEffect.LocationOffset,
                    ParticleEffect.RotationOffset,
                    ParticleEffect.Scale,
                    EAttachLocation::KeepRelativeOffset,
                    ParticleEffect.bAutoDestroy
                );

                SpawnedComponent = ParticleComp;
            }
        }

        // 공통 처리
        if (SpawnedComponent)
        {
            if (ParticleEffect.Duration > 0.0f)
            {
                FTimerHandle DestroyHandle;
                GetWorld()->GetTimerManager().SetTimer(
                    DestroyHandle,
                    [SpawnedComponent]()
                    {
                        if (IsValid(SpawnedComponent))
                        {
                            SpawnedComponent->DestroyComponent();
                        }
                    },
                    ParticleEffect.Duration,
                    false
                );
            }

            ActiveParticleComponents.Add(SpawnedComponent);
        }
    }
}

void USTEnemySkillComponent::CleanupActiveParticles()
{
    for (USceneComponent* Component : ActiveParticleComponents)
    {
        if (IsValid(Component))
        {
            Component->DestroyComponent();
        }
    }
    ActiveParticleComponents.Empty();
}

void USTEnemySkillComponent::ForceStopAllSkills()
{
	// 페이즈 변경으로 인한 강제 중단 플래그 설정
	bForceStoppedByPhaseChange = true;
    
	// 모든 타이머 강제 중단 (대시 타이머 포함)
	if (ActionTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ActionTimerHandle);
	}
    
	// 액션 시퀀스 강제 중단
	if (bExecutingActionSequence)
	{
		bExecutingActionSequence = false;
		CurrentActionIndex = -1;
		CurrentActionSequence.Empty();
	}
    
	// 스킬 실행 상태 초기화
	bIsAnySkillExecuting = false;
	CurrentTarget = nullptr;
    
	// 파티클 정리
	CleanupActiveParticles();

	// 0.5초 후 플래그 자동 리셋
	GetWorld()->GetTimerManager().SetTimer(
		ActionTimerHandle,
		[this]() 
		{ 
			bForceStoppedByPhaseChange = false;
		},
		0.5f,
		false
	);

}
