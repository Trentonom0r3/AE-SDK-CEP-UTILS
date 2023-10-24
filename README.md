# AE-SDK-CEP-UTILS
A collection of various articles, projects, and my own work that demonstrates various sparsely documented features of both c++ plugins and CEP extensions for AE

## Extendscript ExternalObject

- [Hello World Example by Bryful](https://github.com/bryful/HelloWorld)
   - Simple ExternalObject Setup. Serves as a good "Skeleton"

- [FS Utils by Bryful](https://github.com/bryful/FsUtils)
  
- [This Article](https://qiita.com/MAA_/items/b1a35ab73af9f7b327e0)

## Misc

- [Misc AE Utils](https://github.com/bryful/AE_utils)


The files included in this repo currently demonstrate how to use the PlugPlug.Dll from the c++ side in order to send and receive csxs events. 
You can actually use the CSXS wrappers for any adobe app, provided you find a way to get the path to PlugPlug.Dll in your app.

You would then make sure the event listener is loaded whenever your plugin is initialized. 

So you'd want to include this;

https://github.com/Trentonom0r3/AE-SDK-CEP-UTILS/blob/main/AEGP/Grabba/Win/CSXSUtils.h

Make sure you add this to your project;

https://github.com/Trentonom0r3/AE-SDK-CEP-UTILS/blob/main/AEGP/Grabba/CSXUtils.cpp

and then the only other thing you would have to adjust would be this function in CSXUtils.cpp, basically just tell it what you want to listen for and what you want it to do when you hear it. It would also be a really good idea to add a condition to ignore whatever you're sending from the plugin itself, otherwise it'll get stuck in an infinite loop.

	```
	void MyEventListener(const Event* const event, void* const context) {
	    std::cout << "Received event: " << event->type << std::endl;
	
	    if (event->data) {
	        if (strcmp(event->data, "Hello from JSX!") == 0) {
	            std::cout << "Event data is correct." << std::endl;
	            int res = SendEvent("com.adobe.csxs.events.MyCustomEvent", "AEFT", "getimg", "Hello from C++");
	            if (res == kEventErrorCode_Success) {
	                std::cout << "Event sent successfully." << std::endl;
	            }
	            else {
	                std::cerr << "Failed to send event." << std::endl;
	            }
	        }
	        else {
	            std::cerr << "Event data is incorrect." << std::endl;
	        }
	
	    }
	}
	"""

I would make a wrapper around it in your main code, and return the strings from the event listener so you don't have to worry about including SDK headers or anything.
Finally, you can create an event listener like this;

	```
	char* RegisterEvent(const char* EventType)
	{
		std::string path;
		path = GetPlugPlugPath();
	
		LoadDLL(path);
	
		int result = RegisterEventListener(EventType);
	
		if (result == 1) {
			return "Success";
		}
		else {
			return "Error";
		}
	}
	```
