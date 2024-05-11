// ADIAN Copyrighted


#include "HealthSystem.h"

float UHealthSystem::HealthRatio()
{
	if (MaxHealth == 0) return 0.0f; // Prevent division by zero
	return static_cast<float>(Health) / static_cast<float>(MaxHealth);
}
