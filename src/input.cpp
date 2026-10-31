#include "input.h"
#include <cctype>
#include <cstdlib>
#include <iostream>
using namespace std;

string trim(string text) {
    int start = 0;
    int end = (int)text.size();
    while (start < end && isspace((unsigned char)text[start])) start++;
    while (end > start && isspace((unsigned char)text[end - 1])) end--;
    return text.substr(start, end - start);
}

string readLine(string prompt) {
    cout << prompt << ": ";
    string line;
    if (!getline(cin, line)) {       // the keyboard input was closed
        cout << "\nGoodbye.\n";
        exit(0);
    }
    return trim(line);
}

int readInt(string prompt) {
    while (true) {
        string text = readLine(prompt);
        try {
            size_t used = 0;
            int number = stoi(text, &used);
            if (used == text.size()) return number;
        } catch (...) {
        }
        cout << "Please type a whole number.\n";
    }
}

double readDouble(string prompt) {
    while (true) {
        string text = readLine(prompt);
        try {
            size_t used = 0;
            double number = stod(text, &used);
            if (used == text.size() && number > -1000000 && number < 1000000) return number;
        } catch (...) {
        }
        cout << "Please type a number.\n";
    }
}

bool readYesNo(string prompt) {
    string answer = readLine(prompt + " (y/n)");
    return answer == "y" || answer == "Y";
}