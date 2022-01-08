

# Direct3D interfaces methods for defining, storing, and drawing geometric data

## direct3D如何解释GPU内存中的数据

### 6.1 VERTICES AND INPUT LAYOUTS

we need to provide Direct3D with a description of our vertex structure so that it knows what to do with each component. This description is provided to Direct3D in the form of an input layout description.

An input layout description is simply an array of D3D12_INPUT_ELEMENT_DESC
elements and its size.

```c++
struct D3D12_INPUT_ELEMENT_DESC
{
    /**
     * SemanticName: 一个用于联系元素的字符串。他的值必须在合法的范围内。
     * 我们使用这个来将顶点中的元素映射到着色器的输入数据中去。
    */
    LPCSTR SemanticName;
    /**
     * SemanticIndex: 索引值，具体可以参见图片6.1。例如一个顶点可能会有
     * 不止一个纹理坐标，我们必须区分这些纹理坐标，因此我们就加入了索引值
     * 来区分一个顶点里面的多个纹理坐标。如果一个semanticName没有加上索引
     * 的话就默认为索引值为0。例如POSITION就是POSITION0。
    */
    UINT SemanticIndex;
    /**
     * Format: 附加的数据信息的格式，类型是DXGI_FORMAT。
    */
    DXGI_FORMAT Format;
    /**
     * InputSlot: 指定这个元素从哪个输入口进入，Direct3D支持16个输
     * 入口(0-15)输入顶点数据。对于现在来说，我们只使用0输入口。
    */
    UINT InputSlot;
    /**
     * AlignedByteOffset: 内存偏移量，单位是字节。从顶点结构的开端到
     * 这个元素的开端的字节大小。
    */
    UINT AlignedByteOffset;
    /**
     * InputSlotClass: 现在默认使用D3D12_INPUT_PER_VERTEX_DATA。
    */
    D3D12_INPUT_CLASSIFICATION InputSlotClass;
    UINT InstanceDataStepRate;
};
```
下面的例子演示了，对自定义的顶点数据类型Vertex，是如何告知给direcX vertex shader的：


```c++
/**
 * struct Vertex
 * {
 *     Float3 Pos; // 0-byte offset
 *     Float3 col; // 12-byte offset
 * };
 */
    //DXGI_FORMAT_R32G32B32_FLOAT // 3D 32-bit float vector
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout =   {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_PER_VERTEX_DATA, 0},
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
    ...
    ID3D12PipelineState       *m_pPipelineState = nullptr;
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pPipelineState)));

```

### 6.2 VERTEX BUFFERS
In order for the GPU to access an array of vertices, they need to be placed in a GPU resource (ID3D12Resource) called a buffer.


```c++
    ComPtr<ID3D12Resource> defaultBuffer;

    // Create the actual default buffer resource.
    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

    // In order to copy CPU memory data into our default buffer, we need to create
    // an intermediate upload heap. 
    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(uploadBuffer.GetAddressOf())));


    // Describe the data we want to copy into the default buffer.
    D3D12_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pData = initData;
    subResourceData.RowPitch = byteSize;
    subResourceData.SlicePitch = subResourceData.RowPitch;

    // Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
    // will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
    // the intermediate upload heap data will be copied to mBuffer.
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), 
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
    
    UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

```


# Root Signature

7.6 MORE ON ROOT SIGNATURES
We introduced root signatures in §6.6.5 of the previous chapter.

在概念上，我推荐你将整个GPU的渲染管线理解为一个大的函数执行体，其主要代码就是那些运行于GPU上的Shader程序，而根签名则是说明了这个渲染管线可以传入什么参数。更直白的说因为GPU渲染需要数据（比如：纹理、网格、索引、世界变换矩阵、动画矩阵调色板、采样器等等），而这些数据必须是GPU能够理解的专有格式，这些数据始终是CPU负责加载并传递给GPU的，此时就需要提前告诉GPU要传递那些数据，以及传递到什么地方，传递多少个等信息，而这些信息最终就被封装在了根签名里。实际也就是说根签名也是CPU向GPU传递渲染数据的一个契约。整体上看也就是CPU代码调用GPU渲染管线“函数”进行渲染，因此要传递参数给渲染管线，参数的格式就是根签名定义的。


在我们从CPU加载资源到GPU上时就需要详细指定那些数据从哪个槽（slot）上灌入到哪个序号的寄存器上。

而要达到这个目的就需要在两个方面明确指定这些参数，一方面是从程序代码（CPU侧）调用D3D12相关接口创建资源时指定传输参数（存储槽序号，寄存器序号），另一方面在Shader代码中指定接收参数，并指定Shader代码中具体访问哪个存储槽，哪个寄存器中的数据。或者更准确的说一般Shader中就不用管是哪个Slot了，因为数据肯定都已经到了显存中，Shader中实质关心的只是寄存器和其序号。

或者直接的说根签名就是描述了整个的用于渲染的资源的存储布局。在MSDN官方的描述中也是这样说的：根签名是一个绑定约定，由应用程序定义，着色器使用它来定位他们需要访问的资源。

最终在D3D12中之所以要统一管理这些的目的就是为了方便形成一个统一的管线状态对象（Pipeline States Object PSO），有了管线状态对象，在渲染时，只要资源加载正确，我们只需要在不同的渲染过程间切换设置不同的渲染管线状态对象即可，而在传统的D3D管线编码中，这些工作需要一个个设置管线状态，然后自己编写不同的管线状态管理代码来实现，在代码组织上过于分散和复杂，同时也不利于复杂场景渲染时快速切换不同渲染管线状态的需要。

而根签名对象则是总领一条管线状态对象存储绑定架构的总纲。

