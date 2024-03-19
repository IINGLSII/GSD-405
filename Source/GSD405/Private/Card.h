// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Containers/Map.h"
#include "Effect.h"
#include "Card.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum ETargetingType
{
	NonTargeted = 0	UMETA(DisplayName = "NonTargeted"),
	
	Targeted = 1 UMETA(DisplayName = "Targeted")
};

USTRUCT(BlueprintType, Blueprintable)
struct FCardEffect
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UEffect> Effect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Magnitude;
};

UCLASS(Blueprintable, BlueprintType)
class UCard : public UDataAsset
{
	GENERATED_BODY()
	
	public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText CardName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* CardThumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText CardDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int CardCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCardEffect> Effects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ETargetingType> TargetingType;

};
