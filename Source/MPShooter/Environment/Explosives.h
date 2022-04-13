#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Explosives.generated.h"

UCLASS()
class MPSHOOTER_API AExplosives : public AActor
{
	GENERATED_BODY()
	
public:	
	AExplosives();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	UFUNCTION(Server, Reliable)
	void ServerDestroy(AActor* ActorToDestroy);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDestroy(AActor* ActorToDestroy);

private:
	UPROPERTY(VisibleAnywhere, Category = Defaults)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = Effect)
	class UParticleSystem* ExplosionParticles;

	UPROPERTY(EditAnywhere, Category = Effect)
	class USoundCue* ExplosionCue;


public:	
	
};
