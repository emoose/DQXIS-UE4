//
// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

/// \file
/// \mainpage
/// AGS Library Overview
/// --------------------
/// This document provides an overview of the AGS (AMD GPU Services) library. The AGS library provides software developers with the ability to query 
/// AMD GPU software and hardware state information that is not normally available through standard operating systems or graphic APIs.
///
/// The latest version of the API is publicly hosted here: https://github.com/GPUOpen-LibrariesAndSDKs/AGS_SDK/.
/// It is also worth checking http://gpuopen.com/gaming-product/amd-gpu-services-ags-library/ for any updates and articles on AGS.
/// \internal
/// Online documentation is publicly hosted here: http://gpuopen-librariesandsdks.github.io/ags/
/// \endinternal
///
/// What's new in AGS 5.0 since version 4.x
/// ---------------------------------------
/// Version 5.0 is a major overhaul of the library designed to provide a much clearer view of the GPUs in the system and the displays attached to them. 
/// It also exposes the ability to query each display for HDR capabilities and put those HDR capable displays into various HDR modes.
/// Some functions such as agsGetGPUMemorySize and agsGetEyefinityConfigInfo have been removed in favor of including this information in the device & display enumeration.
/// Features include:
/// * Full GPU enumeration with adapter string, device id, revision id and vendor id.
/// * Per GPU display enumeration including information on display name, resolution and HDR capabilities.
/// * Optional user supplied memory allocator.
/// * Function to set displays into HDR mode.
/// * DirectX11 shader compiler controls.
/// * DirectX11 multiview extension.
/// * DirectX11 Crossfire API now supports using the API without needing a driver profile. Can also specify the transfer engine.
///
/// Using the AGS library
/// ---------------------
/// It is recommended to take a look at the source code for the samples that come with the AGS SDK:
/// * AGSSample
/// * CrossfireSample
/// * EyefinitySample
/// The AGSSample application is the simplest of the three examples and demonstrates the code required to initialize AGS and use it to query the GPU and Eyefinity state. 
/// The CrossfireSample application demonstrates the use of the new API to transfer resources on GPUs in Crossfire mode. Lastly, the EyefinitySample application provides a more 
/// extensive example of Eyefinity setup than the basic example provided in AGSSample.
/// There are other samples on Github that demonstrate the DirectX shader extensions, such as the Barycentrics11 and Barycentrics12 samples.
///
/// To add AGS support to an existing project, follow these steps:
/// * Link your project against the correct import library. Choose from either the 32 bit or 64 bit version.
/// * Copy the AGS dll into the same directory as your game executable.
/// * Include the amd_ags.h header file from your source code.
/// * Include the AGS hlsl files if you are using the shader intrinsics.
/// * Declare a pointer to an AGSContext and make this available for all subsequent calls to AGS.
/// * On game initialization, call agsInit() passing in the address of the context. On success, this function will return a valid context pointer.
/// * The agsInit() function should be called before the D3D device is created if the Crossfire mode is specified.
///
/// Don't forget to cleanup AGS by calling agsDeInit() when the app exits, after the device has been destroyed.

#ifndef AMD_AGS_H
#define AMD_AGS_H

#define AMD_AGS_VERSION_MAJOR 5             ///< AGS major version
#define AMD_AGS_VERSION_MINOR 0             ///< AGS minor version
#define AMD_AGS_VERSION_PATCH 5             ///< AGS patch version

#ifdef __cplusplus
extern "C" {
#endif


#define AMD_AGS_API __declspec(dllexport)   ///< AGS calling convention

// Forward declaration of D3D11 types
struct ID3D11Device;
struct ID3D11Resource;
struct ID3D11Buffer;
struct ID3D11Texture1D;
struct ID3D11Texture2D;
struct ID3D11Texture3D;
struct D3D11_BUFFER_DESC;
struct D3D11_TEXTURE1D_DESC;
struct D3D11_TEXTURE2D_DESC;
struct D3D11_TEXTURE3D_DESC;
struct D3D11_SUBRESOURCE_DATA;
struct tagRECT;
typedef tagRECT D3D11_RECT;             ///< typedef this ourselves so we don't have to drag d3d11.h in

// Forward declaration of D3D12 types
struct ID3D12Device;


/// The return codes
enum AGSReturnCode
{
    AGS_SUCCESS,                    ///< Succesful function call
    AGS_FAILURE,                    ///< Failed to complete call for some unspecified reason
    AGS_INVALID_ARGS,               ///< Invalid arguments into the function
    AGS_OUT_OF_MEMORY,              ///< Out of memory when allocating space internally
    AGS_ERROR_MISSING_DLL,          ///< Returned when a driver dll fails to load - most likely due to not being present in legacy driver installation
    AGS_ERROR_LEGACY_DRIVER,        ///< Returned if a feature is not present in the installed driver
    AGS_EXTENSION_NOT_SUPPORTED,    ///< Returned if the driver does not support the requested driver extension
    AGS_ADL_FAILURE,                ///< Failure in ADL (the AMD Display Library)
};

/// The DirectX11 extension support bits
enum AGSDriverExtensionDX11
{
    AGS_DX11_EXTENSION_QUADLIST                             = 1 << 0,
    AGS_DX11_EXTENSION_SCREENRECTLIST                       = 1 << 1,
    AGS_DX11_EXTENSION_UAV_OVERLAP                          = 1 << 2,
    AGS_DX11_EXTENSION_DEPTH_BOUNDS_TEST                    = 1 << 3,
    AGS_DX11_EXTENSION_MULTIDRAWINDIRECT                    = 1 << 4,
    AGS_DX11_EXTENSION_MULTIDRAWINDIRECT_COUNTINDIRECT      = 1 << 5,
    AGS_DX11_EXTENSION_CROSSFIRE_API                        = 1 << 6,
    AGS_DX11_EXTENSION_INTRINSIC_READFIRSTLANE              = 1 << 7,
    AGS_DX11_EXTENSION_INTRINSIC_READLANE                   = 1 << 8,
    AGS_DX11_EXTENSION_INTRINSIC_LANEID                     = 1 << 9,
    AGS_DX11_EXTENSION_INTRINSIC_SWIZZLE                    = 1 << 10,
    AGS_DX11_EXTENSION_INTRINSIC_BALLOT                     = 1 << 11,
    AGS_DX11_EXTENSION_INTRINSIC_MBCOUNT                    = 1 << 12,
    AGS_DX11_EXTENSION_INTRINSIC_COMPARE3                   = 1 << 13,
    AGS_DX11_EXTENSION_INTRINSIC_BARYCENTRICS               = 1 << 14,
    AGS_DX11_EXTENSION_CREATE_SHADER_CONTROLS               = 1 << 15,
    AGS_DX11_EXTENSION_MULTIVIEW                            = 1 << 16
};

/// The DirectX12 extension support bits
enum AGSDriverExtensionDX12
{
    AGS_DX12_EXTENSION_INTRINSIC_READFIRSTLANE              = 1 << 0,
    AGS_DX12_EXTENSION_INTRINSIC_READLANE                   = 1 << 1,
    AGS_DX12_EXTENSION_INTRINSIC_LANEID                     = 1 << 2,
    AGS_DX12_EXTENSION_INTRINSIC_SWIZZLE                    = 1 << 3,
    AGS_DX12_EXTENSION_INTRINSIC_BALLOT                     = 1 << 4,
    AGS_DX12_EXTENSION_INTRINSIC_MBCOUNT                    = 1 << 5,
    AGS_DX12_EXTENSION_INTRINSIC_COMPARE3                   = 1 << 6,
    AGS_DX12_EXTENSION_INTRINSIC_BARYCENTRICS               = 1 << 7,
};

/// The space id for DirectX12 intrinsic support
const unsigned int AGS_DX12_SHADER_INSTRINSICS_SPACE_ID = 0x7FFF0ADE; // 2147420894


/// Addtional topologies supported via extensions
enum AGSPrimitiveTopology
{
    AGS_PRIMITIVE_TOPOLOGY_QUADLIST                         = 7,
    AGS_PRIMITIVE_TOPOLOGY_SCREENRECTLIST                   = 9
};

/// The different modes to control Crossfire behavior.
enum AGSCrossfireMode
{
    AGS_CROSSFIRE_MODE_DRIVER_AFR = 0,                              ///< Use the default driver-based AFR rendering
    AGS_CROSSFIRE_MODE_EXPLICIT_AFR,                                ///< Use the AGS Crossfire API functions to perform explicit AFR rendering without requiring a CF driver profile
    AGS_CROSSFIRE_MODE_DISABLE                                      ///< Completely disable AFR rendering
};


/// The Crossfire API transfer types
enum AGSAfrTransferType
{
    AGS_AFR_TRANSFER_DEFAULT                                = 0,    ///< Default Crossfire driver resource tracking
    AGS_AFR_TRANSFER_DISABLE                                = 1,    ///< Turn off driver resource tracking
    AGS_AFR_TRANSFER_1STEP_P2P                              = 2,    ///< App controlled GPU to next GPU transfer
    AGS_AFR_TRANSFER_2STEP_NO_BROADCAST                     = 3,    ///< App controlled GPU to next GPU transfer using intermediate system memory
    AGS_AFR_TRANSFER_2STEP_WITH_BROADCAST                   = 4,    ///< App controlled GPU to all render GPUs transfer using intermediate system memory
};

/// The Crossfire API transfer engines
enum AGSAfrTransferEngine
{
    AGS_AFR_TRANSFERENGINE_DEFAULT                          = 0,    ///< Use default engine for Crossfire API transfers
    AGS_AFR_TRANSFERENGINE_3D_ENGINE                        = 1,    ///< Use 3D engine for Crossfire API transfers
    AGS_AFR_TRANSFERENGINE_COPY_ENGINE                      = 2,    ///< Use Copy engine for Crossfire API transfers
};

/// The display flags describing various properties of the display.
enum AGSDisplayFlags
{
    AGS_DISPLAYFLAG_PRIMARY_DISPLAY                         = 1 << 0,   ///< Whether this display is marked as the primary display
    AGS_DISPLAYFLAG_HDR10                                   = 1 << 1,   ///< HDR10 is supported on this display
    AGS_DISPLAYFLAG_DOLBYVISION                             = 1 << 2,   ///< Dolby Vision is supported on this display
    AGS_DISPLAYFLAG_EYEFINITY_IN_GROUP                      = 1 << 4,   ///< The display is part of the Eyefinity group
    AGS_DISPLAYFLAG_EYEFINITY_PREFERRED_DISPLAY             = 1 << 5,   ///< The display is the preferred display in the Eyefinity group for displaying the UI
    AGS_DISPLAYFLAG_EYEFINITY_IN_PORTRAIT_MODE              = 1 << 6,   ///< The display is in the Eyefinity group but in portrait mode
};

struct AGSContext;  ///< All function calls in AGS require a pointer to a context. This is generated via \ref agsInit

/// The rectangle struct used by AGS.
struct AGSRect
{
    int offsetX;    ///< Offset on X axis
    int offsetY;    ///< Offset on Y axis
    int width;      ///< Width of rectangle
    int height;     ///< Height of rectangle
};

/// The clip rectangle struct used by \ref agsDriverExtensionsDX11_SetClipRects
struct AGSClipRect
{
    /// The inclusion mode for the rect
    enum Mode
    {
        ClipRectIncluded = 0,   ///< Include the rect
        ClipRectExcluded = 1    ///< Exclude the rect
    };

    Mode            mode; ///< Include/exclude rect region
    AGSRect         rect; ///< The rect to include/exclude
};

/// The display info struct used to describe a display enumerated by AGS
struct AGSDisplayInfo
{
    char                    name[ 256 ];                    ///< The name of the display
    char                    displayDeviceName[ 32 ];        ///< The display device name, ie DISPLAY_DEVICE::DeviceName

    unsigned int            displayFlags;                   ///< Bitfield of ::AGSDisplayFlags

    int                     maxResolutionX;                 ///< The maximum supported resolution of the display
    int                     maxResolutionY;                 ///< The maximum supported resolution of the display
    float                   maxRefreshRate;                 ///< The maximum supported refresh rate of the display

    AGSRect                 currentResolution;              ///< The current resolution and position in the desktop
    AGSRect                 visibleResolution;              ///< The visible resolution and position. Note this may be a sub region
                                                            ///< of currentResolution when Eyefinity bezel compensation is enabled
    float                   currentRefreshRate;             ///< The current refresh rate

    int                     eyefinityGridCoordX;            ///< The X coordinate in the Eyefinity grid. -1 if not in an Eyefinity group
    int                     eyefinityGridCoordY;            ///< The Y coordinate in the Eyefinity grid. -1 if not in an Eyefinity group

    double                  chromaticityRedX;               ///< Red display primary X coord
    double                  chromaticityRedY;               ///< Red display primary Y coord

    double                  chromaticityGreenX;             ///< Green display primary X coord
    double                  chromaticityGreenY;             ///< Green display primary Y coord

    double                  chromaticityBlueX;              ///< Blue display primary X coord
    double                  chromaticityBlueY;              ///< Blue display primary Y coord

    double                  chromaticityWhitePointX;        ///< White point X coord
    double                  chromaticityWhitePointY;        ///< White point Y coord

    double                  screenDiffuseReflectance;       ///< Percentage expressed between 0 - 1
    double                  screenSpecularReflectance;      ///< Percentage expressed between 0 - 1

    double                  minLuminance;                   ///< The minimum luminance of the display in nits
    double                  maxLuminance;                   ///< The maximum luminance of the display in nits
    double                  avgLuminance;                   ///< The average luminance of the display in nits

    int                     logicalDisplayIndex;            ///< The internally used index of this display
    int                     adlAdapterIndex;                ///< The internally used ADL adapter index
};

/// The device info struct used to describe a physical GPU enumerated by AGS
struct AGSDeviceInfo
{
    /// The architecture version
    enum ArchitectureVersion
    {
        ArchitectureVersion_Unknown,                                ///< Unknown architecture, potentially from another IHV. Check AGSDeviceInfo::vendorId
        ArchitectureVersion_PreGCN,                                 ///< AMD architecture, pre-GCN
        ArchitectureVersion_GCN                                     ///< AMD GCN architecture
    };

    ArchitectureVersion             architectureVersion;            ///< Set to Unknown if not AMD hardware
    const char*                     adapterString;                  ///< The adapter name string
    int                             vendorId;                       ///< The vendor id
    int                             deviceId;                       ///< The device id
    int                             revisionId;                     ///< The revision id

    int                             numCUs;                         ///< Number of GCN compute units. Zero if not GCN
    int                             coreClock;                      ///< Core clock speed at 100% power in MHz
    int                             memoryClock;                    ///< Memory clock speed at 100% power in MHz
    float                           teraFlops;                      ///< Teraflops of GPU. Zero if not GCN. Calculated from iCoreClock * iNumCUs * 64 Pixels/clk * 2 instructions/MAD

    int                             isPrimaryDevice;                ///< Whether or not this is the primary adapter in the system.
    long long                       localMemoryInBytes;             ///< The size of local memory in bytes. 0 for non AMD hardware.

    int                             numDisplays;                    ///< The number of active displays found to be attached to this adapter.
    AGSDisplayInfo*                 displays;                       ///< List of displays allocated by AGS to be numDisplays in length.

    int                             eyefinityEnabled;               ///< Indicates if Eyefinity is active
    int                             eyefinityGridWidth;             ///< Contains width of the multi-monitor grid that makes up the Eyefinity Single Large Surface.
    int                             eyefinityGridHeight;            ///< Contains height of the multi-monitor grid that makes up the Eyefinity Single Large Surface.
    int                             eyefinityResolutionX;           ///< Contains width in pixels of the multi-monitor Single Large Surface.
    int                             eyefinityResolutionY;           ///< Contains height in pixels of the multi-monitor Single Large Surface.
    int                             eyefinityBezelCompensated;      ///< Indicates if bezel compensation is used for the current SLS display area. 1 if enabled, and 0 if disabled.

    int                             adlAdapterIndex;                ///< Internally used index into the ADL list of adapters
};

/// \defgroup general General API functions
/// API for initialization, cleanup, HDR display modes and Crossfire GPU count
/// @{

typedef void* (__stdcall *AGS_ALLOC_CALLBACK)( int allocationSize );    ///< AGS user defined allocation protoype
typedef void (__stdcall *AGS_FREE_CALLBACK)( void* allocationPtr );     ///< AGS user defined free protoype

                                                                        /// The configuration options that can be passed in to \ref agsInit
struct AGSConfiguration
{
    AGS_ALLOC_CALLBACK      allocCallback;                  ///< Optional memory allocation callback. If not supplied, malloc() is used
    AGS_FREE_CALLBACK       freeCallback;                   ///< Optional memory freeing callback. If not supplied, free() is used
    AGSCrossfireMode        crossfireMode;                  ///< Desired Crossfire mode
};

/// The top level GPU information returned from \ref agsInit
struct AGSGPUInfo
{
    int                     agsVersionMajor;                ///< Major field of Major.Minor.Patch AGS version number
    int                     agsVersionMinor;                ///< Minor field of Major.Minor.Patch AGS version number
    int                     agsVersionPatch;                ///< Patch field of Major.Minor.Patch AGS version number
    int                     isWACKCompliant;                ///< 1 if WACK compliant.

    const char*             driverVersion;                  ///< The AMD driver package version
    const char*             radeonSoftwareVersion;          ///< The Radeon Software Version

    int                     numDevices;                     ///< Number of GPUs in the system
    AGSDeviceInfo*          devices;                        ///< List of GPUs in the system
};

/// The struct to specify the display settings to the driver.
struct AGSDisplaySettings
{
    /// The display mode
    enum Mode
    {
        Mode_SDR,                                           ///< SDR mode
        Mode_scRGB,                                         ///< scRGB, requiring an FP16 swapchain. Values of 1.0 == 80 nits, 125.0 == 10000 nits. Uses REC709 primaries.
        Mode_PQ,                                            ///< PQ encoding, requiring a 1010102 UNORM swapchain and PQ encoding in the output shader. Uses BT2020 primaries.
        Mode_DolbyVision                                    ///< Dolby Vision, requiring an 8888 UNORM swapchain
    };

    Mode                    mode;                           ///< The display mode to set the display into

    double                  chromaticityRedX;               ///< Red display primary X coord
    double                  chromaticityRedY;               ///< Red display primary Y coord

    double                  chromaticityGreenX;             ///< Green display primary X coord
    double                  chromaticityGreenY;             ///< Green display primary Y coord

    double                  chromaticityBlueX;              ///< Blue display primary X coord
    double                  chromaticityBlueY;              ///< Blue display primary Y coord

    double                  chromaticityWhitePointX;        ///< White point X coord
    double                  chromaticityWhitePointY;        ///< White point Y coord

    double                  minLuminance;                   ///< The minimum scene luminance in nits
    double                  maxLuminance;                   ///< The maximum scene luminance in nits

    double                  maxContentLightLevel;           ///< The maximum content light level in nits (MaxCLL)
    double                  maxFrameAverageLightLevel;      ///< The maximum frame average light livel in nits (MaxFALL)
};

///
/// Function used to initialize the AGS library.
/// Must be called prior to any of the subsequent AGS API calls.
/// Must be called prior to ID3D11Device or ID3D12Device creation.
/// \note This function will fail with AGS_ERROR_LEGACY_DRIVER in Catalyst versions before 12.20.
/// \note It is good practice to check the AGS version returned from AGSGPUInfo against the version defined in the header in case a mismatch between the dll and header has occurred.
///
/// \param [in, out] context                        Address of a pointer to a context. This function allocates a context on the heap which is then required for all subsequent API calls.
/// \param [in] config                              Optional pointer to a AGSConfiguration struct to override the default library configuration.
/// \param [out] gpuInfo                            Optional pointer to a AGSGPUInfo struct which will get filled in for all the GPUs in the system.
///
AMD_AGS_API AGSReturnCode agsInit( AGSContext** context, const AGSConfiguration* config, AGSGPUInfo* gpuInfo );

///
///   Function used to clean up the AGS library.
///
/// \param [in] context                             Pointer to a context. This function will deallocate the context from the heap.
///
AMD_AGS_API AGSReturnCode agsDeInit( AGSContext* context );

///
/// Function used to query the number of GPUs used for Crossfire acceleration.
/// This may be different from the total number of GPUs present in the system.
///
/// \param [in] context                             Pointer to a context.
/// \param [out] numGPUs                            Number of GPUs used for Crossfire acceleration
///
AMD_AGS_API AGSReturnCode agsGetCrossfireGPUCount( AGSContext* context, int* numGPUs );

///
/// Function used to set a specific display into HDR mode
/// \note Setting all of the values apart from color space and transfer function to zero will cause the display to use defaults.
/// \note Call this function after each mode change (switch to fullscreen, any change in swapchain etc).
/// \note HDR10 PQ mode requires a 1010102 swapchain. HDR10 linear125 mode requires an FP16 swapchain.
/// \note Dolby Vision requires a 8888 UNORM swapchain.
///
/// \param [in] context                             Pointer to a context. This is generated by \ref agsInit
/// \param [in] deviceIndex                         The index of the device listed in \ref AGSGPUInfo::devices.
/// \param [in] displayIndex                        The index of the display listed in \ref AGSDeviceInfo::displays.
/// \param [in] settings                            Pointer to the display settings to use.
///
AMD_AGS_API AGSReturnCode agsSetDisplayMode( AGSContext* context, int deviceIndex, int displayIndex, const AGSDisplaySettings* settings );

/// @}

/// \defgroup dx12 DirectX12 Extensions
/// DirectX12 driver extensions
/// @{

/// \defgroup dx12init Initialization and Cleanup
/// @{

///
/// Function used to initialize the AMD-specific driver extensions for D3D12.
/// D3D12 extensions are supported in Radeon Software Crimson Edition 16.9.2 (driver version 16.40.2311) onwards.
/// Newer extensions may require more recent versions of the driver. Check support with extensionsSupported.
///
/// \param [in] context                             Pointer to a context. This is generated by \ref agsInit
/// \param [in] device                              The D3D12 device.
/// \param [out] extensionsSupported                Pointer to a bit mask that this function will fill in to indicate which extensions are supported. See ::AGSDriverExtensionDX12
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX12_Init( AGSContext* context, ID3D12Device* device, unsigned int* extensionsSupported );

///
/// Function used to cleanup any AMD-specific driver extensions for D3D12
///
/// \param [in] context                             Pointer to a context.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX12_DeInit( AGSContext* context );

/// @}

/// @}

/// \defgroup dx11 DirectX11 Extensions
/// DirectX11 driver extensions
/// @{

/// \defgroup dx11init Initialization and Cleanup
/// @{

///
/// Function used to initialize the AMD-specific driver extensions for D3D11.
/// Shader intrinsics are supported in Radeon Software Crimson Edition 16.9.2 (driver version 16.40.2311) onwards.
/// The multiview extension requires Radeon Software Crimson ReLive Edition 16.12.1 (driver version 16.50.2001) or later.
/// Newer extensions may require more recent versions of the driver. Check support with extensionsSupported.
///
/// \param [in] context                             Pointer to a context. This is generated by \ref agsInit
/// \param [in] device                              The D3D11 device.
/// \param [in] uavSlot                             The UAV slot reserved for intrinsic support. This must match the slot defined in the HLSL, ie #define AmdDxExtShaderIntrinsicsUAVSlot.
///                                                 The default slot is 7, but the caller is free to use an alternative slot.
/// \param [out] extensionsSupported                Pointer to a bit mask that this function will fill in to indicate which extensions are supported. See AGSDriverExtensionDX11
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_Init( AGSContext* context, ID3D11Device* device, unsigned int uavSlot, unsigned int* extensionsSupported );

///
/// Function used to cleanup any AMD-specific driver extensions for D3D11
///
/// \param [in] context                             Pointer to a context.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_DeInit( AGSContext* context );

/// @}

/// \defgroup dx11misc Misc Extensions
/// API for depth bounds test, UAV overlap and prim topologies
/// @{

///
/// Function used to set the primitive topology. If you are using any of the extended topology types, then this function should
/// be called to set ALL topology types.
///
/// The Quad List extension is a convenient way to submit quads without using an index buffer. Note that this still submits two triangles at the driver level. 
/// In order to use this function, AGS must already be initialized and agsDriverExtensionsDX11_Init must have been called successfully.
///
/// The Screen Rect extension, which is only available on GCN hardware, allows the user to pass in three of the four corners of a rectangle. 
/// The hardware then uses the bounding box of the vertices to rasterize the rectangle primitive (ie as a rectangle rather than two triangles). 
/// \note Note that this will not return valid interpolated values, only valid SV_Position values.
/// \note If either the Quad List or Screen Rect extension are used, then agsDriverExtensionsDX11_IASetPrimitiveTopology should be called in place of the native DirectX11 equivalent all the time.
///
/// \param [in] context                             Pointer to a context.
/// \param [in] topology                            The topology to set on the D3D11 device. This can be either an AGS-defined topology such as AGS_PRIMITIVE_TOPOLOGY_QUAD_LIST
///                                                 or a standard D3D-defined topology such as D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP.
///                                                 NB. the AGS-defined types will require casting to a D3D_PRIMITIVE_TOPOLOGY type.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_IASetPrimitiveTopology( AGSContext* context, enum D3D_PRIMITIVE_TOPOLOGY topology );

///
/// Function used indicate to the driver it can overlap the subsequent batch of back-to-back dispatches.
/// When calling back-to-back draw calls or dispatch calls that write to the same UAV, the AMD DX11 driver will automatically insert a barrier to ensure there are no write after write (WAW) hazards.
/// If the app can guarantee there is no overlap between the writes between these calls, then this extension will remove those barriers allowing the work to run in parallel on the GPU.
///
/// Usage would be as follows:
/// \code{.cpp}
///     // Disable automatic WAW syncs
///     agsDriverExtensionsDX11_BeginUAVOverlap( m_agsContext );
///
///     // Submit back-to-back dispatches that write to the same UAV
///     m_device->Dispatch( ... );  // First half of UAV
///     m_device->Dispatch( ... );  // Second half of UAV
///
///     // Reenable automatic WAW syncs
///     agsDriverExtensionsDX11_EndUAVOverlap( m_agsContext );
/// \endcode
///
/// \param [in] context                             Pointer to a context.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_BeginUAVOverlap( AGSContext* context );

///
/// Function used indicate to the driver it can no longer overlap the batch of back-to-back dispatches that has been submitted.
///
/// \param [in] context                             Pointer to a context.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_EndUAVOverlap( AGSContext* context );

///
/// Function used to set the depth bounds test extension
///
/// \param [in] context                             Pointer to a context.
/// \param [in] enabled                             Whether to enable or disable the depth bounds testing. If disabled, the next two args are ignored.
/// \param [in] minDepth                            The near depth range to clip against.
/// \param [in] maxDepth                            The far depth range to clip against.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_SetDepthBounds( AGSContext* context, bool enabled, float minDepth, float maxDepth );

/// @}

/// \defgroup mdi Multi Draw Indirect (MDI)
/// API for dispatching multiple instanced draw commands.
/// The multi draw indirect extensions allow multiple sets of DrawInstancedIndirect to be submitted in one API call.
/// The draw calls are issued on the GPU's command processor (CP), potentially saving the significant CPU overheads incurred by submitting the equivalent draw calls on the CPU.
///
/// The extension allows the following code:
/// \code{.cpp}
///     // Submit n batches of DrawIndirect calls
///     for ( int i = 0; i < n; i++ )
///         DrawIndexedInstancedIndirect( buffer, i * sizeof( cmd ) );
/// \endcode
/// To be replaced by the following call:
/// \code{.cpp}
///     // Submit all n batches in one call
///     agsDriverExtensionsDX11_MultiDrawIndexedInstancedIndirect( m_agsContext, n, buffer, 0, sizeof( cmd ) );
/// \endcode
///
/// The buffer used for the indirect args must be of the following formats:
/// \code{.cpp}
///     // Buffer layout for agsDriverExtensions_MultiDrawInstancedIndirect
///     struct DrawInstancedIndirectArgs
///     {
///         UINT VertexCountPerInstance;
///         UINT InstanceCount;
///         UINT StartVertexLocation;
///         UINT StartInstanceLocation;
///     };
///
///     // Buffer layout for agsDriverExtensions_MultiDrawIndexedInstancedIndirect
///     struct DrawIndexedInstancedIndirectArgs
///     {
///         UINT IndexCountPerInstance;
///         UINT InstanceCount;
///         UINT StartIndexLocation;
///         UINT BaseVertexLocation;
///         UINT StartInstanceLocation;
///     };
/// \endcode
///
/// @{

///
/// Function used to submit a batch of draws via MultiDrawIndirect
///
/// \param [in] context                             Pointer to a context.
/// \param [in] drawCount                           The number of draws.
/// \param [in] pBufferForArgs                      The args buffer.
/// \param [in] alignedByteOffsetForArgs            The offset into the args buffer.
/// \param [in] byteStrideForArgs                   The per element stride of the args buffer.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_MultiDrawInstancedIndirect( AGSContext* context, unsigned int drawCount, ID3D11Buffer* pBufferForArgs, unsigned int alignedByteOffsetForArgs, unsigned int byteStrideForArgs );

///
/// Function used to submit a batch of draws via MultiDrawIndirect
///
/// \param [in] context                             Pointer to a context.
/// \param [in] drawCount                           The number of draws.
/// \param [in] pBufferForArgs                      The args buffer.
/// \param [in] alignedByteOffsetForArgs            The offset into the args buffer.
/// \param [in] byteStrideForArgs                   The per element stride of the args buffer.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_MultiDrawIndexedInstancedIndirect( AGSContext* context, unsigned int drawCount, ID3D11Buffer* pBufferForArgs, unsigned int alignedByteOffsetForArgs, unsigned int byteStrideForArgs );

///
/// Function used to submit a batch of draws via MultiDrawIndirect
///
/// \param [in] context                             Pointer to a context.
/// \param [in] pBufferForDrawCount                 The draw count buffer.
/// \param [in] alignedByteOffsetForDrawCount       The offset into the draw count buffer.
/// \param [in] pBufferForArgs                      The args buffer.
/// \param [in] alignedByteOffsetForArgs            The offset into the args buffer.
/// \param [in] byteStrideForArgs                   The per element stride of the args buffer.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_MultiDrawInstancedIndirectCountIndirect( AGSContext* context, ID3D11Buffer* pBufferForDrawCount, unsigned int alignedByteOffsetForDrawCount, ID3D11Buffer* pBufferForArgs, unsigned int alignedByteOffsetForArgs, unsigned int byteStrideForArgs );

///
/// Function used to submit a batch of draws via MultiDrawIndirect
///
/// \param [in] context                             Pointer to a context.
/// \param [in] pBufferForDrawCount                 The draw count buffer.
/// \param [in] alignedByteOffsetForDrawCount       The offset into the draw count buffer.
/// \param [in] pBufferForArgs                      The args buffer.
/// \param [in] alignedByteOffsetForArgs            The offset into the args buffer.
/// \param [in] byteStrideForArgs                   The per element stride of the args buffer.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_MultiDrawIndexedInstancedIndirectCountIndirect( AGSContext* context, ID3D11Buffer* pBufferForDrawCount, unsigned int alignedByteOffsetForDrawCount, ID3D11Buffer* pBufferForArgs, unsigned int alignedByteOffsetForArgs, unsigned int byteStrideForArgs );

/// @}

/// \defgroup shadercompiler Shader Compiler Controls
/// API for controlling DirectX11 shader compilation.
/// Check support for this feature using the AGS_DX11_EXTENSION_CREATE_SHADER_CONTROLS bit.
/// Supported in Radeon Software Crimson Edition 16.9.2 (driver version 16.40.2311) onwards.
/// @{

///
/// This method can be used to limit the maximum number of threads the driver uses for asynchronous shader compilation.
/// Setting it to 0 will disable asynchronous compilation completely and force the shaders to be compiled ?inline? on the threads that call Create*Shader.
///
/// This method can only be called before any shaders are created and being compiled by the driver.
/// If this method is called after shaders have been created the function will return AGS_FAILURE.
/// This function only sets an upper limit.The driver may create fewer threads than allowed by this function.
///
/// \param [in] context                             Pointer to a context.
/// \param [in] numberOfThreads                     The maximum number of threads to use.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_SetMaxAsyncCompileThreadCount( AGSContext* context, unsigned int numberOfThreads );

///
/// This method can be used to determine the total number of asynchronous shader compile jobs that are either
/// queued for waiting for compilation or being compiled by the driver?s asynchronous compilation threads.
/// This method can be called at any during the lifetime of the driver.
///
/// \param [in] context                             Pointer to a context.
/// \param [out] numberOfJobs                       Pointer to the number of jobs in flight currently.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_NumPendingAsyncCompileJobs( AGSContext* context, unsigned int* numberOfJobs );

///
/// This method can be used to enable or disable the disk based shader cache.
/// Enabling/disabling the disk cache is not supported if is it disabled explicitly via Radeon Settings or by an app profile.
/// Calling this method under these conditions will result in AGS_FAILURE being returned.
/// It is recommended that this method be called before any shaders are created by the application and being compiled by the driver.
/// Doing so at any other time may result in the cache being left in an inconsistent state.
///
/// \param [in] context                             Pointer to a context.
/// \param [in] enable                              Whether to enable the disk cache. 0 to disable, 1 to enable.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_SetDiskShaderCacheEnabled( AGSContext* context, int enable );

/// @}

/// \defgroup multiview Multiview
/// API for multiview broadcasting.
/// Check support for this feature using the AGS_DX11_EXTENSION_MULTIVIEW bit.
/// Supported in Radeon Software Crimson ReLive Edition 16.12.1 (driver version 16.50.2001) onwards.
/// @{

///
/// Function to control draw calls replication to multiple viewports and RT slices.
/// Setting any mask to 0 disables draw replication.
///
/// \param [in] context                             Pointer to a context.
/// \param [in] vpMask                              Viewport control bit mask.
/// \param [in] rtSliceMask                         RT slice control bit mask.
/// \param [in] vpMaskPerRtSliceEnabled             If 0, 16 lower bits of vpMask apply to all RT slices; if 1 each 16 bits of 64-bit mask apply to corresponding 4 RT slices.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_SetViewBroadcastMasks( AGSContext* context, unsigned long long vpMask, unsigned long long rtSliceMask, int vpMaskPerRtSliceEnabled );

///
/// Function returns max number of supported clip rectangles.
///
/// \param [in] context                             Pointer to a context.
/// \param [out] maxRectCount                       Returned max number of clip rectangles.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_GetMaxClipRects( AGSContext* context, unsigned int* maxRectCount );

///
/// Function sets clip rectangles.
///
/// \param [in] context                             Pointer to a context.
/// \param [in] clipRectCount                       Number of specified clip rectangles. Use 0 to disable clip rectangles.
/// \param [in] clipRects                           Array of clip rectangles.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_SetClipRects( AGSContext* context, unsigned int clipRectCount, const AGSClipRect* clipRects );

/// @}

/// \defgroup cfxapi Explicit Crossfire API
/// API for explicit control over Crossfire
/// @{

///
/// Function to create a Direct3D11 resource with the specified AFR transfer type and specified transfer engine.
///
/// \param [in] context                             Pointer to a context.
/// \param [in] desc                                Pointer to the D3D11 resource description.
/// \param [in] initialData                         Optional pointer to the initializing data for the resource.
/// \param [out] buffer                             Returned pointer to the resource.
/// \param [in] transferType                        The transfer behavior.
/// \param [in] transferEngine                      The transfer engine to use.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_CreateBuffer( AGSContext* context, const D3D11_BUFFER_DESC* desc, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Buffer** buffer, AGSAfrTransferType transferType, AGSAfrTransferEngine transferEngine );

///
/// Function to create a Direct3D11 resource with the specified AFR transfer type and specified transfer engine.
///
/// \param [in] context                             Pointer to a context.
/// \param [in] desc                                Pointer to the D3D11 resource description.
/// \param [in] initialData                         Optional pointer to the initializing data for the resource.
/// \param [out] texture1D                          Returned pointer to the resource.
/// \param [in] transferType                        The transfer behavior.
/// \param [in] transferEngine                      The transfer engine to use.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_CreateTexture1D( AGSContext* context, const D3D11_TEXTURE1D_DESC* desc, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Texture1D** texture1D, AGSAfrTransferType transferType, AGSAfrTransferEngine transferEngine );

///
/// Function to create a Direct3D11 resource with the specified AFR transfer type and specified transfer engine.
///
/// \param [in] context                             Pointer to a context.
/// \param [in] desc                                Pointer to the D3D11 resource description.
/// \param [in] initialData                         Optional pointer to the initializing data for the resource.
/// \param [out] texture2D                          Returned pointer to the resource.
/// \param [in] transferType                        The transfer behavior.
/// \param [in] transferEngine                      The transfer engine to use.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_CreateTexture2D( AGSContext* context, const D3D11_TEXTURE2D_DESC* desc, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Texture2D** texture2D, AGSAfrTransferType transferType, AGSAfrTransferEngine transferEngine );

///
/// Function to create a Direct3D11 resource with the specified AFR transfer type and specified transfer engine.
///
/// \param [in] context                             Pointer to a context.
/// \param [in] desc                                Pointer to the D3D11 resource description.
/// \param [in] initialData                         Optional pointer to the initializing data for the resource.
/// \param [out] texture3D                          Returned pointer to the resource.
/// \param [in] transferType                        The transfer behavior.
/// \param [in] transferEngine                      The transfer engine to use.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_CreateTexture3D( AGSContext* context, const D3D11_TEXTURE3D_DESC* desc, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Texture3D** texture3D, AGSAfrTransferType transferType, AGSAfrTransferEngine transferEngine );

///
/// Function to notify the driver that we have finished writing to the resource this frame.
/// This will initiate a transfer for AGS_AFR_TRANSFER_1STEP_P2P,
/// AGS_AFR_TRANSFER_2STEP_NO_BROADCAST, and AGS_AFR_TRANSFER_2STEP_WITH_BROADCAST.
///
/// \param [in] context                             Pointer to a context.
/// \param [in] resource                            Pointer to the resource.
/// \param [in] transferRegions                     An array of transfer regions (can be null to specify the whole area).
/// \param [in] subresourceArray                    An array of subresource indices (can be null to specify all subresources).
/// \param [in] numSubresources                     The number of subresources in subresourceArray OR number of transferRegions. Use 0 to specify ALL subresources and one transferRegion (which may be null if specifying the whole area).
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_NotifyResourceEndWrites( AGSContext* context, ID3D11Resource* resource, const D3D11_RECT* transferRegions, const unsigned int* subresourceArray, unsigned int numSubresources );

///
/// This will notify the driver that the app will begin read/write access to the resource.
///
/// \param [in] context                             Pointer to a context.
/// \param [in] resource                            Pointer to the resource.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_NotifyResourceBeginAllAccess( AGSContext* context, ID3D11Resource* resource );

///
///   This is used for AGS_AFR_TRANSFER_1STEP_P2P to notify when it is safe to initiate a transfer.
///   This call in frame N-(NumGpus-1) allows a 1 step P2P in frame N to start.
///   This should be called after agsDriverExtensionsDX11_NotifyResourceEndWrites.
///
/// \param [in] context                             Pointer to a context.
/// \param [in] resource                            Pointer to the resource.
///
AMD_AGS_API AGSReturnCode agsDriverExtensionsDX11_NotifyResourceEndAllAccess( AGSContext* context, ID3D11Resource* resource );

/// @}

/// @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // AMD_AGS_H
