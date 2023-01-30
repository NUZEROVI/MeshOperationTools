//
//  OperationTools.h
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#pragma once

#include "CoreMinimal.h"
#include "Core/Mesh/ProceduralMesh.h"
#include "GameFramework/Character.h"
#include "OperationTools.generated.h"

UCLASS()
class MESHOPERATIONTOOLS_API AOperationTools : public ACharacter
{
	GENERATED_BODY()

public:

	AOperationTools();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Operation Methods */
	void ForcepsToolOn(); // Grasping Mesh (Left-Click On)
	void ForcepsToolOff(); // Grasping Mesh (Left-Click Off)
	void ScalpelTool(); // Cutting Mesh
	
protected:
	/* Player Attributes */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Root Component")
    class UCapsuleComponent* CollisionCylinder;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* FirstPersonCameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* CameraArm; // SpringArm
    
    UPROPERTY(Category = "Movement Variables", BlueprintReadOnly)
    FRotator DefaultCameraRotation;
    
    UPROPERTY(Category = "Movement Variables", BlueprintReadOnly)
    float DefaultCameraArmLength;

	/* Mesh Attributes */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProceduralMesh")
	class AProceduralMesh* ProceduralMesh;
};
