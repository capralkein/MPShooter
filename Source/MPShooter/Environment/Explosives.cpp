#include "Explosives.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

#include "DrawDebugHelpers.h"

AExplosives::AExplosives()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
}

void AExplosives::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AExplosives::ReceiveDamage);
	}
}

void AExplosives::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//DrawDebugSphere(GetWorld(), GetActorLocation(), 100.f, 50, FColor::Blue);
	//DrawDebugSphere(GetWorld(), GetActorLocation(), 1000.f, 50, FColor::Red);
}

void AExplosives::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser)
{
	ServerDestroy(this);
}

void AExplosives::ServerDestroy_Implementation(AActor* ActorToDestroy)
{
	MulticastDestroy(ActorToDestroy);
}

void AExplosives::MulticastDestroy_Implementation(AActor* ActorToDestroy)
{
	if (ActorToDestroy)
	{
		AController* Controller = UGameplayStatics::GetPlayerController(this, 0);
	
		if (Controller)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this, // World context object 
				20, // BaseDamage 
				1.f, // MinimumDamage 
				GetActorLocation(), // Origin 
				100, // DamageInnerRadius 
				1000, // DamageOuterRadius 
				1.f, // DamageFalloff 
				UDamageType::StaticClass(), // DamageTypeClass 
				TArray<AActor*>(), // IgnoreActors 
				this, // DamageCauser 
				Controller // InstigatorController 
			);
		}

		ActorToDestroy->Destroy();
	}
}

void AExplosives::Destroyed()
{
	if (ExplosionCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionCue, GetActorLocation());
	}

	if (ExplosionParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticles,
			GetActorLocation()
		);
	}
}