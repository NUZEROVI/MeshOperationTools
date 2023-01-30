//
//  PrimitiveMeshComponent.h
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/Mesh/ProceduralMesh.h"
#include "PrimitiveMeshComponent.generated.h"

class UMeshVertex;
class UMeshTriangle;
class UMeshEdge;
class UProcedureMeshVertexPair;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MESHOPERATIONTOOLS_API UPrimitiveMeshComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPrimitiveMeshComponent();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
/* Methods */	
public:
	// To Initialize the Mesh.
	UFUNCTION(BlueprintCallable, Category = "Mesh Initialized")
	void InitializedVertices(AProceduralMesh* Mesh, const int& Index, const FVector& CurrPos, const FVector& OriPos, const FVector2D& MeshUVs);
	
	UFUNCTION(BlueprintCallable, Category = "Mesh Initialized")
	UMeshTriangle* InitializedTriangle(int V0Index, int V1Index, int V2Index);
	
	UFUNCTION(BlueprintCallable, Category = "Mesh Initialized")
	void AddTriangleToEdge(AProceduralMesh* Mesh, UProcedureMeshVertexPair* MeshVertexPair, UMeshTriangle* MeshTriangle);
	
	/* Getters */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mesh Getters")
	FORCEINLINE TArray<UMeshVertex*> GetVertices() const { return _Vertices; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mesh Getters")
	FORCEINLINE TArray<UMeshTriangle*> GetTriangles() const { return _Triangles; }
	
	FORCEINLINE TArray<TTuple<int32, int32, int32, UMeshEdge*>> GetEdges() const { return _Edges; }


	/* Setters */
	UFUNCTION(BlueprintCallable, Category = "Mesh Setters")
	void SetVertices(UMeshVertex* MeshVertex) { _Vertices.Add(MeshVertex); }
	
	UFUNCTION(BlueprintCallable, Category = "Mesh Setters")
	void SetTriangle(UMeshTriangle* MeshTriangle) { _Triangles.Add(MeshTriangle); }

	void SetEdges(TTuple<int32, int32, int32, UMeshEdge*> Edge) { _Edges.Add(Edge); }
	
// Attributes
private:
	/* Initialized Mesh */
	AProceduralMesh* _Mesh;
	
	/* Initialized Mesh Component */
	UMeshVertex* PrimitiveMeshVertex;
	UMeshTriangle* PrimitiveMeshTriangle;
	UMeshEdge* PrimitiveMeshEdge;
	UProcedureMeshVertexPair* ProcedureMeshVertexPair;

	/* Initialized Mesh Data */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<UMeshVertex*> _Vertices;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<UMeshTriangle*> _Triangles;
	TArray<TTuple<int32, int32, int32, UMeshEdge*>> _Edges;
};


