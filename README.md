![DirectX Logo](https://github.com/walbourn/contentexporter/wiki/Dx_logo.GIF)

# ATG  Samples Content Exporter

http://go.microsoft.com/fwlink/?LinkId=226208

Copyright (c) Microsoft Corporation. All rights reserved.

**June 1, 2020**

The **Samples Content Exporter** was shipped as a sample in the legacy DirectX SDK. It makes use of the [Autodesk FBX SDK](http://autodesk.com/fbx) to import an FBX file and then export the data as an ``.SDKMESH``. The ``.SDKMESH`` format is a runtime geometry format supported by the _DirectX Tool Kit_ for [DX11](http://go.microsoft.com/fwlink/?LinkId=248929) / [DX12](http://go.microsoft.com/fwlink/?LinkID=615561) and [DXUT](http://go.microsoft.com/fwlink/?LinkId=320437), and is used in the Windows sample [Marble Maze](https://docs.microsoft.com/en-us/samples/microsoft/windows-appsample-marble-maze/directx-marble-maze-game-sample/).

This code is designed to build with Visual Studio 2017 ([15.9](https://walbourn.github.io/vs-2017-15-9-update/)), Visual Studio 2019, or clang for Windows v9 or later. It is recommended that you make use of the Windows 10 May 2020 Update SDK ([19041](https://walbourn.github.io/windows-10-may-2020-update-sdk/)).

These components are designed to work without requiring any content from the legacy DirectX SDK. For details, see [Where is the DirectX SDK?](https://aka.ms/dxsdk).

## Directory Layout

* ``ImportFBX\``
    + Contains the main entry point for the command-line exporter tool and code for capturing data from FBX files.

* ``ExportObjects\``
    + Contains a library of support code for the exporter.

* ``SDKMeshFileWriter\``
    + Contains a library of code for writing out an .SDKMESH file from data captured as export objects.

* ``XATGFileWriter\``
    + Contains a library of code for writing out a ``.XATG`` file from data captured as export objects.

> ``.XATG`` was an XML based format used for older Xbox 360 samples

# Documentation

Documentation is available on the [GitHub wiki](https://github.com/walbourn/contentexporter/wiki).

## Notices

All content and source code for this package are subject to the terms of the [MIT License](http://opensource.org/licenses/MIT).

For the latest version of this tool, bug reports, etc. please visit the project site on [GitHub](https://github.com/walbourn/contentexporter/).

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

Use of OpenEXR is subject to it's own license terms, and requires the ZLIB library as well. Use of OpenEXR can be removed by undefining ``USE_OPENEXR`` from the  project and removing the NuGet packages.

    <https://github.com/openexr/openexr/blob/develop/OpenEXR/LICENSE>
    <http://zlib.net/zlib_license.html>

## Build Instructions

Install the Autodesk FBX SDK 2019.2 or later (latest tested version is 2020.0) for Windows VS 2017.

    http://autodesk.com/fbx

Ensure the environment variable ``FBX_SDK`` is set to point to the Autodesk FBX SDK (such as ``C:\Program Files\Autodesk\FBX\FBX SDK\2020.0.1``)

> Because VS 2015, 2017, and 2019 are all binary compatible, you can use older Autodesk FBX SDK release if needed by modifying the project to use the '2015' versions of the libraries.

Open the ``ContentExporter_201?.sln`` from Visual Studio.

Build the solution

The resulting command-line tool is located under ImportFBX in ``x64\Debug_201x`` or ``x64\Release_201x`` as ContentExporter.exe

Usage: ``ContentExporter [options] <filename.fbx>``

## Disclaimer

``.SDKMESH`` has a long-time samples runtime geometry format for Microsoft since the retiring of the
legacy .X file format. It has a number of limitations, and we don't recommend using it as a your
production solution for meshes. It is, however, very useful for samples and itself serves as an
example of such file containers. The documentation on the format can be found [here](https://github.com/walbourn/contentexporter/wiki/SDKMESH).

## Release Notes

* The VS projects do not support building a 32-bit (x86) version of the exporter. x64 native (x64) use is wide-spread for content creation tools and provides much greater memory flexibility.
