#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"


UCLASS()
class MPSHOOTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

	UFUNCTION(BlueprintCallable)
	void SetDisplayText(FString TextToDisplay);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerLocalRole(APawn* InPawn);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerRemoteRole(APawn* InPawn);
	
protected:
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;
};
