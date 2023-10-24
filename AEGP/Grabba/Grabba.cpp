/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2007-2023 Adobe Inc.                                  */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Inc. and its suppliers, if                    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Inc. and its                    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Inc.            */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/


/*	
	Grabba.cpp

	Answers the timeless question, "How do I get a rendered frame
	from After Effects?"
	

	version		notes							engineer		date
	
	1.0			First implementation			bbb				8/21/03
	
*/
#ifndef _WINSOCKAPI_
#include <boost/asio.hpp>
#endif
#include "Grabba.h"
#include <vector>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include "Win/CSXSUtils.h"




static AEGP_Command			getimg		=	6969L;
static AEGP_PluginID		S_my_id				=	0L;
static A_long				S_idle_count		=	0L;
static SPBasicSuite			*sP					=	NULL;


std::string GetPlugPlugPath() {
	AEGP_SuiteHandler	suites(sP);

		// Your ExtendScript as a string
		const A_char* myScript = R"(
			function get_path() {
				var startupFolder = Folder.startup;
				var startupFolderPath = startupFolder.fsName;
				var dll = "\\PlugPlug.dll";
				var path = startupFolderPath + dll;
				return path;
			}
			get_path();  // Call the function to get its return value
		)";

		AEGP_MemHandle outResult = NULL;     // To hold the return value from the script
		AEGP_MemHandle outErrorString = NULL; // To hold any error message
		AEGP_MemSize resultSize = 0;

		//AEGP_isscriptingavailable, returns bool value. Use this to check, and do a timeout loop if it's not available yet.
		A_Boolean isScriptingAvailable = FALSE;

		while (!isScriptingAvailable) {
			suites.UtilitySuite6()->AEGP_IsScriptingAvailable(&isScriptingAvailable);
			Sleep(100);
		}

		// Execute the script
		A_Err err = suites.UtilitySuite6()->AEGP_ExecuteScript(
			S_my_id,
			myScript,
			false,
			&outResult,
			&outErrorString
		);

		if (!err && outResult) {
			A_char* resultStr = NULL;
			suites.MemorySuite1()->AEGP_GetMemHandleSize(outResult, &resultSize);
			suites.MemorySuite1()->AEGP_LockMemHandle(outResult, (void**)&resultStr);
			suites.MemorySuite1()->AEGP_UnlockMemHandle(outResult);
			return resultStr;

}
else {
			return "Error";
		}
}

//Functions exposed to ExternalObjectInterface
extern "C" DllExport char* RegisterEvent(const char* EventType)
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


static A_Err
DeathHook(
	AEGP_GlobalRefcon	plugin_refconP,
	AEGP_DeathRefcon	refconP)
{
	A_Err	err			= A_Err_NONE;
	AEGP_SuiteHandler	suites(sP);

	A_char report[AEGP_MAX_ABOUT_STRING_SIZE] = {'\0'};
		
	suites.ANSICallbacksSuite1()->sprintf(report, STR(StrID_IdleCount), S_idle_count); 

	return err;
}

static	A_Err	
IdleHook(
	AEGP_GlobalRefcon	plugin_refconP,	
	AEGP_IdleRefcon		refconP,		
	A_long				*max_sleepPL)
{
	A_Err	err			= A_Err_NONE;
	S_idle_count++;

	return err;
}

static A_Err
UpdateMenuHook(
	AEGP_GlobalRefcon		plugin_refconPV,	/* >> */
	AEGP_UpdateMenuRefcon	refconPV,			/* >> */
	AEGP_WindowType			active_window)		/* >> */
{
	A_Err 				err 			=	A_Err_NONE,
						err2			=	A_Err_NONE;
		
	AEGP_ItemH			active_itemH	=	NULL;
	
	AEGP_ItemType		item_type		=	AEGP_ItemType_NONE;
	
	AEGP_SuiteHandler	suites(sP);

	ERR(suites.ItemSuite6()->AEGP_GetActiveItem(&active_itemH));

	if (active_itemH){
		ERR(suites.ItemSuite6()->AEGP_GetItemType(active_itemH, &item_type));
		
		if (AEGP_ItemType_COMP 		==	item_type	||
			AEGP_ItemType_FOOTAGE	==	item_type)	{
			ERR(suites.CommandSuite1()->AEGP_EnableCommand(getimg));
		}
	} else {
		ERR2(suites.CommandSuite1()->AEGP_DisableCommand(getimg));
	}					
	return err;
}

static A_Err
CommandHook(
	AEGP_GlobalRefcon	plugin_refconPV,		/* >> */
	AEGP_CommandRefcon	refconPV,				/* >> */
	AEGP_Command		command,				/* >> */
	AEGP_HookPriority	hook_priority,			/* >> */
	A_Boolean			already_handledB,		/* >> */
	A_Boolean* handledPB)				/* << */
{
	A_Err			err = A_Err_NONE,
		err2 = A_Err_NONE;

	AEGP_SuiteHandler	suites(sP);
	std::string path;
	path = GetPlugPlugPath();

	LoadDLL(path);

	int result = RegisterEventListener("com.adobe.csxs.events.MyCustomEvent");

	if (getimg == command) {

		AEGP_ItemH			active_itemH = NULL;
		AEGP_RenderOptionsH	roH = NULL;

		ERR(suites.ItemSuite6()->AEGP_GetActiveItem(&active_itemH));

		if (active_itemH) {
			ERR(suites.RenderOptionsSuite1()->AEGP_NewFromItem(S_my_id, active_itemH, &roH));

			if (!err && roH) {
				AEGP_FrameReceiptH	receiptH = NULL;
				AEGP_WorldH			frameH = NULL;
				A_Time				timeT = { 0,1 };
				AEGP_WorldType		type = AEGP_WorldType_NONE;
				AEGP_MemHandle		resultH = NULL,
					errH = NULL;
				A_Boolean			const platform_encodingB = FALSE;
				AEGP_MemHandle		scriptH = NULL;

				ERR(suites.RenderOptionsSuite1()->AEGP_SetTime(roH, timeT)); // avoid "div by 0"
				ERR(suites.RenderOptionsSuite1()->AEGP_GetWorldType(roH, &type));
				ERR(suites.RenderSuite2()->AEGP_RenderAndCheckoutFrame(roH, NULL, NULL, &receiptH));

				if (receiptH) {
					ERR(suites.RenderSuite2()->AEGP_GetReceiptWorld(receiptH, &frameH));

					if (frameH) {  // Check if frameH is not null
						AEGP_WorldType worldType;
						ERR(suites.WorldSuite3()->AEGP_GetType(frameH, &worldType));

						if (worldType && worldType == AEGP_WorldType_8) {  // Check if the world type is 8-bit
							A_long width = 0, height = 0;
							ERR(suites.WorldSuite3()->AEGP_GetSize(frameH, &width, &height));

							if (width > 0 && height > 0) {  // Check if width and height are valid
								A_u_long row_bytes = 0;
								ERR(suites.WorldSuite3()->AEGP_GetRowBytes(frameH, &row_bytes));

								PF_Pixel8* pixelData = nullptr;  // Adjusted type to match the SDK's method
								ERR(suites.WorldSuite3()->AEGP_GetBaseAddr8(frameH, &pixelData));  // Corrected method to obtain pixel data

								if (pixelData) {  // Additional check to ensure pixelData is not null
									unsigned char* imgdata = reinterpret_cast<unsigned char*>(pixelData);  // Adjusted type to match the SDK's method
									cv::Mat argb(height, width, CV_8UC4, imgdata);
									// Create a Mat for BGRA image
									cv::Mat bgra(height, width, CV_8UC4);

									// Channel reordering: ARGB to BGRA
									int fromTo[] = { 0, 3, 1, 2, 2, 1, 3, 0 }; // ARGB to BGRA: 0->3, 1->2, 2->1, 3->0
									cv::mixChannels(&argb, 1, &bgra, 1, fromTo, 4);

									// Encode the image into a buffer
									std::vector<uchar> buffer;

									cv::imencode(".png", bgra, buffer);  // Encode as PNG
														
								}
							}
						}
					}
					ERR2(suites.RenderSuite2()->AEGP_CheckinFrame(receiptH));
				}
				*handledPB = TRUE;

			}
			ERR(suites.RenderOptionsSuite1()->AEGP_Dispose(roH));
		}
		//return err;
	}
	return err;
}

A_Err
EntryPointFunc(
	struct SPBasicSuite* pica_basicP,		/* >> */
	A_long				 	major_versionL,		/* >> */
	A_long					minor_versionL,		/* >> */
	AEGP_PluginID			aegp_plugin_id,		/* >> */
	AEGP_GlobalRefcon* global_refconV)	/* << */
{
	S_my_id = aegp_plugin_id;
	A_Err 					err = A_Err_NONE,
		err2 = A_Err_NONE;

	sP = pica_basicP;

	AEGP_SuiteHandler suites(pica_basicP);

	ERR(suites.CommandSuite1()->AEGP_GetUniqueCommand(&getimg));

	ERR(suites.CommandSuite1()->AEGP_InsertMenuCommand(getimg,
		"getimg",
		AEGP_Menu_EXPORT,
		AEGP_MENU_INSERT_SORTED));

	ERR(suites.RegisterSuite5()->AEGP_RegisterCommandHook(S_my_id,
		AEGP_HP_BeforeAE,
		AEGP_Command_ALL,
		CommandHook,
		0));

	ERR(suites.RegisterSuite5()->AEGP_RegisterDeathHook(S_my_id, DeathHook, NULL));

	ERR(suites.RegisterSuite5()->AEGP_RegisterUpdateMenuHook(S_my_id, UpdateMenuHook, NULL));

	ERR(suites.RegisterSuite5()->AEGP_RegisterIdleHook(S_my_id, IdleHook, NULL));

	if (err) { // not !err, err!
		ERR2(suites.UtilitySuite3()->AEGP_ReportInfo(S_my_id, "Grabba : Could not register command hook."));
	}
	return err;
}
			  
			  
