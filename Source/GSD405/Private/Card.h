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
	int ActionPointCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int ActivationPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEffectInstance> Effects;
};

UCLASS(Blueprintable, BlueprintType)
class UCardInstance : public UObject
{
	GENERATED_BODY()

public:
	UCardInstance() {}

	bool operator== (const UCardInstance& Other) const
	{
		return CardData == Other.CardData && Genre == Other.Genre;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCard* CardData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EGenre> Genre;
};



