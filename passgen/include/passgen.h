#pragma once

static const char *LOWER = "abcdefghijklmnopqrstuvwxyz";
static const char *UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char *DIGITS = "0123456789";
static const char *SPECIAL = "!@#$%^&*()_+-=[]{};:'\",.<>/?";

int generatePassword(const char *alphabet, int length, char *out);

char* buildAlphabetFromSet(const char *set);
