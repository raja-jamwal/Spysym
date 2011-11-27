/*

    Spysym007 : malware kernel level execution preventer 
    (C) Raja Jamwal 2009, <www.experiblog.co.cc> 
    <linux1@zoho.com>

    Distributed under GNU GPL License

    Brainwave, is a pretty advance binaural beat generator program
    Copyright (C) 2009  Raja Jamwal

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

Module Name:

	main.c

Abstract:

	This is the main module for the Spysym 007 mini-filter.

Environment:

	Kernel mode
*/

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>

//------------------------------------------------------------
//
//	SPYSYM Structure Declaration
//
//------------------------------------------------------------
typedef struct _SPYSYM {
	//
	// The Object that identifies this driver.
	//
	
	PDRIVER_OBJECT DriverObject;

	//
	// The Filter handle that results from a call to
	// FltRegisterFilter.
	//

	PFLT_FILTER Filter;
	

} SPYSYM, *PSYMSYM;

// Structure that contains all the global declarations and
// variables used through out this driver

SPYSYM SpySym;


//
//  This is a static list of file name extensions files we are interested in
//

const UNICODE_STRING ScannerExtensionsToScan[] =
    { RTL_CONSTANT_STRING( L"ade"),
      RTL_CONSTANT_STRING( L"adp"),
      RTL_CONSTANT_STRING( L"bas"),
      RTL_CONSTANT_STRING( L"bat"),
      RTL_CONSTANT_STRING( L"chm"),
      RTL_CONSTANT_STRING( L"cmd"),
      RTL_CONSTANT_STRING( L"com"),
      RTL_CONSTANT_STRING( L"cpl"),
      RTL_CONSTANT_STRING( L"crt"),
      RTL_CONSTANT_STRING( L"dll"),
      RTL_CONSTANT_STRING( L"doc"),
      RTL_CONSTANT_STRING( L"docs"),
      RTL_CONSTANT_STRING( L"docx"),
      RTL_CONSTANT_STRING( L"exe"),
      RTL_CONSTANT_STRING( L"hlp"),
      RTL_CONSTANT_STRING( L"hta"),
      RTL_CONSTANT_STRING( L"inf"),
      RTL_CONSTANT_STRING( L"ins"),
      RTL_CONSTANT_STRING( L"isp"),
      RTL_CONSTANT_STRING( L"js"),
      RTL_CONSTANT_STRING( L"jse"),
      RTL_CONSTANT_STRING( L"lnk"),
      RTL_CONSTANT_STRING( L"mdb"),
      RTL_CONSTANT_STRING( L"mde"),
      RTL_CONSTANT_STRING( L"msc"),
      RTL_CONSTANT_STRING( L"msi"),
      RTL_CONSTANT_STRING( L"msp"),
      RTL_CONSTANT_STRING( L"mst"),
      RTL_CONSTANT_STRING( L"ocx"),
      RTL_CONSTANT_STRING( L"pcd"),
      RTL_CONSTANT_STRING( L"pif"),
      RTL_CONSTANT_STRING( L"pot"),
      RTL_CONSTANT_STRING( L"ppt"),
      RTL_CONSTANT_STRING( L"reg"),
      RTL_CONSTANT_STRING( L"scr"),
      RTL_CONSTANT_STRING( L"sct"),
      RTL_CONSTANT_STRING( L"shb"),
      RTL_CONSTANT_STRING( L"shs"),
      RTL_CONSTANT_STRING( L"sys"),
      RTL_CONSTANT_STRING( L"url"),
      RTL_CONSTANT_STRING( L"vb"),
      RTL_CONSTANT_STRING( L"vbe"),
      RTL_CONSTANT_STRING( L"vbs"),
      RTL_CONSTANT_STRING( L"wsc"),
      RTL_CONSTANT_STRING( L"wsf"),
      RTL_CONSTANT_STRING( L"wsh"),
      RTL_CONSTANT_STRING( L"xls"),
      {0, 0, NULL}
    };


//------------------------------------------------------------
//
//		Functions Prototypes
//
//------------------------------------------------------------

DRIVER_INITIALIZE DriverEntry;
NTSTATUS DriverEntry(__in PDRIVER_OBJECT DriverObject,
		     __in PUNICODE_STRING RegistryPath);

NTSTATUS DriverUnload(__in FLT_FILTER_UNLOAD_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS ScanPreCreate (
		__inout PFLT_CALLBACK_DATA Data,
		__in PCFLT_RELATED_OBJECTS FltObjects,
		__deref_out_opt PVOID *CompletionContext
		);

NTSTATUS
ScannerInstanceSetup (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_SETUP_FLAGS Flags,
    __in DEVICE_TYPE VolumeDeviceType,
    __in FLT_FILESYSTEM_TYPE VolumeFilesystemType
    );

//-------------------------------------------------------------
//
//		File Registration data filling,
// Current Operation include pre processing of IRP_MJ_CREATE
// Packet...No support for IRP_MJ_READ or IRP_MJ_WRITE..Other
// Packets are too currently unsupported..
//
//-------------------------------------------------------------

const FLT_OPERATION_REGISTRATION Callbacks[]={
	{ IRP_MJ_CREATE,
	  0,
	  ScanPreCreate,
	  NULL },
	{ IRP_MJ_OPERATION_END }
};

const FLT_REGISTRATION FilterRegistration = {
	
	sizeof( FLT_REGISTRATION ), 		// Size
	FLT_REGISTRATION_VERSION,		// Version
	0,					// Flags
	NULL,					// Context Registration
	Callbacks,				// Operation Callbacks
	DriverUnload,				// FilterUnload
	ScannerInstanceSetup,		   	// InstanceSetup
	NULL,					// InstanceQueryTeardown
	NULL,					// InstanceTeardownStart
	NULL,					// InstanceTeardownComplete
	NULL,					// GenerateFileName
	NULL,					// GenerateDestinationFileName
	NULL					// NormalizeNameComponent
};

/**************************************************************
	Filter Initialization routine
**************************************************************/

NTSTATUS DriverEntry(__in PDRIVER_OBJECT DriverObject,
		     __in PUNICODE_STRING RegistryPath)

/*

Routine Description:

	This is the initialization routine for the Spysym 007 mini
	filter file system filter driver. This registers the Spysym
	007 driver with FltMgr and initializes all its global data
	structures.

Arguments:
	DriverObject - Pointer to driver object created by the system
	to represent this driver.
	RegistryPath - Unicode string identifying where the parameters
	for this driver are located in the registry.

Return value:

	Returns STATUS_SUCCESS.
*/
{
	NTSTATUS status;
	UNREFERENCED_PARAMETER( RegistryPath);
	
	// Copy the driver object for global availability

	SpySym.DriverObject=DriverObject;	

	//
	//  Register with FltMgr
	//

	status = FltRegisterFilter( DriverObject,
				    &FilterRegistration,
				    &SpySym.Filter );
	
	if(!NT_SUCCESS( status )) {
		
		return status;
	}else{
		//
		// Start filtering I/O
		//
		
		status = FltStartFiltering( SpySym.Filter );

		if(NT_SUCCESS( status )){
		
			return STATUS_SUCCESS;
		}

	}

	FltUnregisterFilter( SpySym.Filter );	

	return status;
}


/**************************************************************
		Driver Unload Routine
**************************************************************/


NTSTATUS DriverUnload ( __in FLT_FILTER_UNLOAD_FLAGS Flags )

/*

Routine Description:

	This is the unload routine for the Filter driver. This unregisters
	the Filter with the filter manager and frees any allocated global
	data structures.

Arguments:

	None.

Return value:

	Returns the final status of the deallocation routines.

*/
{
	UNREFERENCED_PARAMETER( Flags );
	
	//
	// Unregister the filter
	//

	FltUnregisterFilter( SpySym.Filter );

	return STATUS_SUCCESS;
}




BOOLEAN
ScannerpCheckExtension (
    __in PUNICODE_STRING Extension
    )
/*++

Routine Description:

    Checks if this file name extension is something we are interested in

Arguments

    Extension - Pointer to the file name extension

Return Value

    TRUE - Yes we are interested
    FALSE - No
--*/
{
    const UNICODE_STRING *ext;

    if (Extension->Length == 0) {

        return FALSE;
    }

    //
    //  Check if it matches any one of our static extension list
    //

    ext = ScannerExtensionsToScan;

    while (ext->Buffer != NULL) {

        if (RtlCompareUnicodeString( Extension, ext, TRUE ) == 0) {

            //
            //  A match. We are interested in this file
            //

            return TRUE;
        }
        ext++;
    }

    return FALSE;
}




/*************************************************************
		ScanPreCreate Routine
*************************************************************/
	

FLT_PREOP_CALLBACK_STATUS ScanPreCreate (
		__inout PFLT_CALLBACK_DATA Data,
		__in PCFLT_RELATED_OBJECTS FltObjects,
		__deref_out_opt PVOID *CompletionContext
		)
/*

Routine Description:

	This is the routine which will be called before when an application
	tries to or use WINAPI CreateFile, and the Operating System
	send the IRP_MJ_CREATE packet to the file system driver.

Arguments:

	Data - The structure which describes the operation parameters.

	FltObject - The structure which describes the objects affected by this
		    Operation.

	CompletionContext - Output parameter which can be used to pass a context
		    from this pre-create callback to the post-create callback.

Return Value:

	FLT_PREOP_SUCCESS_WITH_CALLBACK - 
	FLT_PREOP_SUCCESS_NO_CALLBACK - 
	FLT_PREOP_COMPLETE - Returned when driver completed the I/O Operation
	and notifies the I/O Manager not to send the I/O Operation to any
	minifilter drivers below the driver stack or to the file system.

*/
{

    PFLT_FILE_NAME_INFORMATION nameInfo;
    NTSTATUS status;
    BOOLEAN scanFile;

    UNREFERENCED_PARAMETER( CompletionContext );
    UNREFERENCED_PARAMETER( FltObjects );

 
    //
    //  Check if we are interested in this file.
    //

    status = FltGetFileNameInformation( Data,
                                        FLT_FILE_NAME_NORMALIZED |
                                            FLT_FILE_NAME_QUERY_DEFAULT,
                                        &nameInfo );

    if (!NT_SUCCESS( status )) {

        return FLT_PREOP_SUCCESS_NO_CALLBACK;
    }

    FltParseFileNameInformation( nameInfo );

    //
    //  Check if the extension matches the list of extensions we are interested in
    //

    scanFile = ScannerpCheckExtension( &nameInfo->Extension );

    //
    //  Release file name info, we're done with it
    //

    FltReleaseFileNameInformation( nameInfo );

    if(scanFile==TRUE){
    Data->IoStatus.Status = STATUS_ACCESS_DENIED;
    Data->IoStatus.Information = 0;
    return FLT_PREOP_COMPLETE;
    }else{
          return FLT_PREOP_SUCCESS_NO_CALLBACK;
    }
}


/*************************************************************
		ScannerInstanceSetup Routine
*************************************************************/

NTSTATUS
ScannerInstanceSetup (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_SETUP_FLAGS Flags,
    __in DEVICE_TYPE VolumeDeviceType,
    __in FLT_FILESYSTEM_TYPE VolumeFilesystemType
    )
/*++

Routine Description:

    This routine is called by the filter manager when a new instance is created.
    We specified in the registry that we only want for manual attachments,
    so that is all we should receive here.

Arguments:

    FltObjects - Describes the instance and volume which we are being asked to
        setup.

    Flags - Flags describing the type of attachment this is.

    VolumeDeviceType - The DEVICE_TYPE for the volume to which this instance
        will attach.

    VolumeFileSystemType - The file system formatted on this volume.

Return Value:

  STATUS_SUCCESS              - we wish to attach to the volume
  STATUS_FLT_DO_NOT_ATTACH       - no, thank you

--*/
{
    
    PFLT_VOLUME_PROPERTIES pvp;
    ULONG returnLength; NTSTATUS status; 
    
    UNREFERENCED_PARAMETER( Flags );
    UNREFERENCED_PARAMETER( VolumeFilesystemType );

    PAGED_CODE();

    ASSERT( FltObjects->Filter == SpySym.Filter );
    
    //
    //  attach to mass storage volumes.
    //

    if (VolumeDeviceType == FILE_DEVICE_NETWORK_FILE_SYSTEM) {
	
	return STATUS_FLT_DO_NOT_ATTACH;
       
    }
    
    pvp = (PFLT_VOLUME_PROPERTIES) ExAllocatePool(NonPagedPool, sizeof(FLT_VOLUME_PROPERTIES)+512);
    status = FltGetVolumeProperties(FltObjects->Volume, pvp, sizeof(FLT_VOLUME_PROPERTIES)+512, &returnLength); 
    
    if(NULL == pvp){
    status = FltGetVolumeProperties(FltObjects->Volume, pvp, returnLength, &returnLength); 
    }
    
    if(!NT_SUCCESS(status)){
    ExFreePool(pvp);
    return STATUS_FLT_DO_NOT_ATTACH;
    }
    
    if( pvp->DeviceType != FILE_DEVICE_DISK_FILE_SYSTEM)
    {
    ExFreePool(pvp);
    return STATUS_FLT_DO_NOT_ATTACH;
    }
    
    if( !FlagOn(FILE_REMOVABLE_MEDIA, pvp->DeviceCharacteristics) ) { 
    
    DbgPrint(("The DeviceCharacteristics is not FILE_REMOVABLE_MEDIA\n")); 
    ExFreePool(pvp);
    return STATUS_FLT_DO_NOT_ATTACH; 
    } 
    
    DbgPrint("removable disk\n");
    
    
return STATUS_SUCCESS; 

}
