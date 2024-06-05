# ATG  Samples Content Exporter

https://aka.ms/dxsdkcontentexporter

Release available for download on [GitHub](https://github.com/walbourn/contentexporter/releases)

## Release History

### June 5, 2024
* Updated for FBX SDK 2020.3.7
* Updated for DirectXTex, DirectXMesh, and UVAtlas Jun 2024 releases

### December 31, 2023
* Updated for DirectXTex, DirectXMesh, and UVAtlas December 2023 releases

### March 31, 2023
* Updated for DirectXTex, DirectXMesh, and UVAtlas March 2023 releases

### October 18, 2022
* Updated for DirectXTex, DirectXMesh, and UVAtlas October 2022 releases

### July 30, 2022
* Updated for DirectXTex, DirectXMesh, and UVAtlas July 2022 releases

### May 10, 2022
* Updated for DirectXTex, DirectXMesh, and UVAtlas May 2022 releases

### March 24, 2022
* Updated for DirectXTex, DirectXMesh, and UVAtlas March 2022 releases
* Update build switches for SDL recommendations

### February 28, 2022
* Updated for FBX SDK 2020.3.1
* Updated for DirectXTex, DirectXMesh, and UVAtlas February 2022 releases

### November 10, 2021
* Fixed crash when a 0 polygon mesh is encountered (now emits a warning instead)
* Added "12.2" to the ``-featurelevel`` switch
* Updated for DirectXTex, DirectXMesh, and UVAtlas November 2021 releases

### October 21, 2021
* Updated for DirectXTex August 2021 release
* Updated for DirectXMesh and UVAtlas June 2021 releases
* Retired VS 2017 projects
* Code cleanup

### December 4, 2020
* Updated for UVAtlas December 2020 release
* Added ``-uvatlaslfs`` and ``-uvatlaslms`` switches for new UVAtlas artist control behavior flags

### November 13, 2020
* Switched to using NuGet for DirectXTex, DirectXMesh, and UVAtlas using November 2020 releases

### June 1, 2020
* Switched to using NuGet for DirectXTex, DirectXMesh, and UVAtlas using June 2020 releases
* Minor code review

### February 26, 2020
* Switched to using NuGet for DirectXTex, DirectXMesh, and UVAtlas using February 2020 releases
* Retired VS 2015 projects, added VS 2019 projects
* Code cleanup and reformat

### February 8, 2019
* Support for SDKMESH v2 with PBR materials using ``-sdkmesh2`` switch
* Updated DirectXMesh, DirectXTex, and UVAtlas libraries

### November 30, 2018
* Updated for FBX SDK 2019.2
* OpenEXR support removed from VS 2015 project
* Updated DirectXMesh, DirectXTex, and UVAtlas libraries

### August 17, 2018
* Updated DirectXMesh, DirectXTex, and UVAtlas libraries

### May 31, 2018
* VS 2017 updated for Windows 10 April 2018 Update SDK (17134)
* Updated DirectXMesh, DirectXTex, and UVAtlas libraries

### May 14, 2018
* Fixed SDKMESH file writer if subset has no material
* Updated DirectXMesh, DirectXTex, and UVAtlas libraries
* Retired VS 2013 projects

### April 30, 2018
* Updated DirectXMesh, DirectXTex, and UVAtlas libraries

### February 9, 2018
* Added ``-optimization`` switch to control LUR vs. Hoppe algorithm (defaults to LRU)
* Updated DirectXMesh, DirectXTex, and UVAtlas libraries

### November 2, 2017
* VS 2017 updated for Windows 10 Fall Creators Update SDK (16299)
* Updated DirectXMesh, DirectXTex, and UVAtlas libraries

### September 22, 2017
* Updated DirectXMesh, DirectXTex, and UVAtlas for VS 2017 15.3 update /permissive- changes

### July 28, 2017
* Added support for OpenEXR (via NuGet) to VS 2017 project
* Updated DirectXTex library

### June 30, 2017
* Added ``-ignoresrgb`` switch to control handling of sRGB metadata in jpg, png, etc.
* Added support for OpenEXR (via NuGet) to VS 2015 project
* Removed x86 configurations for VS 2015 / VS 2017 projects
* Fixed texture conversion format choice for ``.hdr`` file format

### April 25, 2017
* Updated DirectXTex, UVAtlas, DirectXMesh libraries
* Added VS 2017 projects

### November 9, 2016
* Renamed ``-fl`` switch to ``-featurelevel``
* Added ``-vertexcolortype`` switch to control vertex color format
* Added ``rgba_snorm``, ``rgba_10``, ``r11g11b10``, and ``rgba_s10`` (Xbox One only) to ``-compressedvertextype``
* Updated DirectXTex, DirectXMesh libraries
* Code cleanup

### September 14, 2016
* Updated DirectXTex, UVAtlas, DirectXMesh libraries
* Added HDR file format support for textures

### July 13, 2016
* Updated DirectXTex, UVAtlas, DirectXMesh libraries
* Added ``-normalmaps`` macro to set up for exporting meshes using normal mapping

### April 27, 2016
* Updated DirectXTex, UVAtlas, DirectXMesh libraries
* Updated for Autodesk FBX SDK 2017

### November 20, 2015
* Added warning if using ``-materialcolors+`` with black diffuse colors

### November 5, 2015
* Add warning if attempting to convert non-multiple-of-4 images to compressed DDS
* Changed ``-forcetextureoverwite`` default to true

### October 30, 2015
* Updated with VS 2015 support using the Autodesk FBX SDK 2016.1 or later
* Added new command-line option:
    * ``-flipz`` (default+) to control negation of z component for LH vs. RH view coordinates
* Fixed ``-applyglobaltrans`` to transform exported normals
* Updated DirectXTex, UVAtlas, DirectXMesh libraries    
* Minor code cleanup

### July 29, 2015
* Retired VS 2010 projects

### July 8, 2015
* Added command-line options:
    * ``-materialcolors`` (default+) to control export of material lambert/phong colors
    * ``-lightmaps`` macro to set up for exporting meshes using lightmap textures
* Fixed problems with export of multiple uv sets and per-vertex colors
* Added some more warnings related to uv sets
* Updated DirectXTex, UVAtlas, DirectXMesh libraries    

### June 25, 2015
* Added new command-line options:
    * ``-useemissivetexture[+|-]`` Use EmissiveMapTexture as SpecularMapTexture
    * ``-defaultspecmap [name]`` Sets the default specular map texture name
* Changed defaults for ``-defaultdiffusemap`` and ``-defaultnormalmap`` to blank
* ``SDKMESH`` writer now fills in the SpecularTexture string in the material
* Change the log messages about applying the default texture names to log level 2 messages rather than warnings

### May 6, 2015
* Added command-line options:
    * ``-optimizemeshes`` (default-) applies a vertex cache optimization to the VB/IB controlled by -vcache/-restart
    * ``-cleanmeshes`` (default+ if using optimizemeshes, otherwise -) performs back-facing and/or bowtie cleanup
    * ``-applyglobaltrans`` (default-) applies the global transformation matrix on vertex data if not exporting animations
    * ``-tangentsindex`` (default 0) controls which set of texture coordinates are used when computing tangents & binormals
    * ``-gsadjacency`` (default-) controls how adjacency is computed for cleanup, optmization, and UV atlas operations
    * ``-texturebgra`` (default-) controls how uncompressed processed textures are written (BGRA vs. RGBA)
    * ``-exportcolors`` (default+) controls export of per vertex colors
* Removed support for -compressednormaltype dec3n as there is no DXGI equivalent (i.e. legacy Direct3D 9 only)
* Removed all dependencies on the legacy DirectX SDK and D3DX9
    * Makes use of DirectXMath, DirectXMesh, DirectXTex, and UVAtlas instead

### April 16, 2015
* Updated with VS 2012 support using the Autodesk FBX SDK 2014.1 or later
* Updated with VS 2013 support using the Autodesk FBX SDK 2015.1 or later

### April 30, 2012
* Updated support using the Autodesk FBX SDK 2011.3.1
* Updated with VS 2010 toolset support
* Last version to support VS 2008

### DirectX SDK (June 2010)
* Updated with VS 2010 support (still uses VS 2008 toolset) using the Autodesk FBX SDK 2010.2

### DirectX SDK (February 2010)
* Minor bug fixes
* Last version to support VS 2005

### DirectX SDK (March 2009)
* Original release supporting VS 2005/2008 using the Autodesk FBX SDK 2009.1
