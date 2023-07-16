#ifndef COMMONLIB_H // this is a header guard
#define COMMONLIB_H

#include <Arduino.h>
#include <limits.h>
#include <ctype.h>

#define BAUD_RATE 115200

long long_pow(int base, int index);

void bounded_addition(int* currentVal, int maxVal, int increment, int minVal, bool ifCyclical);
#define CYCLICAL_NEXT(currentVal, maxVal) bounded_addition((int*)currentVal, maxVal, 1, 0, true)
#define CYCLICAL_PREV(currentVal, maxVal) bounded_addition((int*)currentVal, maxVal, -1, 0, true)
#define CAPPED_NEXT(currentVal, maxVal) bounded_addition((int*)currentVal, maxVal, 1, 0, false)
#define CAPPED_PREV(currentVal, maxVal) bounded_addition((int*)currentVal, maxVal, -1, 0, false)

//function overloading
void print_var_full(String variableName, String variableValue, String functionName);
void print_var_full(String variableName, float variableValue, String functionName);
void print_var_full(String variableName, double variableValue, String functionName);
void print_var_full(String variableName, int variableValue, String functionName);
void print_var_full(String variableName, long variableValue, String functionName);
void print_var_full(String variableName, unsigned long variableValue, String functionName);
void print_var_full(String variableName, bool variableValue, String functionName);
#define PRINT_VAR(variableName, variableValue) print_var_full(variableName, variableValue, "")

void print_class_name(String className);

#endif
