struct FNewFloatMinMax
{
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Min = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Max = 1.0f;

	FNewFloatMinMax()
	{}
	FNewFloatMinMax(float InMin, float InMax)
	{}
};

namespace adian
{
	FVector_NetQuantize QuantizeVector(FVector& Vector)
	{
		FVector_NetQuantize Quantized;
		Quantized.X = Vector.X;
		Quantized.Y = Vector.Y;
		Quantized.Z = Vector.Z;
		return Quantized;
	}

	float QuantizedDistSquared(const FVector_NetQuantize& A, const FVector_NetQuantize& B)
	{
		const float DX = A.X - B.X;
		const float DY = A.Y - B.Y;
		const float DZ = A.Z - B.Z;

		return DX * DX + DY * DY + DZ * DZ;
	}

    bool CompareQuantize(FVector_NetQuantize& A, FVector_NetQuantize& B)
    {
        return A.X == B.X && A.Y == B.Y && A.Z == B.Z;
    }

	bool ComputeBarycentricTri(
		const FVector& P,
		const FVector& A,
		const FVector& B,
		const FVector& C,
		FVector& Out,
		float Tolerance)
	{
		if (Tolerance < 0.0f)
			return false;

		// AB = B - A
		const float ABx = B.X - A.X;
		const float ABy = B.Y - A.Y;
		const float ABz = B.Z - A.Z;

		// AC = C - A
		const float ACx = C.X - A.X;
		const float ACy = C.Y - A.Y;
		const float ACz = C.Z - A.Z;

		// Triangle normal = AB x AC
		const float Nx = ABy * ACz - ABz * ACy;
		const float Ny = ABz * ACx - ABx * ACz;
		const float Nz = ABx * ACy - ABy * ACx;

		const float TriNormSizeSq = Nx * Nx + Ny * Ny + Nz * Nz;
		if (TriNormSizeSq <= Tolerance)
			return false;

		const float AreaABCInv = Math::InvSqrt(TriNormSizeSq);

		// (B - P)
		const float BPx = B.X - P.X;
		const float BPy = B.Y - P.Y;
		const float BPz = B.Z - P.Z;

		// (C - P)
		const float CPx = C.X - P.X;
		const float CPy = C.Y - P.Y;
		const float CPz = C.Z - P.Z;

		// (B-P) x (C-P)
		const float Cross1X = BPy * CPz - BPz * CPy;
		const float Cross1Y = BPz * CPx - BPx * CPz;
		const float Cross1Z = BPx * CPy - BPy * CPx;

		const float AreaPBC =
			(Nx * Cross1X + Ny * Cross1Y + Nz * Cross1Z) * AreaABCInv;

		Out.X = AreaPBC * AreaABCInv;

		// (C-P) x (A-P)
		const float APx = A.X - P.X;
		const float APy = A.Y - P.Y;
		const float APz = A.Z - P.Z;

		const float Cross2X = CPy * APz - CPz * APy;
		const float Cross2Y = CPz * APx - CPx * APz;
		const float Cross2Z = CPx * APy - CPy * APx;

		const float AreaPCA =
			(Nx * Cross2X + Ny * Cross2Y + Nz * Cross2Z) * AreaABCInv;

		Out.Y = AreaPCA * AreaABCInv;

		Out.Z = 1.0f - Out.X - Out.Y;
		return true;
	}

	FVector ComputeBaryCentric2D(const FVector& Point, const FVector& A, const FVector& B, const FVector& C)
	{
		FVector Result(0.0f, 0.0f, 0.0f);
		ComputeBarycentricTri(
			Point,
			A,
			B,
			C,
			Result,
			SMALL_NUMBER);
		return Result;
	}
}