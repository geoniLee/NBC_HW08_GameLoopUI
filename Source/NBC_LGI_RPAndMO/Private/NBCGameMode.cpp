// Fill out your copyright notice in the Description page of Project Settings.


#include "NBCGameMode.h"
#include "NBCCharacter.h"
#include "NBCPlayerController.h"
#include "NBCGameState.h"

ANBCGameMode::ANBCGameMode()
{
	// StaticClss = 객체를 생성하지 않고 클래스를 반환하는 역할(리플렉션)
	DefaultPawnClass = ANBCCharacter::StaticClass();
	// 플레이어 컨트롤러 할당
	PlayerControllerClass = ANBCPlayerController::StaticClass();
	GameStateClass = ANBCGameState::StaticClass();
}
