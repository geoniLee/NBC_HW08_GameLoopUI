// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NBCCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
struct FInputActionValue;

UCLASS()
class NBC_LGI_RPANDMO_API ANBCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANBCCharacter();

#pragma region 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

	FTimerHandle SlowTimerHandle;
	FTimerHandle ReverseTimerHandle;
	FTimerHandle BlindTimerHandle;

#pragma endregion

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Debuff|Slow")
	void AddSlowTime(float time);

	UFUNCTION(BlueprintCallable, Category = "Debuff|Reverse")
	void AddReverseControllerTime(float time);

	UFUNCTION(BlueprintCallable, Category = "Debuff|Blind")
	void AddBlindTime(float time);

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);
	
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);
	
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	
	UFUNCTION()
	void StartSprint(const FInputActionValue& Value);
	
	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);

	void OnDeath();
	void UpdateOverheadHP();
	void UpdateMoveSpeed();
	void EndSlow();
	void EndReverse();
	void EndBlind();

#pragma region 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

#pragma endregion

private:
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SlowSpeedMultiplier;
	bool bIsReverseController = false;
};
