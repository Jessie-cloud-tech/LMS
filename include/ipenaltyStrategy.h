#pragma once
using namespace std;

class IPenaltyStrategy {
public:
    virtual ~IPenaltyStrategy();
    virtual double calculatePenalty(int daysLate) = 0;
};