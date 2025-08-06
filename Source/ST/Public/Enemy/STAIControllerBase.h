#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "STEnemyAlertReceiver.h"
#include "Perception/AIPerceptionTypes.h"
#include "STAIControllerBase.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UBlackboardComponent;
class UBehaviorTree;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRecognitionGaugeChanged, float);

UCLASS(Abstract)
class ST_API ASTAIControllerBase : public AAIController, public ISTEnemyAlertReceiver
{
    GENERATED_BODY()
public:
    ASTAIControllerBase();

    FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const { return BlackboardComp; }
    virtual void StartBehaviorTree();
    virtual void NotifyPlayerAttack(AActor* DamagedEnemy) override;

    // 인지 게이지 Getter/Setter
    float GetRecognitionGauge() const { return RecognitionGauge; }
    float GetRecognitionGaugeMax() const { return RecognitionGaugeMax; }
    void AddToRecognitionGauge(float Delta);
    void SetRecognitionGauge(float NewValue);

    // Delegate, UI에서 바인딩
    FOnRecognitionGaugeChanged OnRecognitionGaugeChanged;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAIPerceptionComponent* AIPerception;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAISenseConfig_Sight* SightConfig;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAISenseConfig_Hearing* HearingConfig;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UBlackboardComponent* BlackboardComp;

    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION()
    virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
    virtual float GetRecognitionGaugeGainRate(float Distance) const;

    float RecognitionGauge = 0.f;
    const float RecognitionGaugeMax = 100.f;
    bool bPlayerInSight = false;
};
