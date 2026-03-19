// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NCPawn.h"
#include "NumberCatch.h"


// Called when the game starts or when spawned
void ANCPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRoleString = NumberCatchFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("NCPawn::BeginPlay() %s [%s]"), *NumberCatchFunctionLibrary::GetNetModeString(this), *NetRoleString);
	NumberCatchFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ANCPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = NumberCatchFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("NCPawn::PossessedBy() %s [%s]"), *NumberCatchFunctionLibrary::GetNetModeString(this), *NetRoleString);
	NumberCatchFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}


