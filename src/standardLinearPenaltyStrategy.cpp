#include "standardLinearPenaltyStrategy.h"

double StandardLinearPenaltyStrategy::calculatePenalty(int daysLate) {
    if (daysLate <= 0) return 0;
    if (daysLate > 3) return 100;
    return 10.0 * daysLate;
}