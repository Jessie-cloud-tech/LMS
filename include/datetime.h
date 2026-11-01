#pragma once
#include <string>
using namespace std;

long long nowSeconds();
string nowText();
string secondsToText(long long seconds);
long long toSeconds(string text);  

void setTestTime(long long seconds);   // will be used by automated tests: makes "now" a fixed time
void clearTestTime();                  // returns to the real clock