#pragma once
#include <string>
using namespace std;

string makeHash(string plainPassword);
bool passwordMatches(string storedHash, string plainPassword);