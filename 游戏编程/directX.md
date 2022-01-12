X包括Graphics，input，play，sound，show，setup，media objects等组件。

因此Direct3D于OpenGL才是对等的比较。

https://enjoyphysics.cn/Soft/NotXNA

[Learning DirectX 12](https://www.3dgep.com/learning-directx-12-1/)

[IGP-DirectX12-Chinese DirectX12龙书](https://github.com/LinkClinton/IGP-DirectX12-Chinese), [related source code](https://github.com/d3dcoder/d3d12book)

[tutorial](http://www.rastertek.com/)

[Tutorial 1: Setting up DirectX 12 with Visual Studio](http://www.rastertek.com/dx12tut01.html)

[DirectX 12 Programming Guide](https://docs.microsoft.com/en-us/windows/win32/direct3d12/directx-12-programming-guide)

An introduction to writing a simple "Hello Triangle" DirectX 12 application. 

  - https://alain.xyz/blog/raw-directx12
  - https://github.com/alaingalvan/directx12-seed

DirectX诊断工具： DxDiag

DirectX功能查看器：DirectX Caps Viewer

纹理转换工具：texconv.exe

A "VBO" is just a vertex buffer, which in DirectX 11 is a ID3D12Buffer with a bind flag of D3D11_BIND_VERTEX_BUFFER which contains the vertex data.

There is no direct equivalent to the OpenGL "Vertex Attribute Array". In DirectX 11, you just submit a distinct Draw for each group of like state settings (a.k.a material attribute).

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



## What are the difference between IDXGIFactory and other IDXGIFactory version?

- IDXGIFactory1 adds DXGI 1.1 support to the application, which is available on Windows 7, Windows Server 2008 R2, and as an update to Windows Vista with Service Pack 2 (SP2) and Windows Server 2008. DXGI 1.1 provides new functionality like Synchronized Shared Surfaces Support, BGRA etc.

- IDXGIFactory2 adds DXGI 1.2 support and includes methods to create a newer version swap chain IDXGISwapChain1 with more features than IDXGISwapChain and to monitor stereoscopic 3D capabilities.

If you require DirectX 12, use IDXGIFactory4

## cauldron 记录

CAULDRON_DX12::Device::OnCreate

1. factory是 DirectX 12 API 的入口点，可让您找到可用于执行 DirectX 12 命令的adapter。
    创建factory： CreateDXGIFactory2 创建pFactory
2. adpater提供在给定的DirectX装置的物理属性的信息。您可以查询您当前的 GPU、它有多少内存等。
    adapter就是我们平时常说的显卡
    创建adapter：pFactory->EnumAdapters(0, &m_pAdapter) 
3. device是您的DirectX 12 API主入口点，让您使用API的内部零件。这是访问重要数据结构和功能（例如管道、着色器 blob、渲染状态、资源障碍等）的关键。 
   创建device：  D3D12CreateDevice(m_pAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pDevice))
4. 命令队列允许您提交绘制调用，被称为组命令列出，一起为了执行，从而允许GPU保持忙碌并优化其执行速度。命令队列的类型见D3D12_COMMAND_LIST_TYPE
   创建commandQueue： m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pDirectQueue))


