// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"

#include "Helpers.h"
#include "LSystemFoliage.generated.h"



class ULSystemTurtle;
class UTree;
class ULSystemGenerator;




UCLASS()
class TESTC_API ALSystemFoliage : public AActor
{
	GENERATED_BODY()	
public:	
	// Sets default values for this actor's properties
	ALSystemFoliage();
	void Initialize(ELSystemType type, int gen);
	TArray<USplineComponent*>& GetSplineComponentArray();
	void CreateSplines(UTree* tree);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostActorCreated() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//Components
	UPROPERTY()
	USceneComponent* Root = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh = nullptr;
	UPROPERTY(VisibleAnywhere)
	int m_Generation = 1;
	UPROPERTY(VisibleAnywhere)
	ELSystemType m_Type = ELSystemType::PLANT;
	UPROPERTY(VisibleAnywhere)
	FString m_LString = "";

	/**Spline component*/
     UPROPERTY(EditAnywhere, Category = Lindenmayer)
     TArray<USplineComponent*> m_SplineComponents;
private:
	
	void CreateFoliageTypeInstance();

	//Data
	
	FTransform m_Transform;
	FVector2D m_Angle = FVector2D(0.f,0.f);
	float m_GrowthDist = 0.0f;
	UTree* m_RootTree;
	

	//Systems
	ULSystemTurtle* m_LSystemTurtle = nullptr;
	ULSystemGenerator* m_LSystemGenerator = nullptr;
};
