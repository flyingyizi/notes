


[Learning DirectX 12](https://www.3dgep.com/learning-directx-12-1/)

 (Windows Advanced Rasterization Platform – WARP) or not. The software rasterizer allows the graphics programmer to access the full set of advanced rendering features that may not be available in the hardware (for example, when running on older GPUs). The WARP device can also be used to verify the results of a rendering technique if the quality of the vendor supplied display driver is in question.
 
# DirectX 12 组件

DirectX SDK 实际上是应用程序编程接口 (API) 的集合。

- Direct3D： 与硬件加速 3D 图形渲染有关的 API 称为 Direct3D。

- Direct2D是一种硬件加速的即时模式 2D 图形 API，可为 2D 几何图形、位图和文本提供高性能和高质量的呈现。Direct2D API 旨在与 Direct3D 互操作。

- Direct3D是一种 3D 图形 API，可让您为 3D 几何图形创建高性能渲染。Direct3D API 还允许创建可以利用 GPU 并行性的高性能通用应用程序。Direct3D 是本文的主要主题。当使用 DirectX 12 一词时，它通常是指 Direct3D 12 图形 API。

- DirectWrite 支持高质量的文本呈现、与分辨率无关的轮廓字体以及完整的 Unicode 文本和布局。

- DirectXMath 为单精度浮点向量（2D、3D 和 4D）或矩阵（3×3 和 4×4）上的算术和线性代数运算提供了最佳且可移植的接口。

- XAUDIO2，为游戏提供信号处理和混合基础。XAudio2 取代了 DirectSound。

- XInput， 描述当 Xbox 控制器连接到 Windows 计算机时如何使用 XInput API 与它进行交互。XInput 取代了 DirectInput。

# DirectX 12 Graphics Pipeline

![](https://www.3dgep.com/wp-content/uploads/2014/03/DirectX-11-Rendering-Pipeline.png)


# GPU Synchronization

在 DirectX 12 中，图形程序员必须执行显式 GPU 同步。在之前的版本是不需要的。

## Fence

fence用于同步发布到command queue的commands。它记录了向fence发送最后信号值。建议为每个command queue至少创建一个fence。 多个command queue可以等待同一个fence，但是只允许从单个command queue向fence发信号。An example of performing CPU-GPU synchronization using fences will be shown in the following sections.

重要的是要了解每个command queue必须跟踪它自己的栅栏和栅栏值。

## Command List

command list被用于发布copy, compute (dispatch), or draw commands. 
A Command List is used to issue copy, compute (dispatch), or draw commands. In DirectX 12 commands issued to the command list are not executed immediately like they are with the DirectX 11 immediate context. All command lists in DirectX 12 are deferred; that is, the commands in a command list are only run on the GPU after they have been executed on a command queue.
- D3D12_COMMAND_LIST_TYPE_DIRECT：命令队列可用于执行绘制、计算和复制命令。这是最通用的命令队列类型，将在大多数情况下使用。
- D3D12_COMMAND_LIST_TYPE_COMPUTE：命令队列可用于执行计算和复制命令。
- D3D12_COMMAND_LIST_TYPE_COPY：命令队列可用于执行复制命令。

通常需要单个命令列表来使用单个线程记录 GPU 命令。通常使用主线程记录所有 GPU 命令，在这种情况下只定义了一个命令列表。

## Command Queue

The Command Queue in DirectX 12 has a very simple interface. For most common cases only the ID3D12CommandQueue::ExecuteCommandLists method and the ID3D12CommandQueue::Signal method are used. Let’s look at a simple pseudo-code example of using a command queue.


### 使用命令队列的简单伪代码示例

```
//检查是否已达到围栏的完成值。
method IsFenceComplete( _fenceValue )
    return fence->GetCompletedValue() >= _fenceValue
end method

//停止 CPU 线程，直到达到栅栏值。 
method WaitForFenceValue( _fenceValue )
    if ( !IsFenceComplete( _fenceValue )
        fence->SetEventOnCompletion( _fenceValue, fenceEvent )
        WaitForEvent( fenceEvent )
    end if
end method

//将栅栏值插入命令队列。当命令队列中的值达到时，用于向命令队列发出信号的栅栏将设置它的完成值。
method Signal
    _fenceValue <- AtomicIncrement( fenceValue )
    commandQueue->Signal( fence, _fenceValue )
    return _fenceValue
end method

// 渲染一帧。在达到该帧的前一个栅栏值之前，不要移动到下一帧。
method Render( frameID )
    //首先填充包含渲染场景所需的所有绘制（或计算）命令的命令列表
    _commandList <- PopulateCommandList( frameID )

    //在命令队列上执行生成的命令列表,该调用不会阻塞调用线程
    commandQueue->ExecuteCommandList( _commandList )

    //该Present方法使渲染结果呈现到屏幕上,Present方法返回值返回交换链中要渲染到的下一个后备缓冲区的索引
    _nextFrameID <- Present()

    //该Signal方法会将栅栏值附加到命令队列的末尾
    fenceValues[frameID] = Signal()

    //为了防止资源在呈现到屏幕之前被覆盖，CPU线程需要等待前一帧的fence值到达
    WaitForFenceValue( fenceValues[_nextFrameID] )
    frameID <- _nextFrameID
end method
```

# DirectX 12 Demo

## Query DirectX 12 Adapter

