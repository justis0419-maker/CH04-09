// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NCPawn.generated.h"

UCLASS()
class NUMBERCATCH_API ANCPawn : public APawn
{
	GENERATED_BODY()



protected:

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;


};
