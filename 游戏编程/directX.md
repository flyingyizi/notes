X包括Graphics，input，play，sound，show，setup，media objects等组件。

因此Direct3D于OpenGL才是对等的比较。

[tutorial](http://www.rastertek.com/)

[Tutorial 1: Setting up DirectX 12 with Visual Studio](http://www.rastertek.com/dx12tut01.html)

[DirectX 12 Programming Guide](https://docs.microsoft.com/en-us/windows/win32/direct3d12/directx-12-programming-guide)

DirectX诊断工具： DxDiag

DirectX功能查看器：DirectX Caps Viewer

纹理转换工具：texconv.exe

## Setting up DirectX 12 with Visual Studio

Now click "View" from the menu and select "Property Manager". Next click on the arrow to the left of your project name (mine was called Engine) in the property manager window on the left part of the screen. This will display the solution configurations you have. Select the "Debug | x64" folder and double click "Microsoft.Cpp.x64.user".

In the window that pops up select "VC++ Directories" from the menu on the left side. This will then display the directories that the project uses.

"Include Directories" add: 

- Windows 10 SDK include folder, like: C:\Program Files (x86)\Windows Kits\10\Include\10.0.10240.0\um
- shared headers. like: C:\Program Files (x86)\Windows Kits\10\Include\10.0.10240.0\shared

"libraries directory" add:

- Navigate to the Windows 10 SDK library folder, like: C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10240.0\um\x64

Go to the DirectX 12 Programming Guide on the Windows Dev Center (msdn) website. Download the [Working Samples](https://github.com/Microsoft/DirectX-Graphics-Samples) and look at HelloWorld sample set.

## A simple framework for rapid prototyping on Vulkan or DirectX12

如果采用cmake构建DirectX项目，可以使用下面的cmake project框架。
[A simple framework for rapid prototyping on Vulkan or DirectX12](https://github.com/GPUOpen-LibrariesAndSDKs/Cauldron)

使用该框架的方法有两个： 将该框架编译为static lib； 或直接源码使用。

下面的指令，演示了生成sln项目，并编译static lib（名字是Framework_DX12.lib）
```
c:\tmp\playstation\libs\cauldron\build>GenerateSolutions.bat
...
c:\tmp\playstation\libs\cauldron\build>cd DX12
c:\tmp\playstation\libs\cauldron\build\DX12>msbuild Cauldron_DX12.sln
```

直接采用源码的方式可能应该更合适，这个可以借鉴Cauldron项目readme中所列的使用该框架的实际项目。

下面的片段演示了使用该框架api构建一个程序。其中SPDSample是FrameworkWindows的继承类。
```c++
int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    LPCSTR Name = "FFX SPD SampleVK v2.0";

    // create new Vulkan sample
    return RunFramework(hInstance, lpCmdLine, nCmdShow, new SPDSample(Name));
}
```

The main features could be grouped in 4 categories:

- **Resource managers & loaders**
  - Texture loaders - *can load `DDS`, `PNG`, `JPG` and any other file supported by the WIC subsystem. It can also create image views.*
  - StaticBufferPool - *holds vertices and indices for static geometry. It creates a huge buffer and then suballocates chunks from it in a linear fashion. Chunks cannot be freed.*
  - DynamicBufferRing - *holds constant buffers and also vertices and indices. Same as before but allocation happens in a ring fashion.*
  - ResourceViewHeaps - *holds the Descriptor Sets.*  
  - UploadHeap - *system memory buffer that holds the texture data to be uploaded to video memory via DMA*
  - CommandListRing - *allocates a number of command buffers from a ring buffer and returns a ready-to-use command buffer.*
  - Device - *all the device related code.*
- **Renderers**
  - GLTF* - *loads and renders the model using a certain technique*
  - PostProcPS/PS - *draws a 2D quad using a custom shader, used by the post processing passes.*
  - ImGUI - *code for driving the ImGUI*
  - Widgets
    - Wireframe - *draws a wireframe cube (used for rendering bounding boxes and light/camera frustums)*
    - Axis - *draws the coordinate axis of a matrix*
- **Vulkan specific helpers & setup code**
  - InstanceVK - *creates an instance and enables the validation layer*
  - Extension helpers 
    - ExtDebugMarkers - *sets up the debug markers*
    - ExtFp16 - *enables FP16 extension*
    - ExtFreeSync2 - *enables FreeSync extension*
    - ExtValidation - *enables the validation layer*
- **Commons & OS-specific code**
  - Swapchain - *handles resizing, fullscreen/windowed modes, etc.*
  - FrameworkWindows - *creates a window, processes windows message pump*
