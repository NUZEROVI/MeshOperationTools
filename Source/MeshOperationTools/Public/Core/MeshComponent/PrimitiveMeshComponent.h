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
	FORCEINLINE AProceduralMesh* GetMesh() { return ProceduralMesh; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mesh Getters")
	FORCEINLINE TArray<UMeshVertex*> GetVertices() const { return MeshVertices; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mesh Getters")
	virtual TArray<UMeshTriangle*> GetTriangles() const { return MeshTriangles; }
	
	FORCEINLINE TMap<int32, TTuple<int32, int32, UMeshEdge*>> GetEdges() const { return MeshEdges; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mesh Getters")
	FORCEINLINE bool IsRemoveTriangle(UMeshTriangle* MeshTriangle) { return (MeshTriangles.Remove(MeshTriangle) == 1) ? true : false; }

	/* Setters */
	UFUNCTION(BlueprintCallable, Category = "Mesh Setters")
	void SetMesh(AProceduralMesh* Mesh) { ProceduralMesh = Mesh; }
	
	UFUNCTION(BlueprintCallable, Category = "Mesh Setters")
	void SetVertices(UMeshVertex* Vertex) { MeshVertices.Add(Vertex); }
	
	UFUNCTION(BlueprintCallable, Category = "Mesh Setters")
	virtual void SetTriangle(UMeshTriangle* MeshTriangle) { MeshTriangles.Add(MeshTriangle); }
	
	UFUNCTION(BlueprintCallable, Category = "Mesh Setters")
	void RemoveTriangle(UMeshTriangle* MeshTriangle) { MeshTriangles.Remove(MeshTriangle); }

	void SetEdges(int32 index, TTuple<int32, int32, UMeshEdge*> MeshEdge) { MeshEdges.Add(index, MeshEdge); }

	/* To Catch information about the clicked Mesh */
	UFUNCTION(BlueprintCallable, Category = "RuntimeMesh Methods")
	UMeshVertex* GetClosestVertexToLineSegment(FVector P, FVector Q, float SnapDist);
	
	UFUNCTION(BlueprintCallable, Category = "RuntimeMesh Methods")
	UMeshTriangle* GetClosestTriangleToLineSegment(FVector P, FVector Q, FVector& ClosestPoint, float& Dist, float& U, float& V);
	
	UFUNCTION(BlueprintCallable, Category = "RuntimeMesh Methods")
	UMeshVertex* GetVertexOnClosestEdge(UMeshTriangle* ClosestTri, FVector ClosestPoint, float SnapDist);
	
	UFUNCTION(BlueprintCallable, Category = "RuntimeMesh Methods")
	UMeshVertex* CreateVertexOnEdge(UMeshVertex* V0, UMeshVertex* V1, float T);
	
	UFUNCTION(BlueprintCallable, Category = "RuntimeMesh Methods")
	UMeshVertex* CreateVertexInTriangle(UMeshTriangle* MeshTri, float U, float V);

	// To Set Runtime Mesh return value
	int DragVertexIndex = -1; 
	bool IsOriVertices = false;
	int GetDragVertexIndex(){ return DragVertexIndex; }
	bool GetIsOriVertices() { return IsOriVertices; }
	
// Attributes
private:
	/* Initialized Mesh */
	AProceduralMesh* ProceduralMesh;

	/* Initialized Mesh Data */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<UMeshVertex*> MeshVertices;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<UMeshTriangle*> MeshTriangles;

	// TTuple<int32, int32, UMeshEdge*> Edge;
	TMap<int32, TTuple<int32, int32, UMeshEdge*>> MeshEdges;
	//TArray<TTuple<int32, int32, int32, UMeshEdge*>> _Edges;

	/* Methods */
	void RemoveMeshTriangle(UMeshTriangle* MeshTri);
	void RemoveTriangleFromEdge(int32 V0, int32 V1, UMeshTriangle* MeshTri);
	
	UMeshTriangle* CreateNewTriangle(int32 V0Index, int32 V1Index, int32 V2Index);
	void AddTriangleToEdge(UProcedureMeshVertexPair* MeshVertexPair, UMeshTriangle* MeshTriangle);
	void RemoveUnusedEdges();
};




