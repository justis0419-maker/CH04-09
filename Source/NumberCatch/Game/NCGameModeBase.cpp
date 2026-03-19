// Fill out your copyright notice in the Description page of Project Settings.


#include "NCGameModeBase.h"
#include "NCGameStateBase.h"
#include "Player/NCPlayerController.h"
#include "EngineUtils.h"
#include <Player\NCPlayerState.h>

void ANCGameModeBase::OnPostLogin(AController* NewPlayer)
{

	Super::OnPostLogin(NewPlayer);




	ANCPlayerController* NCPlayerController = Cast<ANCPlayerController>(NewPlayer);
	if (IsValid(NCPlayerController) == true)
	{
		NCPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
		AllPlayerControllers.Add(NCPlayerController);

		ANCPlayerState* NCPS = NCPlayerController->GetPlayerState<ANCPlayerState>();
		if (IsValid(NCPS) == true)
		{
			NCPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ANCGameStateBase* NCGameStateBase = GetGameState<ANCGameStateBase>();
		if (IsValid(NCGameStateBase) == true)
		{
			NCGameStateBase->MulticastRPCBroadcastLoginMessage(NCPS->PlayerNameString);
		}
	}
}

FString ANCGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ANCGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	if (InNumberString.Len() != 3)
		return false;

	TSet<TCHAR> UniqueDigits;

	for (TCHAR C : InNumberString)
	{
		if (!FChar::IsDigit(C) || C == '0')
			return false;

		if (UniqueDigits.Contains(C))
			return false;

		UniqueDigits.Add(C);
	}

	return true;
}

FString ANCGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ANCGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();

	UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);

}

void ANCGameModeBase::PrintChatMessageString(ANCPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{

	if (InChatMessageString.Len() != 3)
	{
		InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("다시 입력하세요"));
		return;
	}
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		IncreaseGuessCount(InChattingPlayerController);

		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
		
		for (TActorIterator<ANCPlayerController> It(GetWorld()); It; ++It)
		{
			ANCPlayerController* NCPlayerController = *It;
			if (IsValid(NCPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
				ANCPlayerState* NCPS = InChattingPlayerController->GetPlayerState<ANCPlayerState>();

				CombinedMessageString = FString::Printf(
					TEXT("%s -> %s (%d/%d)"),
					*InChatMessageString,
					*JudgeResultString,
					NCPS ? NCPS->CurrentGuessCount : 0,
					NCPS ? NCPS->MaxGuessCount : 3
				);
				
				
				NCPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
		
			
			}

		}

		int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
		JudgeGame(InChattingPlayerController, StrikeCount);
	}
	else
	{

		InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("다시 입력하세요"));
		for (TActorIterator<ANCPlayerController> It(GetWorld()); It; ++It)
		{
			ANCPlayerController* NCPlayerController = *It;
			if (IsValid(NCPlayerController) == true)
			{
				NCPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}

}

void ANCGameModeBase::IncreaseGuessCount(ANCPlayerController* InChattingPlayerController)
{
	ANCPlayerState* NCPS = InChattingPlayerController->GetPlayerState<ANCPlayerState>();
	if (IsValid(NCPS) == true)
	{
		NCPS->CurrentGuessCount++;
	}
}

void ANCGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);

	for (const auto& NCPlayerController : AllPlayerControllers)
	{
		if (IsValid(NCPlayerController))
		{
		
			NCPlayerController->NotificationText =
				FText::FromString(TEXT("Connected to the game server."));

			ANCPlayerState* NCPS = NCPlayerController->GetPlayerState<ANCPlayerState>();
			if (IsValid(NCPS) == true)
			{
				NCPS->CurrentGuessCount = 0;
			}
		}
	}
}
void ANCGameModeBase::JudgeGame(ANCPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		ANCPlayerState* NCPS = InChattingPlayerController->GetPlayerState<ANCPlayerState>();
		for (const auto& NCPlayerController : AllPlayerControllers)
		{
			if (IsValid(NCPlayerController) && IsValid(NCPS))
			{
				FString CombinedMessageString = NCPS->PlayerNameString + TEXT(" has won the game.");
				NCPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				
			}
		}

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ANCGameModeBase::ResetGame, 3.0f, false);
	}
	else
	{
		bool bIsDraw = false;
		for (const auto& NCPlayerController : AllPlayerControllers)
		{
			ANCPlayerState* NCPS = NCPlayerController->GetPlayerState<ANCPlayerState>();
			if (IsValid(NCPS) == true)
			{
				if (NCPS->CurrentGuessCount >= NCPS->MaxGuessCount)
				{
					bIsDraw = true;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& NCPlayerController : AllPlayerControllers)
			{
				NCPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));

	
			}
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ANCGameModeBase::ResetGame, 3.0f, false);
		}
	}
}
