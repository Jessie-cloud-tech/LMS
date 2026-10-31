#pragma once
#include "ipenaltyStrategy.h"

class StandardLinearPenaltyStrategy : public IPenaltyStrategy {
public:
    virtual double calculatePenalty(int daysLate);
};