// Fill out your copyright notice in the Description page of Project Settings.


#include "NCPlayerController.h"
#include "UI/NCChatInput.h"
#include <Kismet\KismetSystemLibrary.h>
#include <NumberCatch.h>
#include "EngineUtils.h"
#include <Kismet\GameplayStatics.h>
#include <Game\NCGameModeBase.h>
#include "NCPlayerState.h"
#include "Net/UnrealNetwork.h"

ANCPlayerController::ANCPlayerController()
{
	bReplicates = true;
}

void ANCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UNCChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void ANCPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	if (IsLocalController() == true)
	{

		ANCPlayerState* NCPS = GetPlayerState<ANCPlayerState>();
		if (IsValid(NCPS) == true)
		{
			FString CombinedMessageString = InChatMessageString;
			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}

}

void ANCPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	NumberCatchFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void ANCPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);

}

void ANCPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ANCPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		ANCGameModeBase* NCGM = Cast<ANCGameModeBase>(GM);
		if (IsValid(NCGM) == true)
		{
			NCGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}
