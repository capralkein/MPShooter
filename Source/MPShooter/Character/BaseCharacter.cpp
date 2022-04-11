#include "BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MPShooter/Weapon/Weapon.h"
#include "Components/WidgetComponent.h"
#include "MPShooter/HUD/OverheadWidget.h"
#include "Net/UnrealNetwork.h"

#include "DrawDebugHelpers.h"



ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (WeaponClass == nullptr) return;

	if (IsLocallyControlled() && HasAuthority())
	{
		Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
	}

	if (Weapon)
	{
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("WeaponSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(Weapon, GetMesh());
		}
		Weapon->SetOwner(this);
		Weapon->SetPlayerCharacter(this);
	}

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABaseCharacter::ReceiveDamage);
	}

	UpdateHUDHealth();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseCharacter, Weapon);
	DOREPLIFETIME(ABaseCharacter, Health);
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABaseCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABaseCharacter::LookUp);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABaseCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABaseCharacter::FireButtonReleased);
}

void ABaseCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);

	if (Health <= 0) SetHealth(MaxHealth); // TODO

	UpdateHUDHealth();	
}

void ABaseCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void ABaseCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void ABaseCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABaseCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ABaseCharacter::FireButtonPressed() // TODO (auto fire)
{
	if (Weapon && IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		FVector HitTarget = HitResult.ImpactPoint;

		if (!HasAuthority())
		{
			Weapon->Fire(HitTarget);
		}
		ServerFire(HitTarget);
	}
}

void ABaseCharacter::FireButtonReleased()
{
	// TODO (For auto fire)
}

void ABaseCharacter::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void ABaseCharacter::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (IsLocallyControlled() && !HasAuthority()) return;

	if (Weapon)
	{
		Weapon->Fire(TraceHitTarget);
	}
}


void ABaseCharacter::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	FVector CrosshairWorldPosition, CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		FVector End = Start + CrosshairWorldDirection * 50000.f;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (!TraceHitResult.bBlockingHit) TraceHitResult.ImpactPoint = End;
	}
}

void ABaseCharacter::UpdateHUDHealth()
{
	if (OverheadWidget)
	{
		UOverheadWidget* WBP = Cast<UOverheadWidget>(OverheadWidget->GetUserWidgetObject());
		if (WBP)
		{
			int32 H = Health;
			WBP->SetDisplayText(FString::Printf(TEXT("%i"), H));
		}
	}
}

void ABaseCharacter::OnRep_Weapon()
{
	// TODO

	if (Weapon)
	{
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("WeaponSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(Weapon, GetMesh());
		}
		Weapon->SetOwner(this);
		Weapon->SetPlayerCharacter(this);
	}

	/*if (Weapon)
	{
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("WeaponSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(Weapon, GetMesh());
		}
		Weapon->SetOwner(this);
		Weapon->SetPlayerCharacter(this);
	}*/
}

void ABaseCharacter::OnRep_Health()
{
	UpdateHUDHealth();
	// PlayHitReactMontage
}