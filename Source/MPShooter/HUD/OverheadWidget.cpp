#include "OverheadWidget.h"
#include "Components/TextBlock.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerLocalRole(APawn* InPawn)
{
	ENetRole LocalRole = InPawn->GetLocalRole();
	FString Role;

	switch (LocalRole)
	{
	case ROLE_None:
		Role = FString("None");
		break;
	case ROLE_SimulatedProxy:
		Role = FString("SimulatedProxy");
		break;
	case ROLE_AutonomousProxy:
		Role = FString("AutonomousProxy");
		break;
	case ROLE_Authority:
		Role = FString("Authority");
		break;
	case ROLE_MAX:
		Role = FString("MAX");
		break;
	}

	FString LocalRoleString = FString::Printf(TEXT("LocalRole - %s"), *Role);
	SetDisplayText(LocalRoleString);
}

void UOverheadWidget::ShowPlayerRemoteRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;

	switch (RemoteRole)
	{
	case ROLE_None:
		Role = FString("None");
		break;
	case ROLE_SimulatedProxy:
		Role = FString("SimulatedProxy");
		break;
	case ROLE_AutonomousProxy:
		Role = FString("AutonomousProxy");
		break;
	case ROLE_Authority:
		Role = FString("Authority");
		break;
	case ROLE_MAX:
		Role = FString("MAX");
		break;
	}

	FString RemoteRoleString = FString::Printf(TEXT("RemoteRole - %s"), *Role);
	SetDisplayText(RemoteRoleString);
}

void UOverheadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	RemoveFromParent();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}
