


If a user’s hardware did not support a certain feature level(e.g. D3D_FEATURE_LEVEL_11_0), the application could
fallback to an older feature level

[DirectX Graphics Infrastructure (DXGI)](https://docs.microsoft.com/zh-cn/windows/win32/direct3ddxgi/d3d10-graphics-programming-guide-dxgi?redirectedfrom=MSDN) is an API used along with Direct3D.

![DXGI 的用途是与内核模式驱动程序和系统硬件进行通信](https://docs.microsoft.com/zh-cn/windows/win32/direct3ddxgi/images/dxgi-dll.png)

A system can have several adapters (e.g., if it has several graphics cards). An adapter is represented by the
IDXGIAdapter interface. We We can enumerate all the adapters on a system with the
following code:
```c++
void D3DApp::LogAdapters()
{
    UINT i = 0;
    IDXGIAdapter* adapter = nullptr;
    std::vector<IDXGIAdapter*> adapterList;
    while(mdxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);
        std::wstring text = L”***Adapter: “;
        text += desc.Description;
        text += L”\n”;
        // An example of the output from this method is the following:
        //***Adapter: NVIDIA GeForce GTX 760
        //***Adapter: Microsoft Basic Render Driver
        OutputDebugString(text.c_str());
        adapterList.push_back(adapter);
        ++i;
    }
    for(size_t i = 0; i < adapterList.size(); ++i)
    {
        LogAdapterOutputs(adapterList[i]);
        ReleaseCom(adapterList[i]);
    }
}
```

A system can have several monitors. A monitor is an example of a display output. An
output is represented by the IDXGIOutput interface. Each adapter is associated with a
list of outputs. Note that, per the documentation, the “Microsoft Basic Render Driver” has no display
outputs. We can enumerate all the outputs
associated with an adapter with the following code:
```c++
void D3DApp::LogAdapterOutputs(IDXGIAdapter* adapter)
{
    UINT i = 0;
    IDXGIOutput* output = nullptr;
    while(adapter->EnumOutputs(i, &output) !=
    DXGI_ERROR_NOT_FOUND)
    {
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);
        std::wstring text = L”***Output: “;
        text += desc.DeviceName;
        text += L”\n”;
        OutputDebugString(text.c_str());
        LogOutputDisplayModes(output,
        DXGI_FORMAT_B8G8R8A8_UNORM);
        ReleaseCom(output);
        ++i;
    }
}
```
## Resources and Descriptors

Descriptors have a type, and the type implies how the resource will be used. The types
of descriptors we use in this book are:
- 1. CBV/SRV/UAV descriptors describe constant buffers, shader resources and unordered access view resources.
- 2. Sampler descriptors describe sampler resources (used in texturing).
- 3. RTV descriptors describe render target resources.
- 4. DSV descriptors describe depth/stencil resources.

A view is a synonym for descriptor. The term “view” was used in previous
versions of Direct3D, and it is still used in some parts of the Direct3D 12 API.
We use both interchangeably in this book; for example, constant buffer view and constant buffer descriptor mean the same thing.

descriptor heap是个descriptor array。存储同类型的descriptors



## The Command Queue and Command Lists

### CommandQueue

命令队列（CommandQueue）：在GPU中，是一块环形缓冲区，是GPU的命令执行队列，每个GPU至少维护一个CommandQueue。如果该队列为空，GPU将空闲空转，等到有指令过来；如果该队列满了，将阻塞CPU的执行。

在历史版本中，与D3D12 CommandQueue类似作用的名字叫DeviceContext。这个DeviceContext名字不太合适，原因是：
- 现代的显卡上或者说GPU中，已经包含多个可以同时并行执行命令的引擎了（可以理解为执行某类指令的专用微核）。

- 比如支持D3D12的GPU中至少就有执行3D命令的引擎，执行复制命令的引擎（就是从CPU内存中复制内容到显存中或反之或GPU内部以及引擎之间），执行通用计算命令的引擎（执行Computer Shader的引擎）以及可以进行视频编码解码的视频引擎等。而在D3D12中针对这些不同的引擎，就需要创建不同的命令队列接口来代表不同的引擎对象了。 比如创建类型为D3D12_COMMAND_LIST_TYPE_DIRECT的CommandQueue，它是图形设备的“直接”代表物，本质上还可以执行几乎所有的命令，包括图形命令、复制命令。


### CommandList

- 命令列表（CommandList）：在CPU中，用来记录GPU的执行指令，我们期望让GPU执行的任务会通过它来记录。有下面类型：
    - D3D12_COMMAND_LIST_TYPE_DIRECT: directly be executed by the GPU
    - D3D12_COMMAND_LIST_TYPE_BUNDLE: bundles should be recorded at initialization time

- 命令分配器（CommandAllocator）：在CPU中，用来给CommandList记录的指令分配空间，这个空间用来在CPU侧存储指令，不存储资源。

CommandList有很多方法加入指令，例如设置ViewPort,清理Render Target。例如：
```c++
    CommandList->RSSetViewports(1, &myViewport);
    CommandList->ClearRenderTargetView(CurrentBackBuffer, Color, 0, nullptr);
    CommandList->DrawIndexedInstanced(36, 1, 0, 0, 0);
```

CommandQueue有一个成员函数ExecuteCommandLists就是可以将CommandList提交到CommandQueue中去。
```c++
void ID3D12CommandQueue::ExecuteCommandLists(
    UINT Count, //要提交的指令列表的数量
    ID3D12CommandList *const *ppCommandLists); //指令列表的数组
```

## CPU/GPU Synchronization

### fence
A fence is represented by the ID3D12Fence interface and is used to synchronize the GPU and CPU. it force the CPU to wait until the GPU has finished processing all the commands in the queue up to a specified fence point.

例子：
```c++
    void Device::GPUFlush(D3D12_COMMAND_LIST_TYPE queueType)
    {
        ID3D12Fence *pFence;
        ThrowIfFailed(m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence)));

        ID3D12CommandQueue* queue = (queueType == D3D12_COMMAND_LIST_TYPE_COMPUTE) ? GetComputeQueue() : GetGraphicsQueue();

        ThrowIfFailed(queue->Signal(pFence, 1));

        HANDLE mHandleFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        pFence->SetEventOnCompletion(1, mHandleFenceEvent);
        WaitForSingleObject(mHandleFenceEvent, INFINITE);
        CloseHandle(mHandleFenceEvent);

        pFence->Release();
    }
```

## Resource Transitions

D3D12将资源状态管理从图形API层移交到应用层，迫使我们自己来管理资源的状态，我们不仅要正确的使用资源状态转换，还要保证转换的性能

Direct3D associates a state to resources. state defined in "enum D3D12_RESOURCE_STATES"

For example, if we are writing to a resource, say a texture, we will set the texture state to a render target state; when we need to read the texture, we will change its state to a shader resource state.


# INITIALIZING DIRECT3D

初始化DirectX12了。大致步骤如下：
- 开启D3D12调试层。创建设备。
- 创建围栏，同步CPU和GPU。
- 获取描述符大小。
- 设置MSAA抗锯齿属性。
- 创建命令队列、命令列表、命令分配器。
- 创建交换链。
- 创建描述符堆。
- 创建描述符。
- 资源转换。
- 设置视口和裁剪矩形。
- 设置围栏刷新命令队列。
- 将命令从列表传至队列。

注意：最后两个放在绘制部分

常用描述符descriptor：

- RTV（渲染目标缓冲区描述符）、
- DSV（深度模板缓冲区描述符）、
- CBV（常量缓冲区描述符）
- SRV（着色器资源缓冲描述符）
- UAV（随机访问缓冲描述符）

注意描述符在Draw时绑定到CommandList

```c++
bool D3DApp::InitDirect3D()
{
#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
{
	ComPtr<ID3D12Debug> debugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	debugController->EnableDebugLayer();
}
#endif


	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));

    CreateDevice();

//通过设备创建围栏fence，以便之后同步CPU和GPU
    CreateFence();

//获取描述符大小，这个大小可以让我们知道描述符堆中每个元素的大小（描述符在不同的GPU平台上的大小各异），方便我们之后在地址中做偏移来找到堆中的描述符元素
    GetDescriptorSize();

//设置MSAA抗锯齿属性。先填充多重采样属性结构体，然后通过CheckFeatureSupport函数设置NumQualityLevels。注意：此处不使用MSAA，采样数量设置为0。
    // Check 4X MSAA quality support for our back buffer format.
    // All Direct3D 11 capable devices support 4X MSAA for all render 
    // target formats, so we only need to check quality support.
    SetMSAA();
	
#ifdef _DEBUG
    LogAdapters();
#endif
//创建命令队列、命令列表和命令分配器。他们三者的关系是：首先CPU创建命令列表，然后将关联在命令分配器上的命令传入命令列表，最后将
//命令列表传入命令队列给GPU处理。这一步只是做了三者的创建工作。注意，我们在初始化D3D12_COMMAND_QUEUE_DESC时，
//只初始化了两项，其他两项我们在大括号中默认初始化了。
	CreateCommandObjects();
    CreateSwapChain();
    //有了描述符堆(heap)之后，我们才可以创建堆中的描述符，比如RTV，DSV
    CreateRtvAndDsvDescriptorHeaps();

	return true;
}
```

## CreateDevice

DX12 device用于创建资源，比如纹理和缓存，指令列，指令队列，fence，heaps等。DX12 device不会直接用于发布绘制或dispatch指令。DX12 device可用被认为是一个memory context用于追踪GPU显存上的分配。摧毁DX12 device将会让所有通过这个device分配下的资源都变得无效，然后调试层就会发出警示信息。

```c++
void CreateDevice()
{
	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&md3dDevice));

	// Fallback to WARP device.
	if(FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&md3dDevice)));
	}
}
```
### 调试信息

为了让开发者能够修复由调试层抛出的错误，dx12 device提供了ID3D12InfoQueue接口，用于断点和过滤需要的信息。ID3D12InfoQueue接口使用ID3D12Device使用ComPtr::As查询获得
```c++
  // Enable debug messages in debug mode.
#if defined(_DEBUG)
    ComPtr<ID3D12InfoQueue> pInfoQueue;
    if (SUCCEEDED(md3dDevice.As(&pInfoQueue)))
    {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

```

### 枚举适配器创建设备对象

对于现代的很多PC系统来说CPU中往往集成了显卡，同时系统中还会有一个独立的显卡。另外大多数笔记本系统中，为节能之目的，往往会把集显作为默认的显示适配器，而由于集显功能性能限制的问题，所以在有些示例中可能会引起一些问题，尤其是将来准备使用DXR渲染的时候。

    所以基于这样的原因，这里就使用比较繁琐的枚举显卡适配器的方式来创建3D设备对象。另外这也是为将来使用多显卡渲染示例的需要做准备的。代码如下：

```c++
for(UINT adapterIndex=0;
    DXGI_ERROR_NOT_FOUND !=pIDXGIFactory->EnumAdapters1(adapterIndex,&pIAdapter);
    adapterIndex++){
   DXGI_DAPATER_DESC1 desc={};
   pIAdapter->GetDesc1(&desc);
   //skip software dapter
   if(desc.flags & DXGI_DAPATER_FLAG_SOFTWARE) continue;

   //检查对D3D支持的兼容级别，这里直接要求支持12.1的能力，这里注意返回接口的参数设置位了null，
   //这样就不会实际创建一个设备了，也不用再调用release
   if(SUCCEEDED(D3D12CreateDevice(pIAdapter.Get(),D3D12_FEATURE_LEVEL_12_1,_uuidof(ID3D12Device),nullpter))){
       break;
   }
}
//创建D3D12.1的设备
ThrowIfFailed(D3D12CreateDevice(
	pIAdapter.Get(),
	D3D_FEATURE_LEVEL_12_1,
	IID_PPV_ARGS(&pID3D12Device)));

```


## CreateFence

```c++
void CreateFence()
{
	ThrowIfFailed(md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&mFence)));
}
```

## GetDescriptorSize

```c++
void GetDescriptorSize()
{
	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
```

## SetMSAA

```c++
void SetMSAA()
{
    // Check 4X MSAA quality support for our back buffer format.
    // All Direct3D 11 capable devices support 4X MSAA for all render 
    // target formats, so we only need to check quality support.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;////UNORM是归一化处理的无符号整数
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
    ///当前图形驱动对MSAA多重采样的支持（注意：第二个参数即是输入又是输出）
	ThrowIfFailed(md3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

    m4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");
}
```

## CreateCommandObjects

同一时间一个指令分配器（CommandAllocator）只能被一个指令队列（CommandQueue）使用，但当这个指令队列里的指令在GPU上执行完成后又可以重新使用，方法是通过 ID3D12CommandAllocator::Reset 函数。

```c++
void D3DApp::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

	ThrowIfFailed(md3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));

	ThrowIfFailed(md3dDevice->CreateCommandList(
		0,//掩码值为0，单GPU
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCmdListAlloc.Get(), // Associated command allocator
		nullptr,                   // Initial PipelineStateObject //流水线状态对象PSO，这里不绘制，所以空指针
		IID_PPV_ARGS(mCommandList.GetAddressOf()))); ////返回创建的命令列表

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	mCommandList->Close();
}
```

## CreateSwapChain

可以将交换链理解为一个有很多页画纸的画板，是3D渲染的目标。而渲染管线就是画笔颜料等等，虽然他们要组合在一起才能绘画，但本质上是独立的东西，因为画纸（交换链）我们还可以使用完全不同的别的笔来写字或绘画，比如交换链还可以用于D2D、DirectWrite绘制等。

另外在D3D12中具体创建交换链时就需要指定一个命令队列（CommandQueue），这也是最终呈现画面前，交换链需要确定绘制操作是否完全完成了，也就是需要这个命令队列最终Flush（刷新）一下。

```c++
void D3DApp::CreateSwapChain()
{
    // Release the previous swapchain we will be recreating.
    mSwapChain.Reset();

    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = mClientWidth;//缓冲区分辨率的宽度
    sd.BufferDesc.Height = mClientHeight;//缓冲区分辨率的高度
    sd.BufferDesc.RefreshRate.Numerator = 60;//刷新率的分母
    sd.BufferDesc.RefreshRate.Denominator = 1;//刷新率的分子
    sd.BufferDesc.Format = mBackBufferFormat;//缓冲区的显示格式
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//图像相对屏幕的拉伸（未指定的）
    /**
     * 当back buffer的大小和render output的大小不一致时的缩放行为,包括：
     * DXGI_MODE_SCALING_STRETCHED: 拉伸
     * DXGI_MODE_SCALING_UNSPECIFIED: 不拉伸
     * DXGI_MODE_SCALING_CENTERED: 拉伸但是保持宽高比
    */
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;//逐行扫描VS隔行扫描(未指定的)
    sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
    sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//将数据渲染至后台缓冲区（即作为渲染目标）
    sd.BufferCount = SwapChainBufferCount;	//后台缓冲区数量（e.g. 2表示双缓冲）
    sd.OutputWindow = mhMainWnd;//渲染窗口句柄
    sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//固定写法
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//自适应窗口模式（自动选择最适于当前窗口尺寸的显示模式）

	// Note: Swap chain uses queue to perform flush.
    ThrowIfFailed(mdxgiFactory->CreateSwapChain(
		mCommandQueue.Get(),
		&sd, 
		mSwapChain.GetAddressOf()));
}
```

交换链至少储存两个buffer，一个用于渲染的backbuffer，一个用于显示到屏幕上的frontbuffer。

![示意](https://img-blog.csdnimg.cn/20200623091830728.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQzNDM5MjQw,size_16,color_FFFFFF,t_70)

## CreateRtvAndDsvDescriptorHeaps

escriptor heap 可以被认为是resource views数组。 在DX12中，在resource view创建之前 (such as Render Target Views (RTV), Shader Resource Views (SRV), Unordered Access Views (UAV), or Constant Buffer Views (CBV))，必须先创建descriptor heap。后三种resouce view可以被存储在同一个heap中，但RTV需要一个独立的heap。

```c++
void D3DApp::CreateRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;//值为0代表单个GPU。多个GPU，则值为执行此任务的GPU的ID。
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
        &rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));


    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
        &dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}
```



### CreateRTV

首先在heap上创建RTV。具体过程是，先从RTV堆中拿到首个RTV句柄，然后获得存于交换链中的RT资源，
最后创建RTV将RT资源和RTV句柄联系起来，并在最后根据RTV大小做了在堆中的地址偏移。

下面的例子创建了2个RTV描述符，放在swapChainBuffer
```c++
void CreateRTV()
{
    //单个descriptor的大小会由于显卡供应商的不同而不同，使用下面方式来获得这个值
    auto rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    //管理描述堆的当前元素指针位置，虽然名字是HANDLE，但理解为iterator估计更合适
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
    ComPtr<ID3D12Resource> swapChainBuffer[2];
    for (int i = 0; i < 2; i++)
    {
        //获得存于交换链中的后台缓冲区资源
        swapChain->GetBuffer(i, IID_PPV_ARGS(swapChainBuffer[i].GetAddressOf()));
        //创建RTV
        d3dDevice->CreateRenderTargetView(
            swapChainBuffer[i].Get(),//A pointer to the ID3D12Resource object that represents the render target.
            nullptr,//NULL，表示创建默认descriptor。在本例中在交换链创建时已经定义了该资源的数据格式，所以这里指定为空指针
            rtvHeapHandle);	//the destination where the newly-created render target view will reside.
        //偏移到描述符堆中的下一个缓冲区
        rtvHeapHandle.Offset(1, rtvDescriptorSize);
    }
}
```


### CreateDSV

创建完RTV后，我们就要在heap上创建DSV了。具体过程是，先在CPU中创建好DS资源，然后通过CreateCommittedResource函数将DS资源
提交至GPU显存中，最后创建DSV将显存中的DS资源和DSV句柄联系起来。
```c++
void CreateDSV()
{
    //在CPU中创建好深度模板数据资源
    D3D12_RESOURCE_DESC dsvResourceDesc;
    dsvResourceDesc.Alignment = 0;	//指定对齐
    dsvResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	//指定资源维度（类型）为TEXTURE2D
    dsvResourceDesc.DepthOrArraySize = 1;	//纹理深度为1
    dsvResourceDesc.Width = 1280;	//资源宽
    dsvResourceDesc.Height = 720;	//资源高
    dsvResourceDesc.MipLevels = 1;	//MIPMAP层级数量
    dsvResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	//指定纹理布局（这里不指定）
    dsvResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//深度模板资源的Flag
    dsvResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//24位深度，8位模板,还有个无类型的格式DXGI_FORMAT_R24G8_TYPELESS也可以使用
    dsvResourceDesc.SampleDesc.Count = 4;	//多重采样数量
    dsvResourceDesc.SampleDesc.Quality = msaaQualityLevels.NumQualityLevels - 1;	//多重采样质量
    CD3DX12_CLEAR_VALUE optClear;	//清除资源的优化值，提高清除操作的执行速度（CreateCommittedResource函数中传入）
    optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//24位深度，8位模板,还有个无类型的格式DXGI_FORMAT_R24G8_TYPELESS也可以使用
    optClear.DepthStencil.Depth = 1;	//初始深度值为1
    optClear.DepthStencil.Stencil = 0;	//初始模板值为0
    //创建一个资源和一个堆，并将资源提交至堆中（将深度模板数据提交至GPU显存中）
    ComPtr<ID3D12Resource> depthStencilBuffer;
    ThrowIfFailed(d3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),	//堆类型为默认堆（不能写入）
	D3D12_HEAP_FLAG_NONE,	//Flag
	&dsvResourceDesc,	//上面定义的DSV资源指针
	D3D12_RESOURCE_STATE_COMMON,	//资源的状态为初始状态
	&optClear,	//上面定义的优化值指针
	IID_PPV_ARGS(&depthStencilBuffer)));	//返回深度模板资源
    //创建DSV(必须填充DSV属性结构体，和创建RTV不同，RTV是通过句柄)
    //D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    //dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    //dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    //dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    //dsvDesc.Texture2D.MipSlice = 0;
    d3dDevice->CreateDepthStencilView(depthStencilBuffer.Get(),
	nullptr,	//D3D12_DEPTH_STENCIL_VIEW_DESC类型指针，可填&dsvDesc（见上注释代码），
                        //由于在创建深度模板资源时已经定义深度模板数据属性，所以这里可以指定为空指针
	dsvHeap->GetCPUDescriptorHandleForHeapStart());	//DSV句柄
}
```

## 9. Set the viewport and scissor rectangles.

接下来我们设置视口和裁剪矩形。

```c++
void CreateViewPortAndScissorRect()
{
    D3D12_VIEWPORT viewPort;
    D3D12_RECT scissorRect;
    //视口设置
    viewPort.TopLeftX = 0;
    viewPort.TopLeftY = 0;
    viewPort.Width = 1280;
    viewPort.Height = 720;
    viewPort.MaxDepth = 1.0f;
    viewPort.MinDepth = 0.0f;
    //裁剪矩形设置（矩形外的像素都将被剔除）
    //前两个为左上点坐标，后两个为右下点坐标
    scissorRect.left = 0;
    scissorRect.top = 0;
    scissorRect.right = 1280;
    scissorRect.bottom = 720;
}
```




# 绘制

- 设置围栏刷新命令队列。
- 将命令从列表传至队列。

在DX12中，程序员需要使用指令集中的resource barrier来让资源在正确的位置。比如一个交换链的back buffer，如果要作为render target使用，那么它的状态必须是RENDER_TARGET，如果要作为到屏幕上的渲染图片，则需要转换到PRESENT状态。


```c++
void Draw(const GameTimer& gt)
{
    //重置命令分配器cmdAllocator和命令列表cmdList，目的是重置命令和列表，复用相关内存。
    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(mDirectCmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO.Get()));

    //设置视口和裁剪矩形。mScreenViewport等的值在类似onSize中设置
    mCommandList->RSSetViewports(1, &mScreenViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);

    //接着我们将后台缓冲资源从呈现状态转换到渲染目标状态（即准备接收图像渲染）。
    // Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));//从呈现到渲染目标转换

    // 然后清除后台缓冲区和深度缓冲区，并赋值。
    // Clear the back buffer and depth buffer.
    mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);//清除RT背景色为LightSteelBlue，并且不设置裁剪矩形
    mCommandList->ClearDepthStencilView(DepthStencilView(), //DSV描述符句柄
            D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 
            1.0f, //默认深度值
            0,    //默认模板值
            0,    //裁剪矩形数量
            nullptr);//裁剪矩形指针
	
    // 然后我们指定将要渲染的缓冲区，即指定RTV和DSV。
    // Specify the buffers we are going to render to.
	mCommandList->OMSetRenderTargets(
        1, //待绑定的RTV数量
        &CurrentBackBufferView(), //指向RTV数组的指针
        true, //RTV对象在堆内存中是连续存放的
        &DepthStencilView());//指向DSV的指针

    //sets the root signature and CBV heap to the command list
	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	mCommandList->IASetVertexBuffers(0, 1, &mBoxGeo->VertexBufferView());
	mCommandList->IASetIndexBuffer(&mBoxGeo->IndexBufferView());
    mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    mCommandList->SetGraphicsRootDescriptorTable(0, mCbvHeap->GetGPUDescriptorHandleForHeapStart());

    mCommandList->DrawIndexedInstanced(
		mBoxGeo->DrawArgs["box"].IndexCount, 
		1, 0, 0, 0);
	
    // 等到渲染完成，我们要将后台缓冲区的状态改成呈现状态，使其之后推到前台缓冲区显示。完了，关闭命令列表，等待传入命令队列。
    // Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));//从渲染目标到呈现
    //完成命令的记录关闭命令列表
    // Done recording commands.
	ThrowIfFailed(mCommandList->Close());
 
    //渲染中的最后一件事是把渲染好的图像呈现到屏幕上。在此之前，back buffer必须转换成PRESENT状态。
    //等CPU将命令都准备好后，需要将待执行的命令列表加入GPU的命令队列。使用的是ExecuteCommandLists函数。
    // Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };//声明并定义命令列表数组
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);//将命令从命令列表传至命令队列
	
    //然后交换前后台缓冲区索引（这里的算法是1变0，0变1，为了让后台缓冲区索引永远为0）。
	// swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

    //刷新命令队列，使CPU和GPU同步。见FlushCommandQueue()
	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	FlushCommandQueue();
}
```

```c++
void FlushCommandQueue()
{
	// Advance the fence value to mark commands up to this fence point.
    mCurrentFence++;//CPU传完命令并关闭后，将当前围栏值+1

    // Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
    ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));//当GPU处理完CPU传入的命令后，将fence接口中的围栏值+1，即fence->GetCompletedValue()+1

	// Wait until the GPU has completed commands up to this fence point.
    if(mFence->GetCompletedValue() < mCurrentFence)//如果小于，说明GPU没有处理完所有命令
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

        // Fire event when GPU hits current fence.  
        ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));//当围栏达到mCurrentFence值（即执行到Signal（）指令修改了围栏值）时触发的eventHandle事件

        // Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);//等待GPU命中围栏，激发事件（阻塞当前线程直到事件触发，注意此Enent需先设置再等待，如果没有Set就Wait，就死锁了，Set永远不会调用，所以也就没线程可以唤醒这个线程）
        CloseHandle(eventHandle);
	}
}

```

