// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "STWeaponType.generated.h"

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	//자동 사격
	Automatic UMETA(DisplayName ="Automatic"),
	//단발 사격
	SemiAutomatic UMETA(DisplayName="Semi-Automatic")
};

UENUM(BlueprintType)
enum class EWeaponType: uint8
{
	Rifle   UMETA(DisplayName = "Rifle"),
	Sniper  UMETA(DisplayName = "Sniper"),
	Shotgun UMETA(DisplayName = "Shotgun")
};

USTRUCT(BlueprintType)

struct FSTWeaponData
{
	
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFireMode FireMode = EFireMode::Automatic;
	//무기 타입 지정
	UPROPERTY(EditAnywhere,  Category ="Weapon Type")
	EWeaponType WeaponType;

	//데미지
	UPROPERTY(EditAnywhere,  Category ="Weapon Stats")
	float Damage;
	UPROPERTY(EditAnywhere,  Category ="Weapon Stats")
	float FireRate;
	UPROPERTY(EditAnywhere,  Category ="Weapon Stats")
	int32 MagazineSize;
	UPROPERTY(EditAnywhere,  Category ="Weapon Stats")
	float ReloadTime;


	//총알이 퍼지는 각도 설정
	UPROPERTY(EditAnywhere, Category = "Weapon Stats|Shotgun")
	float SpreadAngle;
	// --- 샷건 전용 스탯 ---


	// 벅샷의 개수
	UPROPERTY(EditAnywhere, Category = "Weapon Stats|Shotgun")
	int32 PelletsPerShot;
	//무기의 사거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceDistance = 10000.f;
};