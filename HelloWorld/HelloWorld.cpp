// Import necessary header files and libraries
#include "./include/SoSharedLibDefs.h"  // Import shared library definitions
#include "HellowWorld.h"  // Import HelloWorld header file
#include <stdlib.h>  // Import standard library for memory allocation and deallocation
#include <stdio.h>  // Import standard I/O library
#include <windows.h>  // Import Windows API library
#include <fstream>  // Import file stream library for file operations
#include <iostream>  // Import input/output stream library for console operations
#include "include/SoCClient.h"
#include "samplelib.h"

// Disable security warning about strcpy on Windows
#pragma warning( push )
#pragma warning(disable : 4996) 

// Alias for _strdup function for string duplication
#define strdup _strdup 

// Define a namespace to hold constant values and function signatures
namespace {
    // Array of extension function signatures
    char EXTENSION_FUNCTIONS[] = {
        "extGetAlertMessageDefault,"  // Function to get default alert message
        "extGetAlertMessage_u,"  // Function to get alert message with arguments
    };

    // Constant long integer to hold the version of HelloWorld
    constexpr long HELLO_WORLD_VERSION = 1;
} // namespace

#if MAC
#define unused(a) (void*) a ;
#else
void* unused(void* x) { return x; };
#endif

// Define external C block to hold memory management and utility functions
extern "C" {
    // Function to get version number
    EXPORT long ESGetVersion() {
        return HELLO_WORLD_VERSION;
    }

    // Function to initialize extension and return function signatures
    EXPORT char* ESInitialize(TaggedData*, long) {
        return EXTENSION_FUNCTIONS;
    }

    // Function to terminate extension (cleanup)
    EXPORT void ESTerminate() {
    }

    // Function to allocate memory
    EXPORT void* ESMallocMem(size_t size) {
        void* p = malloc(size);  // Allocate memory using malloc
        return p;  // Return pointer to allocated memory
    }

    // Function to deallocate memory
    EXPORT void ESFreeMem(void* p) {
        if (p != nullptr) {  // Check if pointer is not null
            free(p);  // Deallocate memory using free
        }
    }

}

// Define external C block to hold extension functions
extern "C" {
    // Newly added function to return a fixed string
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

    // Newly added function to return a string with arguments
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

    /// 番号に対応するメッセージ取得
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
} // この拡張機能固有のエクスポート関数




/**
* \brief Example of a function that returns a string
*
* If memory has not been allocated correctly then returns an out of memory error.
*
* \param argv - The JavaScript argument
* \param argc the argument count
* \param result The return value to be passed back to JavaScript
*/
extern "C" SAMPLIB long built(TaggedData * argv, long argc, TaggedData * result)
{
	int errval = kESErrOK;
	char sResult[1000];

	if (strlen(__DATE__) + strlen(__TIME__) < (sizeof(sResult) - 10)) {

		sResult[0] = 0;
		strcpy(sResult, __DATE__);
		strcat(sResult, " ");
		strcat(sResult, __TIME__);

		result->type = kTypeString;
		result->data.string = strdup(sResult);
		if (!result->data.string) errval = kESErrNoMemory;
	}
	else {
		errval = kESErrNoMemory;
	}

	unused(&argv);
	unused(&argc);

	return errval;
}

// ------------------------------------------------
// START SampleLibObject                          

// Statics

/**
	Array of function pointers to communiate use of the new class
*/
SoObjectInterface* gpObjectInterface = NULL;

/**
	Structure of function pointers which enable the client to call JavaScript
*/
SoServerInterface* gpServer = NULL;

/**
	The server object
*/
SoHServer			ghServer = NULL;

/**
	Counter for instances created
*/
int					ME = 0;

struct myData_s
{
	TaggedData	a; // value of property a
	TaggedData	b; // value of property b
	int			me;			// unique identifier for this instance
};

typedef struct myData_s  myData_t;
typedef        myData_t* myData_p;

// prototypes
ESerror_t objectInitialize(SoHObject hObject, int argc, TaggedData* argv);
ESerror_t objectGet(SoHObject hObject, SoCClientName* name, TaggedData* pResult);
ESerror_t objectPut(SoHObject hObject, SoCClientName* name, TaggedData* pValue);
ESerror_t objectCall(SoHObject hObject, SoCClientName* name, int argc, TaggedData* argv, TaggedData* pResult);
ESerror_t objectToString(SoHObject hObject, TaggedData* pResult);
ESerror_t objectValueOf(SoHObject hObject, TaggedData* pResult);
ESerror_t objectFinalize(SoHObject hObject);

/**
* \brief Called whenever a ne object is constructed
*
	\code
	var myObj = new SampleObject(param1);
	\endcode
*
* \param hObject - The reference for this instance
* \param argc - The argument count
* \param argv - The arguments passed in the constructor
*/
ESerror_t objectInitialize(SoHObject hObject, int argc, TaggedData* argv)
{

	// An array of SoCClientName structures representing 
	// the methods of this object
	SoCClientName Methods[] =
	{ {	"reverse"	, 0 , NULL	}
	,	{	"sine"		, 0 , NULL	}
	,   { NULL }
	};

	// An array of SoCClientName structures representing 
	// the properties of this object
	SoCClientName Properties[] =
	{ {	"a"			, 0 , NULL	}
	,	{	"b"			, 0 , NULL	}
	,	{	"pi"		, 0 , NULL	}
	,	{	"me"		, 0 , NULL	}
	,	{	"built"		, 0 , NULL	}
	,   {   NULL }
	};

	size_t   size = sizeof(myData_t);
	myData_p pMyData = NULL;

	unused(&argc);
	unused(argv);

	pMyData = (myData_p)(malloc(size));

	memset(pMyData, 0, size);
	pMyData->me = ++ME;
	gpServer->taggedDataInit(hObject, &pMyData->a);
	gpServer->taggedDataInit(hObject, &pMyData->b);
	gpServer->setClientData(hObject, pMyData);

	gpServer->addProperties(hObject, Properties);
	gpServer->addMethods(hObject, Methods);

	return kESErrOK;
}

/**
* \brief Retrieves the value of a property of this object
*
* \param hObject - The reference for this instance
* \param name - The name of the property
* \param pResult - Pointer to the result being returned to JavaScript
*/
ESerror_t objectGet(SoHObject hObject, SoCClientName* name, TaggedData* pResult)
{
	ESerror_t error = kESErrOK;

	myData_p pMyData = NULL;
	gpServer->getClientData(hObject, (void**)&pMyData);


	if (strcmp(name->name_sig, "a") == 0)
	{
		*pResult = pMyData->a;
		if (pResult->type == kTypeString) pResult->data.string = strdup(pResult->data.string);
	}

	else if (strcmp(name->name_sig, "b") == 0)
	{
		*pResult = pMyData->b;
		if (pResult->type == kTypeString) pResult->data.string = strdup(pResult->data.string);
	}

	else if (strcmp(name->name_sig, "pi") == 0)
	{
		pResult->type = kTypeDouble;
		pResult->data.fltval = atan2(1.0, 1.0) * 4.0;
	}

	else if (strcmp(name->name_sig, "built") == 0)
	{
		error = built(NULL, 0, pResult);
	}

	else if (strcmp(name->name_sig, "me") == 0)
	{
		pResult->type = kTypeInteger;
		pResult->data.intval = pMyData->me;
	}

	return error;
}

/**
* \brief Sets a value of a property of this object
*
* \param hObject - The reference for this instance
* \param name - The name of the property to update
* \param pValue - A pointer to the value to write to the property
*/
ESerror_t objectPut(SoHObject hObject, SoCClientName* name, TaggedData* pValue)
{
	myData_p    pMyData = NULL;

	gpServer->getClientData(hObject, (void**)&pMyData);

	if (strcmp(name->name_sig, "a") == 0)
	{
		pMyData->a = *pValue;
		if (pMyData->a.type == kTypeString) pMyData->a.data.string = strdup(pMyData->a.data.string);
	}
	else if (strcmp(name->name_sig, "b") == 0)
	{
		pMyData->b = *pValue;
		if (pMyData->b.type == kTypeString) pMyData->b.data.string = strdup(pMyData->b.data.string);
	}


	return 0;
}

/**
* \brief Calls a method of this object
*
* \param hObject - The reference for this instance
* \param name - The name of the method to call
* \param argv - The JavaScript argument
* \param argc - The argument count
* \param pResult - The return value to be passed back to JavaScript
*/
ESerror_t objectCall(SoHObject hObject, SoCClientName* name, int argc, TaggedData* argv, TaggedData* pResult)
{
	ESerror_t result = kESErrOK;
	int       bReverse = strcmp(name->name_sig, "reverse") == 0;
	int       bSine = strcmp(name->name_sig, "sine") == 0;

	int       type = argc == 1 ? argv[0].type : kTypeUndefined;

	unused(hObject);

	if (bSine) {

		if (argc != 1)
			result = kESErrBadArgumentList;
		else if (type != kTypeDouble && type != kTypeInteger)
			result = kESErrTypeMismatch;
		else
		{
			double      angle = type == kTypeDouble ? argv[0].data.fltval : (double)argv[0].data.intval;
			double      pi = 4.0 * atan2(1.0, 1.0);
			double      radian = 180.0 / pi;
			angle = angle / radian;
			pResult->data.fltval = sin(angle);
			pResult->type = kTypeDouble;
		}
	}

	else if (bReverse) {

		if (argc != 1)
			result = kESErrBadArgumentList;
		else if (type != kTypeString)
			result = kESErrTypeMismatch;
		else
		{
			size_t i;
			size_t l;
			pResult->type = kTypeString;
			pResult->data.string = strdup(argv[0].data.string);
			l = strlen(pResult->data.string);
			for (i = 0; i < l / 2; i++) {
				char t = pResult->data.string[i];
				pResult->data.string[i] = pResult->data.string[l - i - 1];
				pResult->data.string[l - i - 1] = t;
			}
		}
	}

	return result;
}

/**
 * Not used in this sample.  To implement, change from NULL to objectToString
 * in array of SoObjectInterface.
 *
 * \see objectInterface
*/
ESerror_t objectValueOf(SoHObject hObject, TaggedData* pResult)
{
	myData_p    pMyData = NULL;
	gpServer->getClientData(hObject, (void**)&pMyData);

	pResult->type = kTypeString;
	pResult->data.string = strdup("objectValueOf:: this is the value");
	return   kESErrOK;

}

/**
 * Not used in this sample.  To implement, change from NULL to objectToString
 * in array of SoObjectInterface.
 *
 * \see objectInterface
*/
ESerror_t objectToString(SoHObject hObject, TaggedData* pResult)
{
	myData_p    pMyData = NULL;
	gpServer->getClientData(hObject, (void**)&pMyData);

	pResult->type = kTypeString;
	pResult->data.string = strdup("objectToString::Object");
	return   kESErrOK;
}

/**
* \brief Calls when the object is released from memory.
*
* \param hObject - The reference for this instance
*/
ESerror_t objectFinalize(SoHObject hObject)
{
	myData_p pMyData = NULL;
	gpServer->getClientData(hObject, (void**)&pMyData);
	if (pMyData) {
		if (pMyData->a.type == kTypeString) free((void*)pMyData->a.data.string);
		if (pMyData->b.type == kTypeString) free((void*)pMyData->b.data.string);
		free(pMyData);
		gpServer->setClientData(hObject, NULL);
	}
	return kESErrOK;
}

/**
* Array of SoObjectInterface.
* Provides the interface which is used by JavaScript to
* communicate the use of the object
*/
SoObjectInterface objectInterface =
{ objectInitialize
,  objectPut
,  objectGet
,  objectCall
,  NULL  /* objectValueOf  */
,  NULL  /* objectToString */
,  objectFinalize
};

/* ESClientInterface - optional call to support ExternalObject Object Interface */
extern "C" SAMPLIB int  ESClientInterface(SoCClient_e kReason, SoServerInterface * pServer, SoHServer hServer)
{
	if (!gpObjectInterface) {
		gpObjectInterface = &objectInterface;
	}
	ghServer = hServer;

	switch (kReason)
	{
	case kSoCClient_init: {
		gpServer = pServer;
		ghServer = hServer;

		gpServer->addClass(hServer, "SampleLibObject", &objectInterface);
		return 0;
	} break;

	case kSoCClient_term: {
		ME = 0;
		return 0;
	} break;
	}

	return 0;
}

#if defined (_WINDOWS)
#pragma warning( pop )
#endif

