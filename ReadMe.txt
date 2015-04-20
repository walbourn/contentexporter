ATG Samples Content Exporter
----------------------------

Copyright (c) Microsoft Corporation. All rights reserved.

April 16, 2015

The Samples Content Exporter was shipped as a sample in the legacy DirectX SDK. It makes use of the Autodesk FBX SDK to
import an FBX file and then export the data as an SDKMESH. The SDKMESH format is a runtime geometry format supported by
the DirectX Tool Kit and DXUT, and is used in the Windows 8 Store app sample Marble Maze.

All content and source code for this package are subject to the terms of the Microsoft Public License (MS-PL).
<http://opensource.org/licenses/MS-PL>.

For the latest version of the Samples Content Exporter, more detailed documentation, bug reports and
feature requests, please visit the GitHub project.

http://go.microsoft.com/fwlink/?LinkId=226208

----------
Disclaimer
----------

.SDKMESH has a long-time samples runtime geometry format for Microsoft since the retiring of the legacy .X file format.
It has a number of limitations, and we don't recommend using it as a your production solution for meshes. It is, however,
very useful for samples and itself serves as an example of such file containers. The documentation on the format can be
found in the DirectXMesh project wiki.

http://go.microsoft.com/fwlink/?LinkID=324981


---------------
RELEASE HISTORY
---------------

April 16, 2015
    Updated with VS 2012 support using the Autodesk FBX SDK 2014.1 or later
    Updated with VS 2013 support using the Autodesk FBX SDK 2015.1

April 30, 2012
    Updated support using the Autodesk FBX SDK 2011.3.1
    Last version to support VS 2008

DirectX SDK (June 2010)
    Updated with VS 2010 support using the Autodesk FBX SDK 2010.x

DirectX SDK (February 2010)
    Minor bug fixes
    Last version to support VS 2005

DirectX SDK (March 2009)
    Original release supporting VS 2005/2008 using the Autodesk FBX SDK 2009.1
