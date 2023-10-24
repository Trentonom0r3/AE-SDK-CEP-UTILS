
#include "./include/SoSharedLibDefs.h"
#include "HellowWorld.h"
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <fstream>  // For std::ofstream
#include <iostream>

//#define EXPORT __declspec(dllexport)


#pragma warning( push )
#pragma warning(disable : 4996) // Security warning about strcpy on win
#define strdup _strdup



namespace {
    /// ���̊g���@�\�ŗL�̃G�N�X�|�[�g�֐�����`
    char EXTENSION_FUNCTIONS[] = {
        "extGetAlertMessageDefault," /// �f�t�H���g���b�Z�[�W�擾
        "extGetAlertMessage_u,"      /// �ԍ��ɑΉ����郁�b�Z�[�W�擾
        "returnString,"              /// returns string--- Test.
    };

    constexpr long HELLO_WORLD_VERSION = 1;
} // namespace

////////////////////////
// �K�{�̃G�N�X�|�[�g�֐�
extern "C" {
    /// �o�[�W�����擾
    EXPORT long ESGetVersion() {
        return HELLO_WORLD_VERSION;
    }

    /// �g���@�\��������
    EXPORT char* ESInitialize(TaggedData*, long) {
        return EXTENSION_FUNCTIONS;
    }

    /// �g���@�\��j��
    EXPORT void ESTerminate() {
    }

    /// �������A���P�[�g
    EXPORT void* ESMallocMem(size_t size) {
        void* p = malloc(size);
        return p;
    }

    /// ���������
    EXPORT void ESFreeMem(void* p) {
        if (p != nullptr) {
            free(p);
        }
    }

} 
////////////////////////
// ���̊g���@�\�ŗL�̃G�N�X�|�[�g�֐�
extern "C" {

    EXPORT long returnString(TaggedData* argv, long argc, TaggedData* retval) {
        // Ensure no arguments are passed
        if (argc != 0) {
            return kESErrBadArgumentList;  // Return error code for bad argument list
        }

        // Prepare the string to be returned
        const char* message = "Hello, Adobe!";
        const size_t length = strlen(message) + 1;

        // Use ESMallocMem to allocate memory for the string
        char* buff = (char*)ESMallocMem(length);

        // Ensure memory allocation was successful
        if (buff == nullptr) {
            return kESErrNoMemory;  // Return error code for memory allocation failure
        }

        // Copy the message into the allocated buffer
        strcpy(buff, message);

        // Set the return value type and data
        retval->type = kTypeString;
        retval->data.string = buff;

        // Return success code
        return kESErrOK;
    }

    /// �f�t�H���g���b�Z�[�W�擾
    EXPORT long extGetAlertMessageDefault(TaggedData* inputData, long inputDataCount, TaggedData* outputData) {
        if (outputData == nullptr) {
            return kESErrNoLvalue; // Custom error code for null pointers
        }

        const char* message = "hello from ExtendScript.";

        // Memory allocation and string copy
        const auto length = strlen(message) + 1;
        char* str = (char*)malloc(length);
        if (str == nullptr) {
            return kESErrNoMemory; // Custom error code for memory allocation failures
        }

        strcpy_s(str, length, message);

        outputData->data.string = str;
        outputData->type = kTypeString;

        return kESErrOK;
    }

    /// �ԍ��ɑΉ����郁�b�Z�[�W�擾
    EXPORT long extGetAlertMessage(TaggedData* inputData, long inputDataCount, TaggedData* outputData) {
        const char* messages[] = {
            "Hello CEP.",
            "Hello World.",
            "Hello Native Extension.",
        };


        if (inputDataCount < 1) {
            return kESErrSyntax;
        }

        HMODULE hModule = GetModuleHandle(TEXT("C:\\Program Files\\Adobe\\Adobe After Effects 2023\\Support Files\\Plug-ins\\Effects\\Grabba.aex"));
        if (!hModule) {
            std::cerr << "Failed to load DLL." << std::endl;
            return NULL;
        }

        // get the function pointer, dll func name is 'RegisterEvent' and takes an argument of const char*
        typedef void(*RegisterEvent)(const char*);
        RegisterEvent registerEvent = (RegisterEvent)GetProcAddress(hModule, "RegisterEvent");
        if (!registerEvent) {
            std::cerr << "Failed to get function pointer." << std::endl;
            return NULL;
        }

        // call the function
        registerEvent("com.adobe.csxs.events.MyCustomEvent");

        char* str = nullptr;
        {
            const char* message = messages[inputData[0].data.intval];
            const auto length = strlen(message) + 1;
            str = (char*)malloc(length);
            strcpy_s(str, length, message);

        }
        outputData->data.string = str;
        outputData->type = kTypeString;

        return kESErrOK;
    }
} // ���̊g���@�\�ŗL�̃G�N�X�|�[�g�֐�

#if defined (_WINDOWS)
#pragma warning( pop )
#endif

