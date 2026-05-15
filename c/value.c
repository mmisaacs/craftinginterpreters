//include files to add helping functions
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "object.h"
#include "memory.h"
#include "value.h"

void initValueArray(ValueArray* array) {
  array->values = NULL;
  array->capacity = 0;
  array->count = 0;
}
//> write-value-array
void writeValueArray(ValueArray* array, Value value) {
  if (array->capacity < array->count + 1) {
    int oldCapacity = array->capacity;
    array->capacity = GROW_CAPACITY(oldCapacity);
    array->values = GROW_ARRAY(Value, array->values,
                               oldCapacity, array->capacity);
  }
  
  array->values[array->count] = value;
  array->count++;
}
//< write-value-array
//> free-value-array
void freeValueArray(ValueArray* array) {
  FREE_ARRAY(Value, array->values, array->capacity);
  initValueArray(array);
}
//< free-value-array
//> print-value
void printValue(Value value) {
//> Optimization print-value
#ifdef NAN_BOXING
  if (IS_BOOL(value)) {
    printf(AS_BOOL(value) ? "true" : "false");
  } else if (IS_NIL(value)) {
    printf("nil");
  } else if (IS_NUMBER(value)) {
    printf("%g", AS_NUMBER(value));
  } else if (IS_OBJ(value)) {
    printObject(value);
  }
#else
//< Optimization print-value
/* Chunks of Bytecode print-value < Types of Values print-number-value
  printf("%g", value);
*/
/* Types of Values print-number-value < Types of Values print-value
 printf("%g", AS_NUMBER(value));
 */
//> Types of Values print-value
  switch (value.type) {
    case VAL_BOOL:
      printf(AS_BOOL(value) ? "true" : "false");
      break;
    case VAL_NIL: printf("nil"); break;
    case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
//> Strings call-print-object
    case VAL_OBJ: printObject(value); break;
//< Strings call-print-object
    case VAL_SHORT_STRING:
      fwrite(value.as.shortString.chars, sizeof(char),
             value.as.shortString.length, stdout);
      break;
  }
//< Types of Values print-value
//> Optimization end-print-value
#endif
//< Optimization end-print-value
}
//< print-value
//> Types of Values values-equal
bool valuesEqual(Value a, Value b) {
//> Optimization values-equal
#ifdef NAN_BOXING
//> nan-equality
  if (IS_NUMBER(a) && IS_NUMBER(b)) {
    return AS_NUMBER(a) == AS_NUMBER(b);
  }
//< nan-equality
  //helper function call to compare strings
  if (IS_STRING(a) && IS_STRING(b)) {
    return stringsEqual(a,b);
  }

#else
//< Optimization values-equal
  if (a.type != b.type) return false;
  switch (a.type) {
    case VAL_BOOL:   return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NIL:    return true;
    case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
/* Strings strings-equal < Hash Tables equal
    case VAL_OBJ: {
      ObjString* aString = AS_STRING(a);
      ObjString* bString = AS_STRING(b);
      return aString->length == bString->length &&
          memcmp(aString->chars, bString->chars,
                 aString->length) == 0;
    }
 */
//> Hash Tables equal
    case VAL_OBJ:    return AS_OBJ(a) == AS_OBJ(b);
//< Hash Tables equal
    case VAL_SHORT_STRING: return stringsEqual(a, b);
    default:         return false; // Unreachable.
  }
//> Optimization end-values-equal
#endif
//< Optimization end-values-equal
}
//< Types of Values values-equal
Value shortStringVal(int length, const char* chars) {
  Value value;
  value.type = VAL_SHORT_STRING;
  value.as.shortString.length = length;
  memcpy(value.as.shortString.chars, chars, length);
  return value;
}

int stringLength(Value value) {
  if (IS_SHORT_STRING(value)) return value.as.shortString.length;
  return AS_STRING(value)->length;
}

const char* stringChars(Value value) {
  if (IS_SHORT_STRING(value)) return value.as.shortString.chars;
  return AS_CSTRING(value);
}

bool stringsEqual(Value a, Value b) {
  int aLength = stringLength(a);
  int bLength = stringLength(b);

  if (aLength != bLength) return false;
  return memcmp(stringChars(a), stringChars(b), aLength) == 0;
}