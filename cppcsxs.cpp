// Enumerations for Event Scopes and Error Codes
enum EventScope {
	kEventScope_Global = 0,
	kEventScope_Application = 1,
	kEventScope_LastValue = 0x7FFFFFFF
};

enum EventErrorCode {
	kEventErrorCode_Success = 0,
	kEventErrorCode_OperationFailed = 1,
	kEventErrorCode_Unknown = 2,
	kEventErrorCode_LastValue = 0x7FFFFFFF
};

// Structure to represent an event
struct Event {
	const char* type;
	EventScope scope;
	const char* appId;
	const char* extensionId;
	const char* data;
};

// Function pointer definition for PlugPlugDispatchEvent
typedef int (*PlugPlugDispatchEventFn)(Event*);

// Function to dispatch an Adobe event from C++
EventErrorCode DispatchAdobeEvent(const char* type, const char* appId, const char* extensionId, const char* data, AEGP_SuiteHandler& suites) {
	try {
		// ExtendScript to get the path of the After Effects executable
		const A_char* myScript = R"(
            function get_path() {
                var startupFolder = Folder.startup;
                var startupFolderPath = startupFolder.fsName;
                var dll = "\\PlugPlug.dll";
                var path = startupFolderPath + dll;
                return path;
            }
            get_path();
        )";

		// Handles to store the script output and any error messages
		AEGP_MemHandle outResult = NULL;
		AEGP_MemHandle outErrorString = NULL;

		// Variable to hold the size of the memory handle
		AEGP_MemSize resultSize = 0;

		// Execute the ExtendScript
		A_Err err = suites.UtilitySuite6()->AEGP_ExecuteScript(
			S_my_id,
			myScript,
			false,
			&outResult,
			&outErrorString
		);

		// Check if the script executed successfully
		if (!err && outResult) {
			A_char* resultStr = NULL;

			// Lock the memory handle to read its content
			suites.MemorySuite1()->AEGP_GetMemHandleSize(outResult, &resultSize);
			suites.MemorySuite1()->AEGP_LockMemHandle(outResult, (void**)&resultStr);

			// If resultStr contains a valid path
			if (resultStr) {
				// Load the DLL dynamically
				HMODULE hModule = LoadLibrary(resultStr);

				// Unlock the memory handle
				suites.MemorySuite1()->AEGP_UnlockMemHandle(outResult);

				// Check if the DLL was loaded successfully
				if (!hModule) {
					std::cerr << "Failed to load DLL." << std::endl;
					return kEventErrorCode_OperationFailed;
				}

				// Get the address of the PlugPlugDispatchEvent function
				PlugPlugDispatchEventFn DispatchEvent = reinterpret_cast<PlugPlugDispatchEventFn>(GetProcAddress(hModule, "PlugPlugDispatchEvent"));

				// Create and populate the event structure
				Event my_event = {
					type,
					kEventScope_Application,
					appId,
					extensionId,
					data
				};

				// Dispatch the event
				EventErrorCode result = static_cast<EventErrorCode>(DispatchEvent(&my_event));

				// Free the loaded DLL
				FreeLibrary(hModule);

				return result;
			}
		}
		else {
			std::cerr << "Failed to execute script." << std::endl;
			return kEventErrorCode_OperationFailed;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
		return kEventErrorCode_OperationFailed;
	}
}

// Test function to demonstrate event dispatch
int test(AEGP_SuiteHandler& suites) {
	EventErrorCode result = DispatchAdobeEvent("com.adobe.csxs.events.MyCustomEvent", "AEFT", "com.test.cep.main", "This is a test to see if things work for", suites);
	if (result == kEventErrorCode_Success) {
		std::cout << "Event dispatched successfully." << std::endl;
	}
	else {
		std::cerr << "Failed to dispatch event. Error code: " << result << std::endl;
	}
	return 0;
}
