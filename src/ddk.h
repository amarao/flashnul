#ifndef _H_DDK_H
#define _H_DDK_H
/*requed defenition from DDK.
thanks to Stanislav Golovnin for adaptation microsoft sample of code to GCC.*/

//#pragma pack (push,4)
typedef enum _STORAGE_QUERY_TYPE {
    PropertyStandardQuery = 0,          // Retrieves the descriptor
    PropertyExistsQuery,                // Used to test whether the descriptor is supported
    PropertyMaskQuery,                  // Used to retrieve a mask of writeable fields in the descriptor
    PropertyQueryMaxDefined     // use to validate the value
} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;

//
// define some initial property id's
typedef enum _STORAGE_PROPERTY_ID {
    StorageDeviceProperty = 0,
    StorageAdapterProperty,
    StorageDeviceIdProperty
} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;

//
// Query structure - additional parameters for specific queries can follow
// the header
typedef struct _STORAGE_PROPERTY_QUERY {
    STORAGE_PROPERTY_ID PropertyId;	// ID of the property being retrieved
    STORAGE_QUERY_TYPE QueryType;	// Flags indicating the type of query being performed
    UCHAR AdditionalParameters[1]; 	// Space for additional parameters if necessary

} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;


typedef struct _STORAGE_ADAPTER_DESCRIPTOR {
    ULONG Version;
    ULONG Size;
    ULONG MaximumTransferLength;
    ULONG MaximumPhysicalPages;
    ULONG AlignmentMask;
    BOOLEAN AdapterUsesPio;
    BOOLEAN AdapterScansDown;
    BOOLEAN CommandQueueing;
    BOOLEAN AcceleratedTransfer;
    UCHAR BusType;
    USHORT BusMajorVersion;
    USHORT BusMinorVersion;
} STORAGE_ADAPTER_DESCRIPTOR, *PSTORAGE_ADAPTER_DESCRIPTOR;

#define CTL_CODE(t,f,m,a) (((t)<<16)|((a)<<14)|((f)<<2)|(m))

DEFINE_GUID( GUID_DEVCLASS_DISKDRIVE,           0x4d36e967L, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 );
DEFINE_GUID(GUID_DEVINTERFACE_DISK,             0x53f56307L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);

#ifndef STORAGE_BUS_TYPE	// MINGW
typedef enum _STORAGE_BUS_TYPE {
    BusTypeUnknown = 0x00,
    BusTypeScsi,
    BusTypeAtapi,
    BusTypeAta,
    BusType1394,
    BusTypeSsa,
    BusTypeFibre,
    BusTypeUsb,
    BusTypeRAID,
    BusTypeiScsi,
    BusTypeSas,
    BusTypeSata,
    BusTypeMaxReserved = 0x7F
} STORAGE_BUS_TYPE, *PSTORAGE_BUS_TYPE;
#endif

typedef struct _STORAGE_DEVICE_DESCRIPTOR {
    ULONG Version;		// Sizeof(STORAGE_DEVICE_DESCRIPTOR)

    // Total size of the descriptor, including the space for additional
    // data and id strings
    ULONG Size;
    UCHAR DeviceType;		// The SCSI-2 device type
    UCHAR DeviceTypeModifier;	// The SCSI-2 device type modifier (if any) - this may be zero

    // Flag indicating whether the device's media (if any) is removable.  This
    // field should be ignored for media-less devices
    BOOLEAN RemovableMedia;

    // Flag indicating whether the device can support mulitple outstanding
    // commands.  The actual synchronization in this case is the responsibility
    // of the port driver.
    BOOLEAN CommandQueueing;

    // Byte offset to the zero-terminated ascii string containing the device's
    // vendor id string.  For devices with no such ID this will be zero
    ULONG VendorIdOffset;

    // Byte offset to the zero-terminated ascii string containing the device's
    // product id string.  For devices with no such ID this will be zero
    ULONG ProductIdOffset;

    // Byte offset to the zero-terminated ascii string containing the device's
    // product revision string.  For devices with no such string this will be
    // zero
    ULONG ProductRevisionOffset;

    // Byte offset to the zero-terminated ascii string containing the device's
    // serial number.  For devices with no serial number this will be zero
    ULONG SerialNumberOffset;

    // Contains the bus type (as defined above) of the device.  It should be
    // used to interpret the raw device properties at the end of this structure
    // (if any)
    STORAGE_BUS_TYPE BusType;
    ULONG RawPropertiesLength;		// The number of bytes of bus-specific data which have been appended to this descriptor
    UCHAR RawDeviceProperties[1];	// Place holder for the first byte of the bus specific property data

} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;

typedef struct _STORAGE_HOTPLUG_INFO {

  UINT Size;
  UCHAR MediaRemovable;
  UCHAR MediaHotplug;
  UCHAR DeviceHotplug;
  UCHAR WriteCacheEnableOverride;
} STORAGE_HOTPLUG_INFO, 

 *PSTORAGE_HOTPLUG_INFO;


//#define FILE_DEVICE_MASS_STORAGE        0x0000002d
#define IOCTL_STORAGE_QUERY_PROPERTY   	CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_STORAGE_BASE 		FILE_DEVICE_MASS_STORAGE
#define IOCTL_STORAGE_GET_HOTPLUG_INFO ( (IOCTL_STORAGE_BASE << 16) | (FILE_ANY_ACCESS << 14) | (0x0305 << 2) | METHOD_BUFFERED )
#define FSCTL_ALLOW_EXTENDED_DASD_IO    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 32, METHOD_NEITHER,  FILE_ANY_ACCESS)
//#define FILE_ANY_ACCESS                 0
#define METHOD_BUFFERED                 0

#endif
