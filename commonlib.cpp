#include "commonlib.h"

long long_pow(int base, int index) {
  return (long)(pow((float)base, (float)index) + 0.5);
}

void bounded_addition(int* currentVal, int maxVal, int increment, int minVal, bool ifCyclical) {
  int temp = *currentVal + increment;
  if (temp > maxVal) {
    temp = ifCyclical ? minVal : maxVal;
  } else if (temp < minVal) {
    temp = ifCyclical ? maxVal : minVal;
  }
  *currentVal = temp;
}

void print_var_full(String variableName, String variableValue, String functionName) {
  if (functionName.length() != 0) {
    Serial.print(functionName + "()--> ");
  }
  Serial.print(variableName + ": ");
  Serial.println(variableValue);
  Serial.flush();
  delay(5); //to prevent overloading Serial
}

void print_var_full(String variableName, float variableValue, String functionName) {
  print_var_full(variableName, String(variableValue, 3) + " (float, 3dp)", functionName); //round to 3 decimal places
}

void print_var_full(String variableName, double variableValue, String functionName) {
  print_var_full(variableName, String(variableValue, 3) + " (double, 3dp)", functionName); //round to 3 decimal places
}

void print_var_full(String variableName, int variableValue, String functionName) {
  print_var_full(variableName, String(variableValue) + " (int)", functionName);
}

void print_var_full(String variableName, long variableValue, String functionName) {
  print_var_full(variableName, String(variableValue) + " (long)", functionName);
}

void print_var_full(String variableName, unsigned long variableValue, String functionName) {
  print_var_full(variableName, String(variableValue) + " (unsigned long)", functionName);
}

void print_var_full(String variableName, bool variableValue, String functionName) {
  String string = variableValue ? "TRUE" : "FALSE";
  print_var_full(variableName, string + " (bool)", functionName);
}

void print_class_name(String className) {
  Serial.print("\n---");
  for (int i = 0; i < className.length(); i++) {
    className[i] = toupper(className[i]);
  }
  Serial.print(className);
  Serial.println(" INFORMATION---");
  Serial.flush();
  delay(5); //to prevent overloading Serial
}
