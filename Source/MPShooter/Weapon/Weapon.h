#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"


UCLASS()
class MPSHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	void Fire(const FVector& HitTarget);

protected:
	virtual void BeginPlay() override;
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);


private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere)
	class ABaseCharacter* PlayerCharacter;

	UPROPERTY(EditAnywhere)
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

public:

	FORCEINLINE void SetPlayerCharacter(ABaseCharacter* Character) { PlayerCharacter = Character; }

};
