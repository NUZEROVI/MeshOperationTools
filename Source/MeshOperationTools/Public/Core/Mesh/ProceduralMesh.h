//
//  ProceduralMesh.h
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralMesh.generated.h"

UCLASS()
class MESHOPERATIONTOOLS_API AProceduralMesh : public AActor
{
	GENERATED_BODY()
	
public:	

	AProceduralMesh();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	
public:
	// Attributes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MeshData")
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MeshData") 
	TArray<int> Indices; // Triangles Index
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MeshData") 
	TArray<FVector> Vertices; // Between 0 and 1 (Normalize)

	/* Initialization */
	void LoadMeshVertices();

private:
	// Attributes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UProceduralMeshComponent* ProceduralMesh;
};