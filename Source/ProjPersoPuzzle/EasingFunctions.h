#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EEasingFamily : uint8
{
	Linear   UMETA(DisplayName = "Linear"),
	Sine     UMETA(DisplayName = "Sine"),
	Quad     UMETA(DisplayName = "Quad"),
	Cubic    UMETA(DisplayName = "Cubic"),
	Quart    UMETA(DisplayName = "Quart"),
	Quint    UMETA(DisplayName = "Quint"),
	Expo     UMETA(DisplayName = "Expo"),
	Circ     UMETA(DisplayName = "Circ"),
	Back     UMETA(DisplayName = "Back"),
	Elastic  UMETA(DisplayName = "Elastic"),
	Bounce   UMETA(DisplayName = "Bounce"),
};

UENUM(BlueprintType)
enum class EEasingDirection : uint8
{
	In      UMETA(DisplayName = "In"),
	Out     UMETA(DisplayName = "Out"),
	InOut   UMETA(DisplayName = "In Out"),
};


struct FEasing
{
	// ── Linear ──────────────────────────────────────────────────
	static FORCEINLINE float Linear(float t) { return t; }

	// ── Sine ────────────────────────────────────────────────────
	static FORCEINLINE float SineIn   (float t) { return 1.f - FMath::Cos((t * PI) / 2.f); }
	static FORCEINLINE float SineOut  (float t) { return FMath::Sin((t * PI) / 2.f); }
	static FORCEINLINE float SineInOut(float t) { return -(FMath::Cos(PI * t) - 1.f) / 2.f; }

	// ── Quad ────────────────────────────────────────────────────
	static FORCEINLINE float QuadIn   (float t) { return t * t; }
	static FORCEINLINE float QuadOut  (float t) { return 1.f - (1.f - t) * (1.f - t); }
	static FORCEINLINE float QuadInOut(float t) { return t < 0.5f ? 2.f*t*t : 1.f - FMath::Pow(-2.f*t+2.f,2.f)/2.f; }

	// ── Cubic ───────────────────────────────────────────────────
	static FORCEINLINE float CubicIn   (float t) { return t*t*t; }
	static FORCEINLINE float CubicOut  (float t) { return 1.f - FMath::Pow(1.f-t, 3.f); }
	static FORCEINLINE float CubicInOut(float t) { return t < 0.5f ? 4.f*t*t*t : 1.f - FMath::Pow(-2.f*t+2.f,3.f)/2.f; }

	// ── Quart ───────────────────────────────────────────────────
	static FORCEINLINE float QuartIn   (float t) { return t*t*t*t; }
	static FORCEINLINE float QuartOut  (float t) { return 1.f - FMath::Pow(1.f-t, 4.f); }
	static FORCEINLINE float QuartInOut(float t) { return t < 0.5f ? 8.f*t*t*t*t : 1.f - FMath::Pow(-2.f*t+2.f,4.f)/2.f; }

	// ── Quint ───────────────────────────────────────────────────
	static FORCEINLINE float QuintIn   (float t) { return t*t*t*t*t; }
	static FORCEINLINE float QuintOut  (float t) { return 1.f - FMath::Pow(1.f-t, 5.f); }
	static FORCEINLINE float QuintInOut(float t) { return t < 0.5f ? 16.f*t*t*t*t*t : 1.f - FMath::Pow(-2.f*t+2.f,5.f)/2.f; }

	// ── Expo ────────────────────────────────────────────────────
	static FORCEINLINE float ExpoIn   (float t) { return t == 0.f ? 0.f : FMath::Pow(2.f, 10.f*t-10.f); }
	static FORCEINLINE float ExpoOut  (float t) { return t == 1.f ? 1.f : 1.f - FMath::Pow(2.f, -10.f*t); }
	static FORCEINLINE float ExpoInOut(float t)
	{
		if (t == 0.f) return 0.f; if (t == 1.f) return 1.f;
		return t < 0.5f ? FMath::Pow(2.f,20.f*t-10.f)/2.f : (2.f-FMath::Pow(2.f,-20.f*t+10.f))/2.f;
	}

	// ── Circ ────────────────────────────────────────────────────
	static FORCEINLINE float CircIn   (float t) { return 1.f - FMath::Sqrt(1.f - t*t); }
	static FORCEINLINE float CircOut  (float t) { return FMath::Sqrt(1.f - FMath::Pow(t-1.f, 2.f)); }
	static FORCEINLINE float CircInOut(float t)
	{
		return t < 0.5f
			? (1.f - FMath::Sqrt(1.f - FMath::Pow(2.f*t,2.f))) / 2.f
			: (FMath::Sqrt(1.f - FMath::Pow(-2.f*t+2.f,2.f)) + 1.f) / 2.f;
	}

	// ── Back ────────────────────────────────────────────────────
	static FORCEINLINE float BackIn   (float t) { const float c1=1.70158f,c3=c1+1.f; return c3*t*t*t - c1*t*t; }
	static FORCEINLINE float BackOut  (float t) { const float c1=1.70158f,c3=c1+1.f; return 1.f+c3*FMath::Pow(t-1.f,3.f)+c1*FMath::Pow(t-1.f,2.f); }
	static FORCEINLINE float BackInOut(float t)
	{
		const float c2 = 1.70158f * 1.525f;
		return t < 0.5f
			? FMath::Pow(2.f*t,2.f)*((c2+1.f)*2.f*t - c2) / 2.f
			: (FMath::Pow(2.f*t-2.f,2.f)*((c2+1.f)*(t*2.f-2.f)+c2)+2.f) / 2.f;
	}

	// ── Elastic ─────────────────────────────────────────────────
	static FORCEINLINE float ElasticIn(float t)
	{
		if (t==0.f) return 0.f; if (t==1.f) return 1.f;
		return -FMath::Pow(2.f,10.f*t-10.f) * FMath::Sin((t*10.f-10.75f)*(2.f*PI/3.f));
	}
	static FORCEINLINE float ElasticOut(float t)
	{
		if (t==0.f) return 0.f; if (t==1.f) return 1.f;
		return FMath::Pow(2.f,-10.f*t) * FMath::Sin((t*10.f-0.75f)*(2.f*PI/3.f)) + 1.f;
	}
	static FORCEINLINE float ElasticInOut(float t)
	{
		if (t==0.f) return 0.f; if (t==1.f) return 1.f;
		const float c5 = 2.f*PI/4.5f;
		return t < 0.5f
			? -(FMath::Pow(2.f,20.f*t-10.f) * FMath::Sin((20.f*t-11.125f)*c5)) / 2.f
			: (FMath::Pow(2.f,-20.f*t+10.f) * FMath::Sin((20.f*t-11.125f)*c5)) / 2.f + 1.f;
	}

	// ── Bounce ──────────────────────────────────────────────────
	static FORCEINLINE float BounceOut(float t)
	{
		const float n1=7.5625f, d1=2.75f;
		if      (t < 1.f/d1)   return n1*t*t;
		else if (t < 2.f/d1)   return n1*(t-=1.5f/d1)*t + 0.75f;
		else if (t < 2.5f/d1)  return n1*(t-=2.25f/d1)*t + 0.9375f;
		else                    return n1*(t-=2.625f/d1)*t + 0.984375f;
	}
	static FORCEINLINE float BounceIn   (float t) { return 1.f - BounceOut(1.f-t); }
	static FORCEINLINE float BounceInOut(float t)
	{
		return t < 0.5f ? (1.f - BounceOut(1.f-2.f*t))/2.f : (1.f + BounceOut(2.f*t-1.f))/2.f;
	}
	
	static float Evaluate(EEasingFamily _family, EEasingDirection _dir, float t)
	{
		t = FMath::Clamp(t, 0.f, 1.f);

		if (_family == EEasingFamily::Linear) return Linear(t);

		switch (_family)
		{
		case EEasingFamily::Sine:
			switch (_dir) { case EEasingDirection::In: return SineIn(t); case EEasingDirection::Out: return SineOut(t); default: return SineInOut(t); }
		case EEasingFamily::Quad:
			switch (_dir) { case EEasingDirection::In: return QuadIn(t); case EEasingDirection::Out: return QuadOut(t); default: return QuadInOut(t); }
		case EEasingFamily::Cubic:
			switch (_dir) { case EEasingDirection::In: return CubicIn(t); case EEasingDirection::Out: return CubicOut(t); default: return CubicInOut(t); }
		case EEasingFamily::Quart:
			switch (_dir) { case EEasingDirection::In: return QuartIn(t); case EEasingDirection::Out: return QuartOut(t); default: return QuartInOut(t); }
		case EEasingFamily::Quint:
			switch (_dir) { case EEasingDirection::In: return QuintIn(t); case EEasingDirection::Out: return QuintOut(t); default: return QuintInOut(t); }
		case EEasingFamily::Expo:
			switch (_dir) { case EEasingDirection::In: return ExpoIn(t); case EEasingDirection::Out: return ExpoOut(t); default: return ExpoInOut(t); }
		case EEasingFamily::Circ:
			switch (_dir) { case EEasingDirection::In: return CircIn(t); case EEasingDirection::Out: return CircOut(t); default: return CircInOut(t); }
		case EEasingFamily::Back:
			switch (_dir) { case EEasingDirection::In: return BackIn(t); case EEasingDirection::Out: return BackOut(t); default: return BackInOut(t); }
		case EEasingFamily::Elastic:
			switch (_dir) { case EEasingDirection::In: return ElasticIn(t); case EEasingDirection::Out: return ElasticOut(t); default: return ElasticInOut(t); }
		case EEasingFamily::Bounce:
			switch (_dir) { case EEasingDirection::In: return BounceIn(t); case EEasingDirection::Out: return BounceOut(t); default: return BounceInOut(t); }
		default:
			return t;
		}
	}
};
