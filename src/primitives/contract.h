#pragma once


#include <stdint.h>
#include <amount.h>
#include <sqlite/sqlite3.h>
#include "random.h"


#define CONTRACT_STACK_SIZE 1024
#define CONTRACT_MEM_SIZE 65536             //note if this changes then might need to check bounds on operands that use memory as a destination

#define EXCEPTION_NONE 0
#define EXCEPTION_INVALID_OPCODE 1          //opcode not a recognized instruction
#define EXCEPTION_INVALID_DEST   2          //destination is not valid for opcode (e.g. move with immediate destination)
#define EXCEPTION_READ_PAST_ROM  3          //attempt to read past end of program rom
#define EXCEPTION_BAD_REGISTER_OPCODE 4     //register number must be 0 to 7

#define OPCODE_MOVE 0x00
#define OPCODE_PUSH 0x01
#define OPCODE_POP 0x02
#define OPCODE_ADD 0x03
#define OPCODE_SUB 0x05
#define OPCODE_AND 0x07

#define OPCODE_SEND 0x22
#define OPCODE_DYN 0x27
#define OPCODE_SENDER 0x28

#define SOURCE_DEST_TYPE_IMMEDIATE 0
#define SOURCE_DEST_TYPE_REGISTER  1
#define SOURCE_DEST_TYPE_MEMORY    2
#define SOURCE_DEST_TYPE_INDIRECT  3

class CContract
{


public:

    std::string contractAddress;
    std::string contractTxnID;
    std::string blockHash;
    std::string code;           //hex
    std::string createdBy;
    int64_t balance;
    unsigned char* bCode;       //binary
    int programLength;

    uint64_t* stack;
    uint64_t* memory;
    uint64_t registerFile[8];

    bool zeroFlag;
 

    CContract(std::string txnID, std::string hash, std::string iCode, std::string iCreatedBy, int64_t initialBalance, std::string address);

    void execContract(std::string context);

    int executeMOVE(int programPointer, int source, int dest, int* exception);
    int executeADD(int programPointer, int source, int dest, int* exception);
    int executeSUB(int programPointer, int source, int dest, int* exception);
    int executeAND(int programPointer, int source, int dest, int* exception);

    uint64_t getSource(int source, int* programPointer, int* exception);
    uint64_t* getDest(int dest, int* programPointer, int* exception);

    int decodeSource(unsigned char opcode);
    int decodeDestination(unsigned char opcode);

    void CContract::parseHex(std::string input, unsigned char* output);
    unsigned char CContract::decodeHex(char in);


};



