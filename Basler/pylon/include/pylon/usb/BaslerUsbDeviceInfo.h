//-----------------------------------------------------------------------------
//  Basler pylon SDK
//  Copyright (C) 2011-2013 Basler
//  http://www.baslerweb.com
//  Author:  AH
//-----------------------------------------------------------------------------
/*!
\file
\brief    Definition of the CBaslerUsbDeviceInfo class
*/

#ifndef INCLUDED_BASLERUSBDEVICEINFO_H_8532831
#define INCLUDED_BASLERUSBDEVICEINFO_H_8532831

#include <pylon/Platform.h>

#ifdef _MSC_VER
#   pragma pack(push, PYLON_PACKING)
#endif /* _MSC_VER */

#include <pylon/DeviceInfo.h>
#include <pylon/usb/PylonUsbDefs.h>

namespace Pylon
{
    namespace Key
    {
        const char* const DeviceGUIDKey( "DeviceGUID" );
        const char* const ManufacturerInfoKey( "ManufacturerInfo" );
        const char* const DeviceIdxKey( "DeviceIdx" );
        const char* const VendorIdKey( "VendorId" );
        const char* const ProductIdKey( "ProductId" );
        const char* const DriverKeyNameKey( "DriverKeyName" );
        const char* const UsbDriverTypeKey( "UsbDriverTypeName" );
        // Internal use only
        const char* const UsbPortVersionBcdKey( "UsbPortVersionBcd" );
        // Internal use only
        const char* const SpeedSupportBitmaskKey( "SpeedSupportBitmask" );
    }


    /*!
    \ingroup Pylon_TransportLayer
    \class CBaslerUsbDeviceInfo
    \brief PylonUsb specific Device Info object.
    */
    class PYLONUSB_API CBaslerUsbDeviceInfo : public Pylon::CDeviceInfo
    {
    public:
        CBaslerUsbDeviceInfo();
        CBaslerUsbDeviceInfo(const CDeviceInfo& di);

        ///Retrieves the device GUID.
        ///This property is identified by Key::DeviceGUID.
        String_t GetDeviceGUID() const;
        ///Returns true if the above property is available.
        bool IsDeviceGUIDAvailable() const;

        ///Retrieves the manufacturer info.
        ///This property is identified by Key::ManufacturerInfo.
        String_t GetManufacturerInfo() const;
        ///Returns true if the above property is available.
        bool IsManufacturerInfoAvailable() const;

        ///Retrieves the device index. For internal use only.
        ///This property is identified by Key::DeviceIdx.
        String_t GetDeviceIdx() const;
        ///Returns true if the above property is available.
        bool IsDeviceIdxAvailable() const;

        ///Retrieves the product ID. For internal use only.
        ///This property is identified by Key::DeviceIdx.
        String_t GetProductId() const;
        ///Returns true if the above property is available.
        bool IsProductIdAvailable() const;

        ///Retrieves the vendor ID. For internal use only.
        ///This property is identified by Key::DeviceIdx.
        String_t GetVendorId() const;
        ///Returns true if the above property is available.
        bool IsVendorIdAvailable() const;

        ///Retrieves the driver key name. For internal use only.
        ///This property is identified by Key::DriverKeyName.
        String_t GetDriverKeyName() const;
        ///Returns true if the above property is available.
        bool IsDriverKeyNameAvailable() const;

        ///Retrieves the usb driver type. For internal use only.
        ///This property is identified by Key::UsbDriverTypeKey.
        String_t GetUsbDriverType() const;
        ///Returns true if the above property is available.
        bool IsUsbDriverTypeAvailable() const;
    protected:
        CBaslerUsbDeviceInfo& SetDeviceGUID( const String_t& );
        CBaslerUsbDeviceInfo& SetManufacturerInfo( const String_t& );
        CBaslerUsbDeviceInfo& SetDeviceIdx( const String_t& );
        CBaslerUsbDeviceInfo& SetProductId( const String_t& );
        CBaslerUsbDeviceInfo& SetVendorId( const String_t& );
        CBaslerUsbDeviceInfo& SetDriverKeyName( const String_t& );
        CBaslerUsbDeviceInfo& SetUsbDriverType( const String_t& );

    };
}

#ifdef _MSC_VER
#   pragma pack(pop)
#endif /* _MSC_VER */

#endif /* INCLUDED_BASLERUSBDEVICEINFO_H_8532831 */
