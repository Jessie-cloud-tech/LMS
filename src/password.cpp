#include "password.h"
#include <sodium.h>
#include <stdexcept>
using namespace std;

string makeHash(string plainPassword) {
    if (sodium_init() < 0) throw runtime_error("Security library failed to start.");
    char result[crypto_pwhash_STRBYTES];
    int status = crypto_pwhash_str(result, plainPassword.c_str(), plainPassword.size(),
                                   crypto_pwhash_OPSLIMIT_INTERACTIVE,
                                   crypto_pwhash_MEMLIMIT_INTERACTIVE);
    if (status != 0) throw runtime_error("Password hashing failed.");
    return string(result);
}

bool passwordMatches(string storedHash, string plainPassword) {
    if (sodium_init() < 0) throw runtime_error("Security library failed to start.");
    return crypto_pwhash_str_verify(storedHash.c_str(), plainPassword.c_str(), plainPassword.size()) == 0;
}