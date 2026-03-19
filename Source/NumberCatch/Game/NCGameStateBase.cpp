// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NCGameStateBase.h"
#include <Player\NCPlayerController.h>
#include <Kismet\GameplayStatics.h>

void ANCGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			ANCPlayerController* NCPC = Cast<ANCPlayerController>(PC);
			if (IsValid(NCPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				NCPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}