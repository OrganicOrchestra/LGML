/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   littleredridinghoodmd_png;
    const int            littleredridinghoodmd_pngSize = 51582;

    extern const char*   default_lgmllayout;
    const int            default_lgmllayoutSize = 2223;

    extern const char*   removeBT_png;
    const int            removeBT_pngSize = 58335;

    extern const char*   grandlouloup_png;
    const int            grandlouloup_pngSize = 71763;

    extern const char*   wolf_footprint_png;
    const int            wolf_footprint_pngSize = 45430;

    extern const char*   wolf_footprint_debug_png;
    const int            wolf_footprint_debug_pngSize = 49058;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 6;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
