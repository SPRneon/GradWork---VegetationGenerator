// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Misc/Guid.h"
#include "PhysicsEngine/BodyInstance.h"
#include "Helpers.h"
#include "Engine/EngineTypes.h"
#include "Components/PrimitiveComponent.h"
#include "Curves/CurveFloat.h"
#include "LindemayerFoliageType.generated.h"

// Function for filtering out hit components during FoliageTrace


class UFoliageType;

UENUM()
enum class ELSystemScaling : uint8
{
	/** Foliage instances will have uniform X,Y and Z scales. */
	Uniform,
	/** Foliage instances will have random X,Y and Z scales. */
	Free,
	/** Locks the X and Y axis scale. */
	LockXY,
	/** Locks the X and Z axis scale. */
	LockXZ,
	/** Locks the Y and Z axis scale. */
	LockYZ
};

UCLASS()
class ULSystemFoliageType : public UObject
{
	GENERATED_UCLASS_BODY()
	
	//virtual void Serialize(FArchive& Ar) override;

	virtual bool IsNotAssetOrBlueprint() const;

	FVector GetRandomScale() const;


#if WITH_EDITOR
	/* Lets subclasses decide if the InstancedFoliageActor should reallocate its instances if the specified property change event occurs */
	virtual bool IsFoliageReallocationRequiredForPropertyChange(struct FPropertyChangedEvent& PropertyChangedEvent) const { return true; }

	virtual void PreEditChange(UProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	/* Notifies all relevant foliage actors that HiddenEditorView mask has been changed */
	 void OnHiddenEditorViewMaskChanged(UWorld* InWorld);

	/* Nice and clean name for user interface */
	 FName GetDisplayFName() const;
#endif

	/* A GUID that is updated every time the foliage type is modified, 
	   so foliage placed in the level can detect the FoliageType has changed. */
	UPROPERTY()
	FGuid UpdateGuid;

	/** Custom collision for foliage */
	UPROPERTY(EditAnywhere, Category=InstanceSettings, meta=(HideObjectType=true))
	struct FBodyInstance BodyInstance;

public:

	// PAINTING

	/** Foliage instances will be placed at this density, specified in instances per 1000x1000 unit area */
	UPROPERTY(EditAnywhere, Category=Painting, meta=(DisplayName="Density / 1Kuu", UIMin=0, ClampMin=0))
	float Density;

	/** The factor by which to adjust the density of instances. Values >1 will increase density while values <1 will decrease it. */
	UPROPERTY(EditAnywhere, Category=Painting, meta=(UIMin=0, ClampMin=0, ReapplyCondition="ReapplyDensity"))
	float DensityAdjustmentFactor;

	/** The minimum distance between foliage instances */
	UPROPERTY(EditAnywhere, Category=Painting, meta=(UIMin=0, ClampMin=0, ReapplyCondition="ReapplyRadius"))
	float Radius;

	/** Specifies foliage instance scaling behavior when painting. */
	UPROPERTY(EditAnywhere, Category=Painting, meta=(ReapplyCondition="ReapplyScaling"))
	ELSystemScaling Scaling;

	/** Specifies the range of scale, from minimum to maximum, to apply to a foliage instance's X Scale property */
	UPROPERTY(EditAnywhere, Category=Painting, meta=(ClampMin="0.001", UIMin="0.001", ReapplyCondition="ReapplyScaleX"))
	FFloatInterval ScaleX;

	/** Specifies the range of scale, from minimum to maximum, to apply to a foliage instance's Y Scale property */
	UPROPERTY(EditAnywhere, Category=Painting, meta=(ClampMin="0.001", UIMin="0.001", ReapplyCondition="ReapplyScaleY"))
	FFloatInterval ScaleY;

	/** Specifies the range of scale, from minimum to maximum, to apply to a foliage instance's Z Scale property */
	UPROPERTY(EditAnywhere, Category=Painting, meta=(ClampMin="0.001", UIMin="0.001", ReapplyCondition="ReapplyScaleZ"))
	FFloatInterval ScaleZ;
public:

	//LINDENMAYER
	UPROPERTY(EditAnywhere, Category=Lindenmayer, meta=(DisplayName="Lindenmayer Type", ReapplyCondition="ReapplyLindenmayerType"))
	ELSystemType LSystemType;


	// PLACEMENT

	/** Specifies a range from minimum to maximum of the offset to apply to a foliage instance's Z location */
	UPROPERTY(EditAnywhere, Category=Placement, meta=(DisplayName="Z Offset", ReapplyCondition="ReapplyZOffset"))
	FFloatInterval ZOffset;

	/** Whether foliage instances should have their angle adjusted away from vertical to match the normal of the surface they're painted on 
	 *  If AlignToNormal is enabled and RandomYaw is disabled, the instance will be rotated so that the +X axis points down-slope */
	UPROPERTY(EditAnywhere, Category=Placement, meta=(ReapplyCondition="ReapplyAlignToNormal"))
	uint32 AlignToNormal:1;

	/** The maximum angle in degrees that foliage instances will be adjusted away from the vertical */
	UPROPERTY(EditAnywhere, Category=Placement, meta=(HideBehind="AlignToNormal"))
	float AlignMaxAngle;

	/** If selected, foliage instances will have a random yaw rotation around their vertical axis applied */
	UPROPERTY(EditAnywhere, Category=Placement, meta=(ReapplyCondition="ReapplyRandomYaw"))
	uint32 RandomYaw:1;

	/** A random pitch adjustment can be applied to each instance, up to the specified angle in degrees, from the original vertical */
	UPROPERTY(EditAnywhere, Category=Placement, meta=(ReapplyCondition="ReapplyRandomPitchAngle"))
	float RandomPitchAngle;

	/* Foliage instances will only be placed on surfaces sloping in the specified angle range from the horizontal */
	UPROPERTY(EditAnywhere, Category=Placement, meta=(UIMin=0, ReapplyCondition="ReapplyGroundSlope"))
	FFloatInterval GroundSlopeAngle;

	/* The valid altitude range where foliage instances will be placed, specified using minimum and maximum world coordinate Z values */
	UPROPERTY(EditAnywhere, Category=Placement, meta=(ReapplyCondition="ReapplyHeight"))
	FFloatInterval Height;

	/** If a layer name is specified, painting on landscape will limit the foliage to areas of landscape with the specified layer painted */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category=Placement, meta=(ReapplyCondition="ReapplyLandscapeLayers"))
	TArray<FName> LandscapeLayers;

	UPROPERTY()
	FName LandscapeLayer_DEPRECATED;
	
	/* If checked, an overlap test with existing world geometry is performed before each instance is placed */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category=Placement, meta=(ReapplyCondition="ReapplyCollisionWithWorld"))
	uint32 CollisionWithWorld:1;

	/* The foliage instance's collision bounding box will be scaled by the specified amount before performing the overlap check */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category=Placement, meta=(HideBehind="CollisionWithWorld"))
	FVector CollisionScale;

	/** Specifies the minimum value above which the landscape layer weight value must be, in order for foliage instances to be placed in a specific area */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category=Placement, meta=(HideBehind="LandscapeLayers"))
	float MinimumLayerWeight;

	UPROPERTY()
	FBoxSphereBounds MeshBounds;

	// X, Y is origin position and Z is radius...
	UPROPERTY()
	FVector LowBoundOriginRadius;

public:
	// PROCEDURAL

	 float GetSeedDensitySquared() const { return InitialSeedDensity * InitialSeedDensity; }
	 float GetMaxRadius() const;
	 float GetScaleForAge(const float Age) const;
	 float GetInitAge(FRandomStream& RandomStream) const;
	 float GetNextAge(const float CurrentAge, const int32 NumSteps) const;
	 bool GetSpawnsInShade() const;

	// COLLISION

	/** The CollisionRadius determines when two instances overlap. When two instances overlap a winner will be picked based on rules and priority. */
	UPROPERTY(Category=Procedural, EditAnywhere, meta=(Subcategory="Collision", ClampMin="0.0", UIMin="0.0"))
	float CollisionRadius;

	/** The ShadeRadius determines when two instances overlap. If an instance can grow in the shade this radius is ignored.*/
	UPROPERTY(Category=Procedural, EditAnywhere, meta=(Subcategory="Collision", ClampMin="0.0", UIMin="0.0"))
	float ShadeRadius;

	/** The number of times we age the species and spread its seeds. */
	UPROPERTY(Category=Procedural, EditAnywhere, meta=(Subcategory="Clustering", ClampMin="0", UIMin="0"))
	int32 NumSteps;

	// CLUSTERING

	/** Specifies the number of seeds to populate along 10 meters. The number is implicitly squared to cover a 10m x 10m area*/
	UPROPERTY(Category=Procedural, EditAnywhere, meta=(Subcategory="Clustering", ClampMin="0.0", UIMin="0.0"))
	float InitialSeedDensity;

	/** The average distance between the spreading instance and its seeds. For example, a tree with an AverageSpreadDistance 10 will ensure the average distance between the tree and its seeds is 10cm */
	UPROPERTY(Category=Procedural, EditAnywhere, meta=(Subcategory="Clustering", ClampMin="0.0", UIMin="0.0"))
	float AverageSpreadDistance;

	/** Specifies how much seed distance varies from the average. For example, a tree with an AverageSpreadDistance 10 and a SpreadVariance 1 will produce seeds with an average distance of 10cm plus or minus 1cm */
	UPROPERTY(Category=Procedural, EditAnywhere, meta=(Subcategory="Clustering", ClampMin="0.0", UIMin="0.0"))
	float SpreadVariance;

	/** The number of seeds an instance will spread in a single step of the simulation. */
	UPROPERTY(Category=Procedural, EditAnywhere, meta=(Subcategory="Clustering", ClampMin="0", UIMin="0"))
	int32 SeedsPerStep;

	/** The seed that determines placement of initial seeds. */
	UPROPERTY(Category=Procedural, EditAnywhere, meta=(Subcategory="Clustering"))
	int32 DistributionSeed;

	/** The seed that determines placement of initial seeds. */
	UPROPERTY(Category=Procedural, EditAnywhere, meta=(Subcategory="Clustering"))
	float MaxInitialSeedOffset;

	// GROWTH

	/** If true, seeds of this type will ignore shade radius during overlap tests with other types. */
	UPROPERTY(Category=Procedural, EditAnywhere, meta=(Subcategory="Growth"))
	bool bCanGrowInShade;

	/** 
	 * Whether new seeds are spawned exclusively in shade. Occurs in a second pass after all types that do not spawn in shade have been simulated. 
	 * Only valid when CanGrowInShade is true. 
	 */
	UPROPERTY(Category=Procedural, EditAnywhere, meta=(Subcategory="Growth", EditCondition="bCanGrowInShade"))
	bool bSpawnsInShade;

	/** Allows a new seed to be older than 0 when created. New seeds will be randomly assigned an age in the range [0,MaxInitialAge] */
	UPROPERTY(Category=Procedural, EditAnywhere, meta=(Subcategory="Growth", ClampMin="0.0", UIMin="0.0"))
	float MaxInitialAge;

	/** Specifies the oldest a seed can be. After reaching this age the instance will still spread seeds, but will not get any older*/
	UPROPERTY(Category=Procedural, EditAnywhere, meta=(Subcategory="Growth", ClampMin="0.0", UIMin="0.0"))
	float MaxAge;

	/** 
	 * When two instances overlap we must determine which instance to remove. 
	 * The instance with a lower OverlapPriority will be removed. 
	 * In the case where OverlapPriority is the same regular simulation rules apply.
	 */
	UPROPERTY(Category=Procedural, EditAnywhere, meta=(Subcategory="Growth"))
	float OverlapPriority;

	/** The scale range of this type when being procedurally generated. Configured with the Scale Curve. */
	UPROPERTY(Category=Procedural, EditAnywhere, meta = (Subcategory = "Growth", ClampMin = "0.001", UIMin = "0.001"))
	FFloatInterval ProceduralScale;

	/** 
	 * Instance scale factor as a function of normalized age (i.e. Current Age / Max Age).
	 * X = 0 corresponds to Age = 0, X = 1 corresponds to Age = Max Age.
	 * Y = 0 corresponds to Min Scale, Y = 1 corresponds to Max Scale.
	 */
	UPROPERTY(Category = Procedural, EditAnywhere, meta = (Subcategory = "Growth", XAxisName = "Normalized Age", YAxisName = "Scale Factor"))
	FRuntimeFloatCurve ScaleCurve;

	UPROPERTY()
	int32 ChangeCount;

	public:
	// REAPPLY EDIT CONDITIONS

	/** If checked, the density of foliage instances already placed will be adjusted by the density adjustment factor. */
	UPROPERTY(EditDefaultsOnly, Category=Reapply)
	uint32 ReapplyDensity:1;

	/** If checked, foliage instances not meeting the new Radius constraint will be removed */
	UPROPERTY(EditDefaultsOnly, Category=Reapply)
	uint32 ReapplyRadius:1;

	/** If checked, foliage instances will have their normal alignment adjusted by the Reapply tool */
	UPROPERTY(EditDefaultsOnly, Category=Reapply)
	uint32 ReapplyAlignToNormal:1;

	/** If checked, foliage instances will have their yaw adjusted by the Reapply tool */
	UPROPERTY(EditDefaultsOnly, Category=Reapply)
	uint32 ReapplyRandomYaw:1;

	/** If checked, foliage instances will have their scale adjusted to fit the specified scaling behavior by the Reapply tool */
	UPROPERTY(EditDefaultsOnly, Category=Reapply)
	uint32 ReapplyScaling:1;

	/** If checked, foliage instances will have their X scale adjusted by the Reapply tool */
	UPROPERTY(EditDefaultsOnly, Category=Reapply)
	uint32 ReapplyScaleX:1;

	/** If checked, foliage instances will have their Y scale adjusted by the Reapply tool */
	UPROPERTY(EditDefaultsOnly, Category=Reapply)
	uint32 ReapplyScaleY:1;

	/** If checked, foliage instances will have their Z scale adjusted by the Reapply tool */
	UPROPERTY(EditDefaultsOnly, Category=Reapply)
	uint32 ReapplyScaleZ:1;

	/** If checked, foliage instances will have their pitch adjusted by the Reapply tool */
	UPROPERTY(EditDefaultsOnly, Category=Reapply)
	uint32 ReapplyRandomPitchAngle:1;

	/** If checked, foliage instances not meeting the ground slope condition will be removed by the Reapply too */
	UPROPERTY(EditDefaultsOnly, Category=Reapply)
	uint32 ReapplyGroundSlope:1;

	/* If checked, foliage instances not meeting the valid Z height condition will be removed by the Reapply tool */
	UPROPERTY(EditDefaultsOnly, Category=Reapply)
	uint32 ReapplyHeight:1;

	/* If checked, foliage instances painted on areas that do not have the appropriate landscape layer painted will be removed by the Reapply tool */
	UPROPERTY(EditDefaultsOnly, Category=Reapply)
	uint32 ReapplyLandscapeLayers:1;

	/** If checked, foliage instances will have their Z offset adjusted by the Reapply tool */
	UPROPERTY(EditDefaultsOnly, Category=Reapply)
	uint32 ReapplyZOffset:1;

	/* If checked, foliage instances will have an overlap test with the world reapplied, and overlapping instances will be removed by the Reapply tool */
	UPROPERTY(EditDefaultsOnly, Category=Reapply)
	uint32 ReapplyCollisionWithWorld:1;

	/* If checked, foliage instances no longer matching the vertex color constraint will be removed by the Reapply too */
	UPROPERTY(EditDefaultsOnly, Category=Reapply)
	uint32 ReapplyVertexColorMask:1;

public:
	// SCALABILITY

	/**
	 * Whether this foliage type should be affected by the Engine Scalability system's Foliage scalability setting.
	 * Enable for detail meshes that don't really affect the game. Disable for anything important.
	 * Typically, this will be enabled for small meshes without collision (e.g. grass) and disabled for large meshes with collision (e.g. trees)
	 */
	UPROPERTY(EditAnywhere, Category=Scalability)
	uint32 bEnableDensityScaling:1;

private:

#if WITH_EDITORONLY_DATA
	// Deprecated since FFoliageCustomVersion::FoliageTypeCustomization
	UPROPERTY()
	float ScaleMinX_DEPRECATED;

	UPROPERTY()
	float ScaleMinY_DEPRECATED;

	UPROPERTY()
	float ScaleMinZ_DEPRECATED;

	UPROPERTY()
	float ScaleMaxX_DEPRECATED;

	UPROPERTY()
	float ScaleMaxY_DEPRECATED;

	UPROPERTY()
	float ScaleMaxZ_DEPRECATED;

	UPROPERTY()
	float HeightMin_DEPRECATED;

	UPROPERTY()
	float HeightMax_DEPRECATED;

	UPROPERTY()
	float ZOffsetMin_DEPRECATED;

	UPROPERTY()
	float ZOffsetMax_DEPRECATED;

	UPROPERTY()
	int32 StartCullDistance_DEPRECATED;
	
	UPROPERTY()
	int32 EndCullDistance_DEPRECATED;

	UPROPERTY()
	uint32 UniformScale_DEPRECATED:1;

	UPROPERTY()
	uint32 LockScaleX_DEPRECATED:1;

	UPROPERTY()
	uint32 LockScaleY_DEPRECATED:1;

	UPROPERTY()
	uint32 LockScaleZ_DEPRECATED:1;

	UPROPERTY()
	float GroundSlope_DEPRECATED;
	
	UPROPERTY()
	float MinGroundSlope_DEPRECATED;

	UPROPERTY()
	float MinScale_DEPRECATED;

	UPROPERTY()
	float MaxScale_DEPRECATED;
#endif// WITH_EDITORONLY_DATA
};
