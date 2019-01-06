#include "InstancedLSystemFoliage.h"
#include "Components/ModelComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/BrushComponent.h"
#include "LindemayerFoliageType.h"
#include "LSystemFoliage.h"

bool FLSysPotentialInstance::PlaceInstance(const UWorld* InWorld, const ULSystemFoliageType* Settings, FLSysFoliageInstance& Inst, bool bSkipCollision)
{
	if (DesiredInstance.PlacementMode != ELSysPlacementMode::Procedural)
	{
		Inst.DrawScale3D = Settings->GetRandomScale();
	}
	else
	{
		//Procedural foliage uses age to get the scale
		Inst.DrawScale3D = FVector(Settings->GetScaleForAge(DesiredInstance.Age));
	}

	Inst.ZOffset = Settings->ZOffset.Interpolate(FMath::FRand());

	Inst.Location = HitLocation;

	if (DesiredInstance.PlacementMode != ELSysPlacementMode::Procedural)
	{
		// Random yaw and optional random pitch up to the maximum
		Inst.Rotation = FRotator(FMath::FRand() * Settings->RandomPitchAngle, 0.f, 0.f);

		if (Settings->RandomYaw)
		{
			Inst.Rotation.Yaw = FMath::FRand() * 360.f;
		}
		else
		{
			Inst.Flags |= LSYS_NoRandomYaw;
		}
	}
	else
	{
		Inst.Rotation = DesiredInstance.Rotation.Rotator();
		Inst.Flags |= LSYS_NoRandomYaw;
	}


	if (Settings->AlignToNormal)
	{
		Inst.AlignToNormal(HitNormal, Settings->AlignMaxAngle);
	}

	// Apply the Z offset in local space
	if (FMath::Abs(Inst.ZOffset) > KINDA_SMALL_NUMBER)
	{
		Inst.Location = Inst.GetInstanceWorldTransform().TransformPosition(FVector(0, 0, Inst.ZOffset));
	}

	UModelComponent* ModelComponent = Cast<UModelComponent>(HitComponent);
	if (ModelComponent)
	{
		ABrush* BrushActor = ModelComponent->GetModel()->FindBrush(HitLocation);
		if (BrushActor)
		{
			HitComponent = BrushActor->GetBrushComponent();
		}
	}

	return bSkipCollision || ALSystemFoliage::CheckCollisionWithWorld(InWorld, Settings, Inst, HitNormal, HitLocation, HitComponent);
}
