/*
    ===================== Comprehensive Guide for Modifying This Code =====================

    Introduction:
    This code serves as a template for creating an ExtendScript-compatible shared library. It includes methods for initializing,
    calling, and finalizing an ExtendScript object. This comprehensive guide aims to assist you in understanding, using,
    and modifying this code for your specific needs.

    --------------------- What You Should Modify ---------------------

    1. Method Definitions:
        - Location: Inside the 'objectInitialize' function, within the 'SoCClientName Methods[]' array.
        - What to do: Add the names of methods you'd like your object to expose.
        - Example: { "myNewMethod", 0, NULL }
        - How it works: This declares a new method named "myNewMethod" that can be called on instances of your object.

    2. Method Implementations:
        - Location: Inside the 'objectCall' function.
        - What to do: Implement the logic for your methods.
        - Example: Add a new 'else if' block for "myNewMethod" and define what should happen when it's called.
        - How it works: The 'objectCall' function is a dispatcher that routes method calls to their implementations.

    3. Instance Data:
        - Location: The 'myData_s' struct.
        - What to do: Add variables to hold any data specific to each instance of your object.
        - Example: int myState;
        - How it works: Each instance of your object will have its own 'myData_s' struct for storing instance-specific data.

    4. Initialization:
        - Location: Inside the 'objectInitialize' function.
        - What to do: Modify this function to initialize your object's state.
        - Example: If your object needs a configuration file, read it here.
        - How it works: This function is called once for each new instance of your object.

    5. Cleanup:
        - Location: Inside the 'objectFinalize' function.
        - What to do: Add any cleanup code necessary for when an object instance is destroyed.
        - Example: Free dynamically allocated resources.
        - How it works: This function is called once for each instance that is being destroyed.

    6. Object Properties:
        - Location: Inside the 'objectGet' and 'objectPut' functions.
        - What to do: If your object needs to expose properties, implement 'get' and 'put' functions.
        - Example: Add logic to retrieve or set a property.
        - How it works: Update 'SoObjectInterface' to include your 'get' and 'put' function pointers.

    --------------------- What You Should NOT Modify ---------------------

    1. Global Variables:
        - Example: 'gpObjectInterface', 'gpServer', etc.
        - Why: These are essential for the functioning of the shared library.

    2. Library Initialization and Termination:
        - Example: 'ESClientInterface' function and its inner logic.
        - Why: This is a standard setup for ExtendScript integration.

    3. Object Interface Declaration:
        - Example: 'SoObjectInterface objectInterface'.
        - Why: This is a standard declaration and should only be modified for very specific needs.

    --------------------- Naming Conventions ---------------------

    - Prefix instance-specific variables with 'pMyData->'.
    - Use camelCase for function names and variables.
    - Use PascalCase for type definitions and struct names.

    ========================================================================================
*/

// Required headers and macros based on your platform
// This part is platform-dependent. If you are on a Mac, the macro is defined differently.
#if MAC
#define unused(a) (void*) a ;
#else
void* unused(void* x) { return x; };
#endif

// Include necessary headers for the script engine and other functionalities
#include "./include/SoSharedLibDefs.h"  // Import shared library definitions
#include "include/SoCClient.h"
#include "samplelib.h"
#include <corecrt_malloc.h>
#include <vcruntime_string.h>
#include <string.h>

// Declare global variables to hold server and object interfaces
// These variables will be initialized later and used throughout the script.
SoObjectInterface* gpObjectInterface = NULL;
SoServerInterface* gpServer = NULL;
SoHServer          ghServer = NULL;
int                ME = 0;  // Counter to track the number of instances created

// Define a structure for instance-specific data
// This will hold data specific to each instance of the external object.
struct myData_s {
    char* additionalDllPath;  // Path to an additional DLL
    int   me;                 // A unique identifier for each instance
};
typedef struct myData_s  myData_t;
typedef        myData_t* myData_p;

// This function is called whenever a new instance of the external object is created.
// It initializes the object and sets up its methods and properties.
ESerror_t objectInitialize(SoHObject hObject, int argc, TaggedData* argv) {
    // Allocate memory for instance-specific data
    myData_p pMyData = (myData_p)malloc(sizeof(myData_t));
    memset(pMyData, 0, sizeof(myData_t));
    pMyData->me = ++ME;

    // Declare methods that this object will expose
    SoCClientName Methods[] = {
        { "useAdditionalDll", 0, NULL },
        { "newMethod", 0, NULL },  // Additional method
        { NULL }  // Null-terminator
    };

    // If a DLL path is provided as an argument during object creation,
    // store it in the instance-specific data.
    if (argc > 0 && argv[0].type == kTypeString) {
        pMyData->additionalDllPath = strdup(argv[0].data.string);
    }

    // Attach the instance-specific data to the object and declare its methods
    gpServer->setClientData(hObject, pMyData);
    gpServer->addMethods(hObject, Methods);

    return kESErrOK;
}

// Implement the methods that the object exposes.
// This function is called whenever a method on the object is invoked.
ESerror_t objectCall(SoHObject hObject, SoCClientName* name, int argc, TaggedData* argv, TaggedData* pResult) {
    // Retrieve the instance-specific data
    myData_p pMyData;
    gpServer->getClientData(hObject, (void**)&pMyData);

    /*
    else if(strcmp(name->name_sig, "myNewMethod") == 0) {
		// Implement the logic for your new method here
		// ...
	}
    */

    // Implement the 'useAdditionalDll' and 'newMethod' functionalities
    if (strcmp(name->name_sig, "useAdditionalDll") == 0) {
        pResult->type = kTypeString;
        pResult->data.string = strdup("Method useAdditionalDll called.");
    }
    else if (strcmp(name->name_sig, "newMethod") == 0) {
        pResult->type = kTypeString;
        pResult->data.string = strdup("New method called.");
    }
    else {
		// If the method is not implemented, return an error
		return kESErrNotImplemented;
	}

    return kESErrOK;
}

/*
    \brief Retrieves the value of a property of this object

    This function gets called when JavaScript code accesses a property of your object.
    You can implement custom logic to return the desired value.

    \param hObject - The reference for this instance
    \param name - The name of the property
    \param pResult - Pointer to the result being returned to JavaScript
*/
ESerror_t objectGet(SoHObject hObject, SoCClientName* name, TaggedData* pResult) {
    // Implement your property retrieval logic here
    // For example:
    if (strcmp(name->name_sig, "someProperty") == 0) {
        pResult->type = kTypeString;
        pResult->data.string = strdup("This is someProperty");
    }
    else {
        return kESErrNotImplemented;
    }
    return kESErrOK;
}

/*
    \brief Sets a value of a property of this object

    This function gets called when JavaScript code sets a property of your object.

    \param hObject - The reference for this instance
    \param name - The name of the property to update
    \param pValue - A pointer to the value to write to the property
*/
ESerror_t objectPut(SoHObject hObject, SoCClientName* name, TaggedData* pValue) {
    // Implement your property setting logic here
    // For example:
    if (strcmp(name->name_sig, "someProperty") == 0) {
        // Validate and set the property value
        // ...
    }
    else {
        return kESErrNotImplemented;
    }
    return kESErrOK;
}

/*
    \brief Called when JavaScript uses the 'valueOf' method on your object.

    \param hObject - The reference for this instance
    \param pResult - The return value to be passed back to JavaScript
*/
ESerror_t objectValueOf(SoHObject hObject, TaggedData* pResult) {
    // Implement your valueOf logic here
    pResult->type = kTypeString;
    pResult->data.string = strdup("valueOf: This is the value");
    return kESErrOK;
}

/*
    \brief Called when JavaScript uses the 'toString' method on your object.

    \param hObject - The reference for this instance
    \param pResult - The return value to be passed back to JavaScript
*/
ESerror_t objectToString(SoHObject hObject, TaggedData* pResult) {
    // Implement your toString logic here
    pResult->type = kTypeString;
    pResult->data.string = strdup("toString: This is the object");
    return kESErrOK;
}

// Cleanup code for each instance of the object.
// This function is called when an instance is destroyed.
ESerror_t objectFinalize(SoHObject hObject) {
    // Retrieve the instance-specific data
    myData_p pMyData;
    gpServer->getClientData(hObject, (void**)&pMyData);

    // Free any allocated memory
    if (pMyData) {
        if (pMyData->additionalDllPath) free(pMyData->additionalDllPath);
        free(pMyData);
    }
    return kESErrOK;
}

// Declare the object interface.
// These are the function pointers for the object's constructor, methods, and destructor.
SoObjectInterface objectInterface = {
    objectInitialize,  // constructor
    NULL,              // put (property setter)
    NULL,              // get (property getter)
    objectCall,        // call (method invocation)
    NULL,              // valueOf
    NULL,              // toString
    objectFinalize     // destructor
};

// Library Initialization and Termination
// This function is called when the script engine initializes or terminates the library.
extern "C" SAMPLIB int ESClientInterface(SoCClient_e kReason, SoServerInterface * pServer, SoHServer hServer) {
    // Initialize the object interface if it has not been initialized
    if (!gpObjectInterface) {
        gpObjectInterface = &objectInterface;
    }

    // Store the server handle for future use
    ghServer = hServer;

    // Switch based on the reason for the call (initialization or termination)
    switch (kReason) {
    case kSoCClient_init: {
        // Store the server interface for future use
        gpServer = pServer;

        // Add the object class to the server
        gpServer->addClass(hServer, "MyExternalObject", &objectInterface);
        return 0;
    }
    case kSoCClient_term: {
        // Reset the instance counter
        ME = 0;
        return 0;
    }
    }
    return 0;
}
