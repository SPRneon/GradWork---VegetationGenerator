// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
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
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditMove(bool bFinished) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//Components
	UPROPERTY()
	USceneComponent* Root = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SplineMesh = nullptr;
	UPROPERTY(EditAnywhere)
	int m_Generation = 1;
	UPROPERTY(VisibleAnywhere)
	ELSystemType m_Type = ELSystemType::PLANT;
	UPROPERTY(VisibleAnywhere)
	FString m_LString = "";

	/**Spline component*/
     UPROPERTY(EditAnywhere, Category = Lindenmayer)
     TArray<USplineComponent*> m_SplineComponents;
	/**SplineMesh component*/
     UPROPERTY(EditAnywhere, Category = Lindenmayer)
     TArray<USplineMeshComponent*> m_SplineMeshComponents;

	 
private:
	
	void CreateFoliageTypeInstance();
	void ResimulateTree();
	void CreateSplineMeshComponents(USplineComponent* spline);

	//Data
	
	FTransform m_Transform;
	FVector2D m_Angle = FVector2D(0.f,0.f);
	float m_GrowthDist = 0.0f;
	UTree* m_RootTree = nullptr;
	

	//Systems
	ULSystemTurtle* m_LSystemTurtle = nullptr;
	ULSystemGenerator* m_LSystemGenerator = nullptr;
};
