// Fill out your copyright notice in the Description page of Project Settings.


#include "Test_Pistol.h"

ATest_Pistol::ATest_Pistol()
{
	PrimaryActorTick.bCanEverTick = false;

	Pistol = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pistole"));

	static ConstructorHelpers::FObjectFinder<UParticleSystem>Fire(TEXT("ParticleSystem'/Game/Material/Pistol_Particle.Pistol_Particle'"));
	if (Fire.Succeeded())
	{
		FireParticle = Fire.Object;
	}


	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SOUND"));
	AudioComponent->SetupAttachment(Pistol);

	static ConstructorHelpers::FObjectFinder<USoundCue>FireSound(TEXT("SoundCue'/Game/Sound/Pistol-Sound/PistolOneShot_Sound.PistolOneShot_Sound'"));
	if (FireSound.Succeeded())
	{
		Fire_Sound = FireSound.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue>ReloadSound(TEXT("SoundCue'/Game/Sound/Pistol-Sound/PistolReload_Sound.PistolReload_Sound'"));
	if (ReloadSound.Succeeded())
	{
		Reload_Sound = ReloadSound.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue>EmptySound(TEXT("SoundCue'/Game/Sound/AK47_Sound/EmptySound.EmptySound'"));
	if (EmptySound.Succeeded())
	{
		Empty_Sound = EmptySound.Object;
	}
}


void ATest_Pistol::StartFire(const ACharacter* owner)
{
	
}

void ATest_Pistol::FireWithLineTrace(const ACharacter* owner)
{
	if (owner)
	{
		const AController* ownerController = owner->GetController();

		if (ownerController)
		{
			CurrentAmmos -= 1;
			PlayerHUD->SetAmmo(CurrentAmmos, MaxAmmos);
			owner->GetController()->GetPlayerViewPoint(Loc, Rot);

			const FVector start = Pistol->GetSocketLocation(TEXT("Muzzle"));
			const FVector end = Loc + (Rot.Vector() * ShotDistance);
			AudioComponent->SetSound(Fire_Sound);
			AudioComponent->Play();
			GameStatic->SpawnEmitterAttached(FireParticle, Pistol, FName("Muzzle"));
			FCollisionQueryParams collisionParams;
			collisionParams.AddIgnoredActor(this);
			DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 1.0f);
			if (GetWorld()->LineTraceSingleByChannel(
				hitResult,
				start,
				end,
				ECC_Visibility,
				collisionParams))
			{
				if (hitResult.GetActor())
				{
					auto* hitActor = hitResult.GetActor();
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Hit Actor Name: %s"), *hitActor->GetName()));
				}
			}
		}
	}
}



void ATest_Pistol::StopFire()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ATest_Pistol::Reloading() 
{
	AudioComponent->SetSound(Reload_Sound);
	AudioComponent->Play();
	GetWorld()->GetTimerManager().SetTimer(timer, FTimerDelegate::CreateLambda([&]()
		{
			// 내가 원하는 코드 구현
			MaxAmmos += -12 + CurrentAmmos;
			CurrentAmmos = 12;
			PlayerHUD->SetAmmo(CurrentAmmos, MaxAmmos);

		}), 2.3f, false); // 반복 실행을 하고 싶으면 false 대신 true 대입
}

void ATest_Pistol::EmptySound()
{
	AudioComponent->SetSound(Empty_Sound);
	AudioComponent->Play();
}
