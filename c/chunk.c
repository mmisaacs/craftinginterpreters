//> Chunks of Bytecode chunk-c
#include <stdlib.h>

#include "chunk.h"
//> chunk-c-include-memory
#include "memory.h"
//< chunk-c-include-memory
//> Garbage Collection chunk-include-vm
#include "vm.h"
//< Garbage Collection chunk-include-vm

void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
//> chunk-null-lines
  chunk->lines = NULL;
//< chunk-null-lines
//> chunk-init-constant-array
  initValueArray(&chunk->constants);
//< chunk-init-constant-array
}
//> free-chunk
void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
//> chunk-free-lines
  FREE_ARRAY(int, chunk->lines, chunk->capacity);
//< chunk-free-lines
//> chunk-free-constants
  freeValueArray(&chunk->constants);
//< chunk-free-constants
  initChunk(chunk);
}
//< free-chunk
/* Chunks of Bytecode write-chunk < Chunks of Bytecode write-chunk-with-line
void writeChunk(Chunk* chunk, uint8_t byte) {
*/
//> write-chunk
//> write-chunk-with-line
void writeChunk(Chunk* chunk, uint8_t byte, int line) {
//< write-chunk-with-line
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code,
        oldCapacity, chunk->capacity);
//> write-chunk-line
    chunk->lines = GROW_ARRAY(int, chunk->lines,
        oldCapacity, chunk->capacity);
//< write-chunk-line
  }
  chunk->code[chunk->count] = byte;
  // check if we're on the same line
  if (chunk->lineCount > 0 && chunk->lines[chunk->lineCount - 1].line == line) {
    chunk->lines[chunk->lineCount - 1].count++;
  } else {
    // New line or first instruction: Add a new LineRun
    if (chunk->lineCapacity < chunk->lineCount + 1) {
      int oldCapacity = chunk->lineCapacity;
      chunk->lineCapacity = GROW_CAPACITY(oldCapacity);
      chunk->lines = GROW_ARRAY(LineRun, chunk->lines, oldCapacity, chunk->lineCapacity);
    }
    //> chunk-write-line
    chunk->lines[chunk->lineCount].line = line;
    chunk->lines[chunk->lineCount].count = 1;
    //< chunk-write-line
    chunk->lineCount++;
  }
}
//< write-chunk
//> add-constant
int addConstant(Chunk* chunk, Value value) {
//> Garbage Collection add-constant-push
  push(value);
//< Garbage Collection add-constant-push
  writeValueArray(&chunk->constants, value);
//> Garbage Collection add-constant-pop
  pop();
//< Garbage Collection add-constant-pop
  return chunk->constants.count - 1;
}
//< add-constant
int getLine(Chunk* chunk, int instructionIndex) {
  int offset = 0;

  for (int i = 0; i < chunk->lineCount; i++) {
    offset += chunk->lines[i].count;
    // If the combined count of all runs so far exceeds our index,
    // we've found the run containing this instruction.
    if (instructionIndex < offset) {
      return chunk->lines[i].line;
    }
  }
  return -1; // Should not happen if index is valid
}

void writeConstant(Chunk* chunk, Value value, int line) {
  int index = addConstant(chunk, value);

  if (index < 256) {
    // Standard path: 1-byte operand
    writeChunk(chunk, OP_CONSTANT, line);
    writeChunk(chunk, (uint8_t)index, line);
  } else {
    // Long path: 3-byte (24-bit) operand
    writeChunk(chunk, OP_CONSTANT_LONG, line);

    // We break the 24-bit integer into three 8-bit bytes
    // Using bit-shifting to isolate each byte:
    writeChunk(chunk, (uint8_t)(index & 0xff), line);         // Low byte
    writeChunk(chunk, (uint8_t)((index >> 8) & 0xff), line);  // Middle byte
    writeChunk(chunk, (uint8_t)((index >> 16) & 0xff), line); // High byte
  }
}
