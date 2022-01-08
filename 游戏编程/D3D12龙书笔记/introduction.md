
Direct3D 12 起始平台为win10， 特别的，对win7支持，见下面“on win7”章节说明.原生支持语言为C++

## 资料链接

[Direct3D 12 编程指南 ](https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/directx-12-programming-guide?redirectedfrom=MSDN)   

[Direct3D 12 sample programs](https://github.com/Microsoft/DirectX-Graphics-Samples)

## Linking the DirectX Libraries

```c++
// Link necessary d3d12 libraries.
#pragma comment(lib,“d3dcompiler.lib”)
#pragma comment(lib, “D3D12.lib”)
#pragma comment(lib, “dxgi.lib”)
```
## Left-Handed Versus Right-Handed Coordinate Systems

Direct3D使用左手系， opengl使用右手系


## on  win7

To  support game developers at larger scales, we are publishing the following resources to allow game developers to run their DirectX 12 games on Windows 7. Please post technical question or feedback to our Discord channel at http://discord.gg/directx.

- [Development Guidance Document](https://microsoft.github.io/DirectX-Specs/d3d/D3D12onWin7.html) – please read through this document before planning and coding
- [D3D12onWin7 NuGet package](https://www.nuget.org/packages/Microsoft.Direct3D.D3D12On7), which contains header file, binary files, and license terms to unblock coding
- [D3D12 sample](https://github.com/microsoft/DirectX-Graphics-Samples/tree/develop/Samples/Desktop/D3D12On7), which runs on both Win7 and Win10 with the same binary
- For anything else, visit the [DirectX Landing Page](https://devblogs.microsoft.com/directx/landing-page/) for the full list of resources a DirectX developer might find useful.
