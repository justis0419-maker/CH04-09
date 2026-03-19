#pragma once


#include <Player\NCPlayerController.h>

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NCGameModeBase.generated.h"


class ANCPlayerController;
/**
 * 
 */
UCLASS()
class NUMBERCATCH_API ANCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	virtual void BeginPlay() override;

	void PrintChatMessageString(ANCPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(ANCPlayerController* InChattingPlayerController);

	void ResetGame();

	void JudgeGame(ANCPlayerController* InChattingPlayerController, int InStrikeCount);


protected:
	FString SecretNumberString;

	TArray<TObjectPtr<ANCPlayerController>> AllPlayerControllers;


};
