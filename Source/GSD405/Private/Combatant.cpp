// Fill out your copyright notice in the Description page of Project Settings.


#include "Combatant.h"

// Sets default values
ACombatant::ACombatant()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StatusWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	StatusWidget->SetupAttachment(GetCapsuleComponent(), FName("StatusWidget"));
	StatusWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCapsuleHalfHeight(64);
}

int ACombatant::GetHealth()
{
	return Health;
}

int ACombatant::GetMaxHealth()
{
	return MaxHealth;
}

void ACombatant::AddEffect(FEffectInstance CardEffect, UObject* Applier)
{
	UEffect* Effect = NewObject<UEffect>(this,CardEffect.Effect);
	Effects.Add(Effect);
	Effect->Magnitude = CardEffect.Magnitude;
	Effect->Owner = this;
	Effect->Applier = Applier;
	Effect->Event(EEffectEvent::EffectApplied, nullptr);
	OnAddEffect.Broadcast(Effect);
	CullEffects();
}


void ACombatant::ClearEffects()
{
	for (UEffect* Effect : Effects) {
		Effect->RemoveEffect();
	}
}

void ACombatant::Heal_Implementation(int  HealAmount)
{
	Health = FMath::Clamp(Health + HealAmount, 0, MaxHealth);
}



void ACombatant::Damage_Implementation(int Damage, UObject* Responsible)
{
	UDamagePayload* DamagePayload = NewObject<UDamagePayload>();
	DamagePayload->Damage = Damage;
	DamagePayload->Responsible = Responsible;
	DamagePayload->Damaged = this;
	UE_LOG(LogTemp, Log, TEXT("Damage PreMitigation %d"), DamagePayload->Damage);

	if (Responsible && Responsible->Implements<UEffectable>()){
		IEffectable* InterfaceInstance = Cast<IEffectable>(Responsible);

		InterfaceInstance->CallEffectEvent(EEffectEvent::DealDamagePreMitigation, DamagePayload);
		CallEffectEvent(EEffectEvent::TakeDamagePreMitigation, DamagePayload);

		InterfaceInstance->CallEffectEvent(EEffectEvent::DealDamagePostMitigation, DamagePayload);
		CallEffectEvent(EEffectEvent::TakeDamagePostMitigation, DamagePayload);
	}
	else {
		CallEffectEvent(EEffectEvent::TakeDamagePreMitigation, DamagePayload);
		CallEffectEvent(EEffectEvent::TakeDamagePostMitigation, DamagePayload);
	}
	

	UE_LOG(LogTemp, Log, TEXT("Damage Post Mitigation %d"), DamagePayload->Damage);
	Health -= DamagePayload->Damage;
	if (Health <= 0) {
		Die();
	}
}

void ACombatant::EndTurn_Implementation()
{
	CallEffectEvent(EEffectEvent::TurnEnd, nullptr);
	EndTurnDelegate.ExecuteIfBound();
	isTurn = false;
}

void ACombatant::StartTurn_Implementation()
{
	if (!isAlive) {
		EndTurn();
		return;
	}
	isTurn = true;
	RefreshAP();
	CallEffectEvent(EEffectEvent::TurnStart, nullptr);
}

void ACombatant::CallEffectEvent(EEffectEvent Event, UObject* Payload)
{
	for (int i = 0; i < Effects.Num(); i++) {
			Effects[i]->Event(Event, Payload);
			UE_LOG(LogTemp, Log, TEXT("NUM EFFECTS : %d"), Effects.Num());
	}

	switch (Event) {
		case EEffectEvent::RoundStart:
		case EEffectEvent::TurnEnd:
		case EEffectEvent::TurnStart:
		case EEffectEvent::SkillUsed:
			CullEffects();
		
		default:
			break;
	}
}

void ACombatant::Die_Implementation()
{
	isAlive = false;
	for (UEffect* Effect : Effects) {
		Effect->MarkedForRemoval = 1;
	}
	CullEffects();

	DeathDelegate.ExecuteIfBound();
}

// Called when the game starts or when spawned
void ACombatant::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACombatant::StartCombat_Implementation()
{
	ClearEffects();
}

void ACombatant::RefreshAP()
{
	ModifyAP(MaxActionPoints);
}

void ACombatant::CombatEnd_Implementation()
{
}

void ACombatant::ModifyAP_Implementation(int Modifier)
{
	ActionPoints = FMath::Clamp(ActionPoints + Modifier, 0, MaxActionPoints);
}

void ACombatant::GetTargets(TEnumAsByte<ETargetingType> TargetingType, TArray<ACombatant*>& ReturnByRef)
{
	TArray<ACombatant*> Targets;
	GetTargetsDelegate.ExecuteIfBound(this, Targets);
	int self = Targets.Find(this);
	int flag = self + 1;
	switch (TargetingType) {
	case ETargetingType::Self:
		ReturnByRef.Add(Targets[self]);
		break;
	case ETargetingType::AllOpposing:
		for (int i = self + 1; i < Targets.Num(); i++)
			ReturnByRef.Add(Targets[i]);
		break;
	case ETargetingType::All:
		ReturnByRef.Append(Targets);
		break;
	case ETargetingType::Melee:
		for (int i = self+1; i < Targets.Num(); i++)
			if (Targets[i]->isAlive) {
				ReturnByRef.Add(Targets[i]);
				break;
			}
		break;
	case ETargetingType::Ranged:
		for(int i = Targets.Num()-1; i > 0; i--)
			if (Targets[i]->isAlive) {
				ReturnByRef.Add(Targets[i]);
				break;
			}
		break;
	case ETargetingType::MostHealth:
		for (int i = self + 1; i < Targets.Num(); i++)
			if (Targets[i]->GetHealth() > Targets[flag]->GetHealth())
				flag = i;
		ReturnByRef.Add(Targets[flag]);
		break;
	}
	
}

void ACombatant::CullEffects()
{
	UE_LOG(LogTemp, Log, TEXT("CULLING"));
	for (int i = Effects.Num() - 1; i >= 0; i--) {
		if (Effects[i]->MarkedForRemoval) {
			OnRemoveEffect.Broadcast(Effects[i]);
			Effects[i]->ConditionalBeginDestroy();
			Effects.RemoveAt(i);
		}
		UE_LOG(LogTemp, Log, TEXT("i : %d"), i);
	}
}


void ACombatant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACombatant::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


