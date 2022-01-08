

Pipeline State Object:  PSO

## 5.3 BASIC COMPUTER COLOR

- 128-Bit Color:  四维向量$(r, g, b, a$)，
  
- 32-Bit Color： $32bit$大小的类型存储颜色，即每个分量占$8bit$。每$8$位占据一个颜色分量。具体你也可以去看DirectXMath的XMCOLOR类型。在XMCOLOR中，颜色值是一个按照$ARGB$顺序(从高位到低位)来的：
![](https://github.com/LinkClinton/IGP-DirectX12-Chinese/raw/master/Images/5.10.png)

# 5.4 OVERVIEW OF THE RENDERING PIPELINE

图片5.11解释了渲染管线的组成，虽然出现了GPU资源，但是他并不属于渲染管线。 从GPU资源指向渲染管线的某个阶段的箭头表示这个阶段可以读取这些资源.，例如我们在像素着色器阶段读取纹理信息。 而从某个阶段指向GPU资源的箭头就表示这个阶段可以写入这些资源，例如我们在输出合并阶段向后台缓冲和深度缓冲中写入像素信息。 

![](https://github.com/LinkClinton/IGP-DirectX12-Chinese/raw/master/Images/5.11.png)




## 5.5 THE INPUT ASSEMBLER STAGE

第一个阶段是Input-Assembler (IA)，用于把用户自定义的顶点和索引缓存收集起来并组装成图元，比如线，三角形，或多边形。


这个阶段从内存里读取图形数据，即顶点数据和索引数据，然后使用这些数据取组成一些基础图形，例如三角形和线。

顶点是以一个叫做顶点缓冲区的Direct3D数据结构的形式绑定到图形管线的。顶点缓冲区只是在连续的内存中存储了一个顶点列表。它并没有说明以何种方式组织顶点，形成几何图元。例如，是应该把顶点缓冲区中的每两个顶点解释为一条直线，还是应该把顶点缓冲区中的每三个顶点解释为一个三角形？我们通过指定图元拓扑来告诉Direct3D以何种方式组成几何图元

```c++
void ID3D12GraphicsCommandList::IASetPrimitiveTopology(
        D3D_PRIMITIVE_TOPOLOGY Topology);
```        
这个函数会设置构成图形的方式，即我们使用什么基础图形来组合我们要绘制的图形。,例如：

- Point List(D3D_PRIMITIVE_TOPOLOGY_POINTLIST): 直接绘制顶点
- Triangle List(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST): 将给点的点按照顺序三个三个一组构成三角形

- 点列表（point list）由D3D11_PRIMITIVE_TOPOLOGY_POINTLIST标志值表示。当使用点列表时，每个顶点都会被绘制为一个独立的点
- 线带（line strip）由D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP标志值表示。当使用线带时，前后相邻的两个顶点会形成一条直线（参见图5.13b）；这样，n+1个顶点可以形成n条直线
- 线列表（line list）由D3D11_PRIMITIVE_TOPOLOGY_LINELIST标志值表示。当使用线列表时，每两个顶点会形成一条独立的直线
- 三角形带（triangle strip）由D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP标志值表示。当使用三角形带时，顶点会按照图5.13d所示的带状方式形成连续的三角形
- 三角形列表（triangle list）由D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST标志值表示。当使用三角形列表时，每三个顶点会形成一个独立的三角形
- 

所有的绘图操作以当前设置的图元拓扑方式为准。在没有改变拓扑方式之前，当前设置的拓扑方式会一直有效。下面的代码说明了一点：
```c++
md3dDevice->IASetPrimitiveTopology(
    D3D11_PRIMITIVE_TOPOLOGY_LINELIST); 
/* ...draw objects using line list... */ 
md3dDevice->IASetPrimitiveTopology(
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 
/* ...draw objects using triangle list... */ 
md3dDevice->IASetPrimitiveTopology(
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); 
/* ...draw objects using triangle strip... */ 
```

## 5.6 THE VERTEX SHADER STAGE

Vertex Shader (VS)用于把顶点数据从物体空间转变为摄像机的裁剪空间，同样可用于骨骼动画或者逐顶点光照。

在完成图元装配后，顶点将被送往顶点着色器（vertex shader）阶段。顶点着色器可以被看成是一个以顶点作为输入输出数据的函数。每个将要绘制的顶点都会通过顶点着色器推送至硬件；实际上，我们可以概念性地认为在硬件上执行了如下代码, 其中VertexShader过程是由我们自己来给定的(使用HLSL编写)：

```c++
    for (UINT i = 0; i < numVertices; ++i)
        outputVertex[i] = VertexShader(inputVertex[i]);
```

## Hull Shader (HS) 

HS用于决定输入的patch有多少应该进入Tessellation阶段。

## Tessellator Stage 

Tessellator Stage根据tessellation factors 将patch中的图元划分成更小的图元。

## The Domain Shader (DS) optional
The Domain Shader (DS) stage is an optional shader stage and it computes the final vertex attributes based on the output control points from the hull shader and the interpolation coordinates from the tesselator stage [14]. The input to the domain shader is a single output point from the tessellator stage and the output is the computed attributes of the tessellated primitive.

## Geometry Shader (GS) optional

Geometry Shader (GS)是可选的，输入是单个图元（一个顶点则是代表一个点，三个顶点则是三角形，两个顶点则一条线），然后丢弃这个图元，把这个图元转换为其他类型的图元，或者生成新的图元。

## Stream Output (SO) optional

Stream Output (SO) 是个可选的固定管线，可以把图元数据返回到GPU中，这在粒子效果中很有用。

## Rasterizer Stage (RS)

Rasterizer Stage (RS) 同样是固定管线阶段，用于Culling，即把屏幕看不到东西都丢弃，也用于把逐顶点属性插值并传递给玄素着色器。

## Pixel Shader (PS)
Pixel Shader (PS)

## Output-Merger (OM)
Output-Merger (OM) stage combines the various types of output data (pixel shader output values, depth values, and stencil information) together with the contents of the currently bound render targets to produce the final pipeline result.
