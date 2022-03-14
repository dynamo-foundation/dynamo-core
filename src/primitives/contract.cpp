
#include <primitives/contract.h>

CContract::CContract(std::string txnID, std::string hash, std::string iCode, std::string iCreatedBy, int64_t initialBalance, std::string address)

{
    contractTxnID = txnID;
    blockHash = hash;
    code = iCode;
    createdBy = iCreatedBy;
    balance = initialBalance;
    contractAddress = address;

    bCode = (unsigned char*)malloc(code.length() / 2);
    parseHex(code, (unsigned char*)bCode);
    programLength = code.length() / 2;

    uint64_t* stack = (uint64_t*)malloc(sizeof(uint64_t) * CONTRACT_STACK_SIZE);
    uint64_t* memory = (uint64_t*)malloc(sizeof(uint64_t) * CONTRACT_MEM_SIZE);

}

void CContract::execContract(std::string context)
{
    memset(stack, 0, sizeof(uint64_t) * CONTRACT_STACK_SIZE);
    memset(memory, 0, sizeof(uint64_t) * CONTRACT_MEM_SIZE);
    memset(registerFile, 0, sizeof(registerFile));

    int exception = EXCEPTION_NONE;
    bool executionComplete = false;
    int gas = 10000;
    zeroFlag = true;

    int programPointer = 0;

    //int source, destination;

    while ((exception == EXCEPTION_NONE) && (!executionComplete) && (gas > 0)) {

        unsigned char opcode = bCode[programPointer];
        if (opcode == OPCODE_MOVE) {
            programPointer += executeMOVE(programPointer, decodeSource(programPointer + 1), decodeDestination(programPointer + 1), &exception);
            gas--;
        }

        else if (opcode == OPCODE_ADD) {
            programPointer += executeADD(programPointer, decodeSource(programPointer + 1), decodeDestination(programPointer + 1), &exception);
            gas--;
        }

        else if (opcode == OPCODE_SUB) {
            programPointer += executeSUB(programPointer, decodeSource(programPointer + 1), decodeDestination(programPointer + 1), &exception);
            gas--;
        }

        else if (opcode == OPCODE_AND) {
            programPointer += executeAND(programPointer, decodeSource(programPointer + 1), decodeDestination(programPointer + 1), &exception);
            gas--;
        }

        else
            exception = EXCEPTION_INVALID_OPCODE;


        /*
        switch () {

            case OPCODE_MOVE:
            source = decodeSource(programPointer + 1);

                break;

            case OPCODE_PUSH:
                break;

            case OPCODE_POP:
                break;

            case OPCODE_ADD:
                break;

            case OPCODE_SUB:
                break;

            case OPCODE_AND:
                break;

            case OPCODE_SEND:
                break;

            case OPCODE_DYN:
                break;

            case OPCODE_SENDER:
                break;

            default:
                exception = EXCEPTION_INVALID_OPCODE;
        }
        */

    }

}


int CContract::executeMOVE(int programPointer, int source, int dest, int* exception) {

    if (dest == SOURCE_DEST_TYPE_IMMEDIATE) {
        *exception = EXCEPTION_INVALID_DEST;
        return 0;
    }

    programPointer += 2;

    uint64_t sourceVal = getSource(source, &programPointer, exception);

    if (*exception == EXCEPTION_NONE) {
        uint64_t* destPtr = getDest(dest, &programPointer, exception);
        if (*exception == EXCEPTION_NONE) {
            *destPtr = sourceVal;
            zeroFlag = (*destPtr == 0);
        }
    }

}


int CContract::executeADD(int programPointer, int source, int dest, int* exception) {

    if (dest == SOURCE_DEST_TYPE_IMMEDIATE) {
        *exception = EXCEPTION_INVALID_DEST;
        return 0;
    }

    programPointer += 2;

    uint64_t sourceVal = getSource(source, &programPointer, exception);

    if (*exception == EXCEPTION_NONE) {
        uint64_t* destPtr = getDest(dest, &programPointer, exception);
        if (*exception == EXCEPTION_NONE) {
            *destPtr = (*destPtr) + sourceVal;
            zeroFlag = (*destPtr == 0);
        }
    }

}


int CContract::executeSUB(int programPointer, int source, int dest, int* exception) {

    if (dest == SOURCE_DEST_TYPE_IMMEDIATE) {
        *exception = EXCEPTION_INVALID_DEST;
        return 0;
    }

    programPointer += 2;

    uint64_t sourceVal = getSource(source, &programPointer, exception);

    if (*exception == EXCEPTION_NONE) {
        uint64_t* destPtr = getDest(dest, &programPointer, exception);
        if (*exception == EXCEPTION_NONE) {
            *destPtr = (*destPtr) - sourceVal;
            zeroFlag = (*destPtr == 0);
        }
    }

}


int CContract::executeAND(int programPointer, int source, int dest, int* exception) {

    if (dest == SOURCE_DEST_TYPE_IMMEDIATE) {
        *exception = EXCEPTION_INVALID_DEST;
        return 0;
    }

    programPointer += 2;

    uint64_t sourceVal = getSource(source, &programPointer, exception);

    if (*exception == EXCEPTION_NONE) {
        uint64_t* destPtr = getDest(dest, &programPointer, exception);
        if (*exception == EXCEPTION_NONE) {
            *destPtr = (*destPtr) & sourceVal;
            zeroFlag = (*destPtr == 0);
        }
    }

}

uint64_t CContract::getSource(int source, int* programPointer, int* exception) {

    uint64_t result = 0;

    if (source == SOURCE_DEST_TYPE_IMMEDIATE) {
        if (*programPointer <= programLength - 8) {
            memcpy(&result, bCode + *programPointer, 8);
            (*programPointer) += 8;
        }
        else
            *exception = EXCEPTION_READ_PAST_ROM;
    }

    else if (source == SOURCE_DEST_TYPE_REGISTER) {
        if ((bCode[*programPointer] >= 0) && (bCode[*programPointer] < 8)) {       //technically dont need to test if > 0 because its all unsigned char
            result = registerFile[bCode[*programPointer]];
            (*programPointer)++;
        }
        else
            *exception = EXCEPTION_BAD_REGISTER_OPCODE;
    }

    else if (source == SOURCE_DEST_TYPE_MEMORY) {
        if (*programPointer <= programLength - 2) {
            uint16_t addr;
            memcpy(&addr, &bCode[*programPointer], 2);
            result = memory[addr];                          //dont need to check bounds because memory is limited to 64k
            (*programPointer) += 2;
        }
        else
            *exception = EXCEPTION_READ_PAST_ROM;
    }

    else if (source == SOURCE_DEST_TYPE_INDIRECT) {
        if ((bCode[*programPointer] >= 0) && (bCode[*programPointer] < 8)) {       //technically dont need to test if > 0 because its all unsigned char
            uint64_t index = registerFile[bCode[*programPointer]];
            index = index & 0x000000000000FFFF;
            result = memory[index];
            (*programPointer)++;
        }
        else
            *exception = EXCEPTION_BAD_REGISTER_OPCODE;

    }

    return result;
}


uint64_t* CContract::getDest(int dest, int* programPointer, int* exception) {

    uint64_t* result = NULL;

    if (dest == SOURCE_DEST_TYPE_IMMEDIATE) {
        if (*programPointer <= programLength - 8) {
            result = (uint64_t*)(&bCode + *programPointer);
            *programPointer += 8;
        }
        else
            *exception = EXCEPTION_READ_PAST_ROM;
    }

    else if (dest == SOURCE_DEST_TYPE_REGISTER) {
        if ((bCode[*programPointer] >= 0) && (bCode[*programPointer] < 8)) {       //technically dont need to test if > 0 because its all unsigned char
            result = &registerFile[bCode[*programPointer]];
            *programPointer++;
        }
        else
            *exception = EXCEPTION_BAD_REGISTER_OPCODE;
    }

    else if (dest == SOURCE_DEST_TYPE_MEMORY) {
        if (*programPointer <= programLength - 2) {
            uint16_t addr;
            memcpy(&addr, &bCode[*programPointer], 2);
            result = &memory[addr];                          //dont need to check bounds because memory is limited to 64k
            *programPointer += 2;
        }
        else
            *exception = EXCEPTION_READ_PAST_ROM;
    }

    else if (dest == SOURCE_DEST_TYPE_INDIRECT) {
        if ((bCode[*programPointer] >= 0) && (bCode[*programPointer] < 8)) {       //technically dont need to test if > 0 because its all unsigned char
            uint64_t index = registerFile[bCode[*programPointer]];
            index = index & 0x000000000000FFFF;
            result = &memory[index];
            *programPointer++;
        }
        else
            *exception = EXCEPTION_BAD_REGISTER_OPCODE;

    }

    return result;
}


int CContract::decodeSource(unsigned char opcode) {

    return (opcode & 0b00001100) >> 4;

}


int CContract::decodeDestination(unsigned char opcode) {
    return (opcode & 0b00000011);
}


void CContract::parseHex(std::string input, unsigned char* output)
{
    for (int i = 0; i < input.length(); i += 2) {
        unsigned char value = decodeHex(input[i]) * 16 + decodeHex(input[i + 1]);
        output[i / 2] = value;
    }
}

unsigned char CContract::decodeHex(char in)
{
    in = toupper(in);
    if ((in >= '0') && (in <= '9'))
        return in - '0';
    else if ((in >= 'A') && (in <= 'F'))
        return in - 'A' + 10;
    else
        return 0; //todo raise error
}
