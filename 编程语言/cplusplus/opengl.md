
https://openglbook.com/

https://learnopengl-cn.github.io/01%20Getting%20started/01%20OpenGL/

[官方文档](http://docs.gl/es2/glTexImage2D)

## opengl 矩阵

### opengl 内置矩阵

opengl内置提供四种矩阵：GL_MODELVIEW，GL_PROJECTION，GL_TEXTURE和GL_COLOR），以及转换例程：glLoadIdentity（） ，glTranslatef（）函数，glRotatef（） ，glScalef（） ，glMultMatrixf（） ，glFrustum（）和glOrtho（）

opengl内置矩阵是采用列存储，这与eigen的默认storage刚好是一样的，因此“Eigen::Matrix.data()”方式传递数据给opengl是完全匹配的。所谓列存储，

```text
std::vector<float> {
m0  m1  m2  m3
m4  m5  m6  m7
m8  m9  m10 m11
m12 m13 m14 m15};

存储为列存储方式的矩阵如下：

m0 m4 m8  m12
m1 m5 m9  m13
m2 m6 m10 m14
m3 m7 m11 m15
```

在opengl中，视图和模型变换是在一个矩阵中进行控制的，这个矩阵就是GL_MODELVIEW_MATRIX,可以使用glGetFloatv函数来获取这个矩阵，而我们所要使用glTranslate*, glRotate*，glScale* 等操作均最后反映到这个矩阵的变化中。

由于无法再在OpenGL可编程管线（GLSL）中使用这些内置矩阵函数。您必须具有自己的marix实现，然后将矩阵数据通过uniform传递给OpenGL着色器。

# 变换

![变换过程](https://learnopengl-cn.github.io/img/01/08/coordinate_systems.png)

变换过程解释：
- 1.局部坐标是对象相对于局部原点的坐标，也是物体起始的坐标。
- 2.下一步是将局部坐标变换为世界空间坐标，世界空间坐标是处于一个更大的空间范围的。这些坐标相对于世界的全局原点，它们会和其它物体一起相对于世界的原点进行摆放。
- 3.接下来我们将世界坐标变换为观察空间坐标，使得每个坐标都是从摄像机或者说观察者的角度进行观察的。
- 4.坐标到达观察空间之后，我们需要将其投影到裁剪坐标。裁剪坐标会被处理至-1.0到1.0的范围内，并判断哪些顶点将会出现在屏幕上。
- 5.最后，我们将裁剪坐标变换为屏幕坐标，我们将使用一个叫做视口变换(Viewport Transform)的过程。视口变换将位于-1.0到1.0范围的坐标变换到由glViewport函数所定义的坐标范围内。最后变换出来的坐标将会送到光栅器，将其转化为片段。
  
- 总结来说： 整个变换过程是 ` gl_Position = projection * view * model * vec4(aPos, 1.0f);`  


## model matrix(局部空间坐标 -> 世界坐标的转换)

  由model matrix 转换"local space" to "world space"  。

  下面是一个典型的model matrix，将物体移动到空间坐标`的cubePositions[i]`位置，然后旋转一个角度`angle`
   ```c++
        float angle = 20.0f * i;
        auto model = Eigen::AngleAxisf( glm::radians(angle), Eigen::Vector3f(0.0f, 0.0f, 0.0f) ) *
                    Eigen::Translation3f(cubePositions[i].x, cubePositions[i].y, cubePositions[i].z) ;
        auto loc=glGetUniformLocation(color_prog.progid, "model");
        color_prog.uniformmat4_set({{ loc, model.matrix()}});
   ```


## view matrix(世界坐标 -> eye坐标系的转换)

这个转换就是从eye的角度解释世界坐标系中物理的坐标，eye坐标系如下：

- VRP: 相机位置,或称为视点eyepoint，或观察参考点(view reference point).下面以"眼睛在哪儿"eye_vec3指示。
- VPN: 相机镜头方向，也称为观察平面法向量(view plane normal)。下面以"看哪里"center_vec3指示
- VUV: 相机顶部正朝向(view up vector)。下面以“头顶指向哪儿”up_vec3指示

上面三个数据都是以世界坐标来定义的， 现在我们来推导eye坐标系:

首先求解：
$$VPN= vec3_{center} - vec3_{eye}$$
$$UP = \frac{vec3_{up}}{||vec3_{up}||}$$

计算得到"+Z轴"的反向：

$$\ n = \frac{VPN}{||VPN||}$$

计算得到"+X轴"：
    
    这是根据向量cross product的含义进行计算

$$u = \frac{n \times UP}{||n \times UP||}$$

计算得到"+Y轴":

$$v = \frac{u \times n}{||u \times n||}$$

上面得到的uvn是左手坐标系统的，在opengl中相机坐标是右手坐标的，因此在采用上述计算构造实际变换矩阵时注意需要将n轴翻转为opengl相机坐标系中的"+Z轴"。

上面已经求解得到了eye坐标系的各个方向法向量(正交)。

我们的最终目标是要构造"view matrix"，即在相机坐标系下解释世界坐标系统中的点。这个变换的逆过程就是：  将相机坐标系变换到与原来的实际坐标系统重合,可以通过旋转/平移两个过程来达到重合。

假设将世界坐标系统原点旋转相机坐标系原点重合的变换是"$M = TR$" .其中$R$是旋转，$T$是平移.因此我们最终要的结果是"$M^{-1} = R^{-1}T^{-1}$"

因为相机的位置是:  $vec3_{eye}$, 显然

$$T=\begin{bmatrix}
        1 & 0 & 0 & eye_x \\   
        0 & 1 & 0 & eye_y \\
        0 & 0 & 1 & eye_z \\   
        0 & 0 & 0 & 1 \\
        \end{bmatrix}$$
因此
$$T^{-1} = \begin{bmatrix}
        1 & 0 & 0 & -eye_x \\   0 & 1 & 0 & -eye_y \\
        0 & 0 & 1 & -eye_z \\   0 & 0 & 0 & 1 \\
        \end{bmatrix}$$


我们上面已经知道相机坐标系原点是[u,v,n] , 世界坐标系原点[0,0,0]. 它们都是基向量，因此无位移动，只表达旋转。因此有下面的等式存在(注意将原点坐标以基向量表达):

 "$ R[0,0,0]=[u,v,n]$",即

 $$  R \begin{bmatrix}
        1 & 0 & 0 & 0 \\   
        0 & 1 & 0 & 0 \\
        0 & 0 & 1 & 0 \\   
        0 & 0 & 0 & 1 \\
        \end{bmatrix} = \begin{bmatrix}
                          u_x & v_x & -n_x & 0 \\    
                          u_y & v_y & -n_y & 0 \\
                          u_z & v_z & -n_z & 0 \\    
                          0 & 0 & 0 & 1 \\
                          \end{bmatrix}$$

 $$ R =  \begin{bmatrix}
              u_x & v_x & -n_x & 0 \\    
              u_y & v_y & -n_y & 0 \\
              u_z & v_z & -n_z & 0 \\    
              0 & 0 & 0 & 1 \\
              \end{bmatrix}$$

最终结果

$$M^{-1} = R^{-1}T^{-1} = \begin{bmatrix}
              u_x & v_x & -n_x & 0 \\    
              u_y & v_y & -n_y & 0 \\
              u_z & v_z & -n_z & 0 \\    
              0 & 0 & 0 & 1 \\
              \end{bmatrix}^{-1} \begin{bmatrix}
                     1 & 0 & 0 & -eye_x \\   
                     0 & 1 & 0 & -eye_y \\
                     0 & 0 & 1 & -eye_z \\   
                     0 & 0 & 0 & 1 \\  \end{bmatrix} 
          = \begin{bmatrix}
              u_x  & u_y  & u_z  &-eye . u \\    
              v_x  & v_y  & v_z  &-eye . v \\    
              -n_x & -n_y & -n_z & eye . n \\    
              0 & 0 & 0 & 1 \\
              \end{bmatrix}$$

下面是根据该算法实现
```c++
    // eye: 在世界坐标中摄像机位置. 眼睛在哪儿
    // center: 在世界坐标中场景原点. 、看哪儿
    // up:  在世界坐标中上向量(Up Vector). 头顶朝哪儿
    //
    // eigen 实现 glm:: lookAt 函数，"返回结果.data()" 作为传递uniform的参数
    // 例子: auto view_eigen=trans::lookAt( Eigen::Vector3f(camX, 0.0f, camZ),... );
    //       glUniformMatrix4fv(glGetUniformLocation(prog, "view"), 1, GL_FALSE,  view_eigen.data());
    Eigen::Matrix<GLfloat,4,4> lookAt(const vec3 &eye, const vec3 &center, const vec3 &up)
    {
        // 得到相机坐标系(u-v-n) ,该坐标系是左手坐标系统。而opgen相机坐标系是右手系
        vec3 n(center - eye);   // n 代表 opgen相机坐标系"+Z轴"反向
        n.normalize();

        vec3 u(n.cross(up));    // u 代表 opgen相机坐标系"+X轴"
        u.normalize();

        vec3 v(u.cross(n));    //  v 代表 opgen相机坐标系"+Y轴"
        v.normalize();

        Eigen::Matrix<GLfloat,4,4> Result;
        Result << 
            u.x(),  u.y(),  u.z(), -u.dot(eye),
            v.x(),  v.y(),  v.z(), -v.dot(eye),
            -n.x(), -n.y(), -n.z(), n.dot(eye),
            0,      0,  0, 1;
        return Result;
    };
```







##  projection matrix(eye坐标系 ->  裁剪坐标的转换(perspective projection matrix / or Orthographic Projection))

在eye坐标系中，已经将世界坐标系统的3D point转换到eye坐标系空间了。 此时我们保持eye坐标系不变，即眼睛看的角度保持不变，我们再左一个转换：3D Point -> 2D point，即我们要做的“eye坐标系 ->  裁剪坐标系”。

投影矩阵会把所有顶点坐标从eye coordinates(观察空间，eye space或view space)变换到裁剪坐标(clip coordinated，属于裁剪空间，clip space)。然后，这些裁剪坐标被变换到标准化设备坐标(normalized device coordinates, NDC，即坐标范围在-1到1之间)，这一步是通过用用裁剪坐标的wc分量除裁剪坐标实现的。

投影有两种方式： 透视投影(perspective projection)， 正交投影(Orthographic Projection)

projection matrix转换矩阵的推导见[推导过程](http://www.songho.ca/opengl/gl_projectionmatrix.html)。

## 各空间介绍

![opengl采用右手直角坐标系](https://pic3.zhimg.com/80/v2-4ba2a170d562a5c4b3714ec43886fc2a_720w.jpg)

顶点着色器运行后，我们可见的所有顶点都为标准化设备坐标(Normalized Device Coordinate, NDC)。也就是说，每个顶点的x，y，z坐标都应该在-1.0到1.0之间，超出这个坐标范围的顶点都将不可见。

将坐标变换为标准化设备坐标，接着再转化为屏幕坐标的过程通常是分步进行的，也就是类似于流水线那样子。在流水线中，物体的顶点在最终转化为屏幕坐标之前还会被变换到多个坐标系统(Coordinate System)。


### 局部空间(Local Space，或者称为物体空间(Object Space))

    由model matrix 转换"local space" to "world space"  。

### 世界空间(World Space)

    从局部变换到世界空间；该变换是由模型矩阵(Model Matrix)实现。模型矩阵是一种变换矩阵，它能通过对物体进行位移、缩放、旋转来将它置于它本应该在的位置或朝向。你可以将它想像为变换一个房子，你需要先将它缩小（它在局部空间中太大了），并将其位移至郊区的一个小镇，然后在y轴上往左旋转一点以搭配附近的房子。

    世界空间的坐标系,注意右手坐标系，以屏幕中心为原点(0, 0, 0)，Z正向是指向你


### 观察空间(View Space，或者称为视觉空间(Eye Space))

    将世界坐标变换到观察空间，该变换由观察矩阵(View Matrix)实现。观察空间就是从摄像机的视角所观察到的空间。而这通常是由一系列的位移和旋转的组合来完成，平移/旋转场景从而使得特定的对象被变换到摄像机的前方。

    当讨论View Matrix时，是在讨论以摄像机的视角作为场景原点时场景中所有的顶点坐标：观察矩阵把所有的世界坐标变换为相对于摄像机位置与方向的观察坐标。

  ![](https://learnopengl-cn.github.io/img/01/09/camera_axes.png)

    
    注意： 视点坐标是以视点为原点，以视线的方向为Z+轴正方向的坐标系。

### 裁剪空间(Clip Space)

    在一个顶点着色器运行的最后，OpenGL期望所有的坐标都能落在一个特定的范围内，且任何在这个范围之外的点都应该被裁剪掉(Clipped)

    投影矩阵(Projection Matrix)将顶点坐标从观察变换到裁剪空间。指定了一个范围的坐标，比如在每个维度上的-1000到1000。投影矩阵接着会将在这个指定的范围内的坐标变换为标准化设备坐标的范围(-1.0, 1.0)。所有在范围外的坐标不会被映射到在-1.0到1.0的范围之间，所以会被裁剪掉。在上面这个投影矩阵所指定的范围内，坐标(1250, 500, 750)将是不可见的，这是由于它的x坐标超出了范围，它被转化为一个大于1.0的标准化设备坐标，所以被裁剪掉了。

### 屏幕空间(Screen Space)

    经过裁剪空间之后，最终的坐标将会被映射到屏幕空间中（使用glViewport中的设定）


## 欧拉角 转换为 方向向量

定义： 
- 绕X轴旋转 roll
- 绕y轴旋转 
-    Z                              Y                                X
 //then yaw                    then pitch                      First roll:
 | cos(yaw), -sin(yaw), 0 |  | cos(pitch), 0, -sin(pitch) |  | 1 ,   0     ,     0      | 
 | sin(yaw),  cos(yaw), 0 |  |     0     , 1,      0      |  | 0 ,cos(roll), -sin(roll) | 
 |    0    ,     0    , 1 |  | sin(pitch), 0,  cos(pitch) |  | 0 ,sin(roll),  cos(roll) | 



|cos(y)cos(p),  -sin(y), -cos(y)sin(p)|  | 1 ,   0     ,     0      |  
|sin(y)cos(p),  cos(y),  -sin(y)sin(p)|  | 0 ,cos(roll), -sin(roll) | 
|sin(p),        0,       cos(p)       |  | 0 ,sin(roll),  cos(roll) | 


| cos(y)cos(p),  -sin(y)cos(r) - cos(y)sin(p)sin(r),  sin(y)sin(r) -cos(y)sin(p)cos(r)   |
| sin(y)cos(p),  cos(y)cos(r) - sin(y)sin(p)sin(r) ,  - cos(y)sin(r) - sin(y)sin(p)cos(r)|
| sin(p),        cos(p)sin(r),                        cos(p)cos(r)                       |



## 变换编码举例

假设在vert.glsl中, 引用了变换矩阵transform
```glsl
...
uniform mat4 transform;
...
void main()
{
    gl_Position = transform * vec4(aPos, 1.0f);
}
```

在外界，通过类似下面的方式将变换矩阵传递给GPU
```c++
  // create transformations
  glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
  transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
  transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
  // get matrix's uniform location and set matrix
  glUseProgram(this->program);
  unsigned int transformLoc = glGetUniformLocation(this->program, "transform");
  glUniformMatrix4fv(transformLoc , 1, GL_FALSE, glm::value_ptr(transform));

  // // 上面等价于下面的eigen实现
  //   auto m = Eigen::Translation3f(0.5f, -0.5f, 0.0f) * Eigen::AngleAxisf((float)glfwGetTime(), Eigen::Vector3f::UnitZ());
  //   glUseProgram(this->program);
  //   unsigned int transformLoc = glGetUniformLocation(this->program, "transform");
  //   glUniformMatrix4fv(transformLoc, 1, GL_FALSE, m.data());
```

这里需要注意的是,在直接将矩阵传递给gpu时，应该通过`Affine transformations`,因为他能得到matrix，从而与opengl mat4对应。 上面的例子能成功，因为是涉及到运算导致结果是matrix。 

```c++
    auto radians=[](float angle)->float{ return  (M_PI * angle) / 180;  };
    Eigen::Matrix4f model = Eigen::Affine3f(Eigen::AngleAxisf( radians(-55.0f), Eigen::Vector3f::UnitX())).matrix();
    Eigen::Matrix4f view = Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0f, -3.0f)).matrix();
    // glm::mat4 projection = glm::mat4(1.0f);
    // projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    auto projection = trans::perspective<float>((M_PI * 45.f) / 180/*radians(45.0f)*/, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, model.data());
    glUniformMatrix4fv(glGetUniformLocation(this->program, "view"), 1, GL_FALSE, view.data());
    glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, GL_FALSE, projection.data());
```

# 颜色

我们在现实生活中看到某一物体的颜色并不是这个物体真正拥有的颜色，而是它所反射的(Reflected)颜色。换句话说，那些不能被物体所吸收(Absorb)的颜色（被拒绝的颜色）就是我们能够感知到的物体的颜色.颜色可以数字化的由红色(Red)、绿色(Green)和蓝色(Blue)三个分量组成，例如"`glm::vec3 coral(1.0f, 0.5f, 0.31f);`". 下面演示如何得到反射的颜色：

```c++
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 toyColor(1.0f, 0.5f, 0.31f);
//定义物体的颜色为物体从一个光源反射各个颜色分量的大小
glm::vec3 result = lightColor * toyColor; // = (1.0f, 0.5f, 0.31f);
```

现实世界的光照是极其复杂的，有限的计算能力所无法模拟的。因此OpenGL的光照使用的是简化的模型。其中一个模型被称为冯氏光照模型(Phong Lighting Model)。冯氏光照模型的主要结构由3个分量组成：环境(Ambient)、漫反射(Diffuse)和镜面(Specular)光照。

## 概念

### 环境光照(Ambient Lighting)：

即使在黑暗的情况下，世界上通常也仍然有一些光亮（月亮、远处的光），所以物体几乎永远不会是完全黑暗的。为了模拟这个，**我们会使用一个环境光照常量，它永远会给物体一些颜色**。
  
  下面演示了如何在片段着色器中，引入环境光照：
  ```c++
      float ambientStrength = 0.1;
      vec3 ambient = ambientStrength * lightColor;

      vec3 result = ambient * objectColor;
      FragColor = vec4(result, 1.0);
  ```    

### 漫反射光照(Diffuse Lighting)：

模拟光源对物体的方向性影响(Directional Impact)。它是冯氏光照模型中视觉上最显著的分量。物体的某一部分越是正对着光源，它就会越亮。

  引入一个物理表面法向量normal，下面演示了如何在片段着色器中，使用法向量 。具体如何使用，见下面对着色器的代码注释  

```c++
  //3D物体的着色器
  lightingShader_prog.load(
      // 代表3D物体的顶点着色器
      R"(
      #version 330 core
      layout (location = 0) in vec3 aPos;    //从顶点数据中获取顶点位置
      layout (location = 1) in vec3 aNormal; //从顶点数据中获取对应顶点的法向量

      out vec3 FragPos;  // 代表3D物体顶点的世界坐标
      out vec3 Normal;

      uniform mat4 model;
      uniform mat4 view;
      uniform mat4 projection;

      void main()
      {
          // 输出该顶点的世界坐标 给3D物体的片段着色器
          FragPos = vec3(model * vec4(aPos, 1.0));
          // 输出该顶点的法向量 给3D物体的片段着色器
          Normal = aNormal;  
          // 通过法线矩阵解决不等比例缩放等导致法线不再垂直的问题
          Normal = mat3(transpose(inverse(model))) * Normal;  

          // 输出渲染3D物体所需的位置信息
          gl_Position = projection * view * vec4(FragPos, 1.0);
      }
  )",
      // 代表3D物体的片段着色器
      R"(
      #version 330 core
      out vec4 FragColor;

      in vec3 Normal;  // 从3D物体的顶点着色器获得
      in vec3 FragPos; // 从3D物体的顶点着色器获得 ，代表3D物体顶点的世界坐标
        
      uniform vec3 lightPos;       // 光源位置，通过uniform获取
      uniform vec3 lightColor;     // 光源色彩，通过uniform获取
      uniform vec3 objectColor;    // 3D物体的颜色，通过uniform获取

      void main()
      {
          // 环境光照设定为光源色彩的0.1 ambient
          float ambientStrength = 0.1;
          vec3 ambient = ambientStrength * lightColor;
          
          // 下面是计算漫反射
          // diffuse 
          vec3 norm = normalize(Normal);  // 注意一定要标准化，不然我们无法通过
          vec3 lightDir = normalize(lightPos - FragPos); //通过向量减法，得到光线方向
          float diff = max(dot(norm, lightDir), 0.0); // 通过点乘得到夹角余弦，显然如何法向量与光线方向一致，那结果就是1
          vec3 diffuse = diff * lightColor;
          // 组合环境光照与漫反射得到最终对3D物体的渲染颜色        
          vec3 result = (ambient + diffuse) * objectColor;
          FragColor = vec4(result, 1.0);
      } 

)");
  // 光源的着色器
  lightCubeShader_prog.load(
      R"(
      #version 330 core
      layout (location = 0) in vec3 aPos;

      uniform mat4 model;
      uniform mat4 view;
      uniform mat4 projection;

      void main()
      {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
      }  
  )",
    R"(
      #version 330 core
      out vec4 FragColor;

      void main()
      {   // 光源的颜色固定为白色
          FragColor = vec4(1.0); // set alle 4 vector values to 1.0
      }
)");

```

  这里还有一个重要的问题，如何对3D物体做了不是等比例的缩放，向量就不会再垂直于对应的表面了，这样光照就会被破坏。修复这个行为的诀窍是使用一个为法向量专门定制的模型矩阵。这个矩阵称之为法线矩阵(Normal Matrix)，它使用了一些线性代数的操作来移除对法向量错误缩放的影响。这个矩阵是如何计算出来的，建议去阅读这个[文章](http://www.lighthouse3d.com/tutorials/glsl-tutorial/the-normal-matrix/)

  存在$T=P_2 - P_1$, 假设有$model_matrix$ ,有$model_matrix T = model_matrix (P_2 - P_1)$. 

### 镜面光照(Specular Lighting)：

模拟有光泽物体上面出现的亮点。镜面光照的颜色相比于物体的颜色会更倾向于光的颜色。

和漫反射光照一样，镜面光照也是依据光的方向向量和物体的法向量来决定的，但是它也依赖于观察方向，例如玩家是从什么方向看着这个片段的。镜面光照是基于光的反射特性。

我们通过反射法向量周围光的方向来计算反射向量。然后我们计算反射向量和视线方向的角度差，如果夹角越小，那么镜面光的影响就会越大。它的作用效果就是，当我们去看光被物体所反射的那个方向的时候，我们会看到一个高光。

在世界空间，还是在观察空间来计算镜面光照是不一样的，显然如果在世界空间计算，那也输入一个观察者位置。 如果是在观察空间计算，那观察位置一定是观察空间原点。

反光度通过下面公式计算: "`float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);`",其中viewDir是视线方向（顶点->眼睛）；reflect是反射方向（顶点->反射路径）。和物理中一般图表达的一样。32次幂结果就是反光度

![](https://learnopengl-cn.github.io/img/02/02/basic_lighting_specular_theory.png)

```c++
//定义一个镜面强度(Specular Intensity)变量，给镜面高光一个中等亮度颜色，让它不要产生过度的影响。
float specularStrength = 0.5;

//计算视线方向向量，和对应的沿着法线轴的反射向量：
vec3 viewDir = normalize(viewPos - FragPos);// 方向是FragPos --> viewPos
//取反。reflect函数要求第一个向量是从光源指向片段位置的向量，但是lightDir当前正好相反
vec3 reflectDir = reflect(-lightDir, norm); //得到反射方向

//计算镜面分量。下面的代码完成了这件事：
//先计算视线方向与反射方向的点乘（并确保它不是负值），然后取它的32次幂。这个32是高光的反光度(Shininess)
float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); //通过点乘得到夹角余弦，显然方向一致，那结果就是1
vec3 specular = specularStrength * spec * lightColor;
```

## 光照贴图	Lighting maps

引入漫反射贴图和镜面光贴图


漫反射贴图(Diffuse Map)（3D艺术家通常都这么叫它），它是一个表现了物体所有的漫反射颜色的纹理图像。


# 深度测试(depth testing)

在绘制3D场景的时候，我们需要决定哪些部分对观察者是可见的，或者说哪些部分对观察者不可见，对于不可见的部分，我们应该及早的丢弃，例如在一个不透明的墙壁后的物体就不应该渲染。这种问题称之为[隐藏面消除](https://en.wikipedia.org/wiki/Hidden_surface_determination)（Hidden surface elimination）,或者称之为找出可见面(Visible surface detemination)。

解决这一问题比较简单的做法是[画家算法](https://en.wikipedia.org/wiki/Painter%27s_algorithm)(painter’s algorithm)。画家算法的基本思路是，先绘制场景中离观察者较远的物体，再绘制较近的物体。例如绘制下面图中的物体(来自[Z buffer 和 W buffer 簡介](https://www.csie.ntu.edu.tw/~r89004/hive/hsr/page_1.html))，先绘制红色部分，再绘制黄色，最后绘制灰色部分，即可解决隐藏面消除问题。

![Z buffer 和 W buffer 簡介](https://img-blog.csdn.net/20160807203914290)

解决隐藏面消除问题的算法有很多，具体可以参考[Visible Surface Detection](http://www.tutorialspoint.com/computer_graphics/visible_surface_detection.htm)。结合OpenGL，我们使用的是Z-buffer方法，也叫深度缓冲区Depth-buffer。

深度缓冲区一般由窗口管理系统，例如GLFW来创建. 深度缓冲区(Detph buffer)同颜色缓冲区(color buffer)是对应的，颜色缓冲区存储的像素的颜色信息，而深度缓冲区存储像素的深度信息。在决定是否绘制一个物体的表面时，首先将表面对应像素的深度值与当前深度缓冲区中的值进行比较，默认逻辑是：如果大于等于深度缓冲区中值，则丢弃这部分;否则利用这个像素对应的深度值和颜色值，分别更新深度缓冲区和颜色缓冲区。这一过程称之为深度测试(Depth Testing)。

深度值的范围在[0,1]之间，值越小表示越靠近观察者，值越大表示远离观察者。

```c++
// 开启
glEnable(GL_DEPTH_TEST);
//另外还需要在绘制场景前，清除颜色缓冲区时，清除深度缓冲区：
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//当前深度值和深度缓冲区中的深度值，进行比较的函数，可以由用户指定
glDepthFunc(GL_LESS); //默认值是GL_LESS	在片段深度值小于缓冲的深度值时通过测试
//与深度缓冲区相关的另一个函数是,它的参数是布尔类型，GL_FALSE将关闭缓冲区写入，默认是GL_TRUE，开启了深度缓冲区写入。
glDepthMask(GL_TRUE);
```

# 基本概念

## opengl 库之间关系

![一张图弄懂opengl的诸多库gl glu glut freeglut glew glfw之间关系](https://img2018.cnblogs.com/blog/1183944/201904/1183944-20190417125324071-1330104680.png)

- gl :opengl原生库 gl,随opengl一起发布

- glu :opengl实用库 glu*,随opengl一起发布

- glut/freeglut :opengl实用工具库glut glut.  对于新学者，用glfw替代它，因为 类似apple公司等都是这样做的。

    glut是基本的窗口界面，是独立于gl和glu的，如果不喜欢用glut可以用MFC和Win32窗口等代替.
    [api 说明](https://blog.csdn.net/deniece1/article/details/102642934)

- glfw :opengl [GLFW库](https://www.glfw.org/docs/latest/) glfw*,跨平台的library
    GLFW is a free, Open Source, multi-platform library for creating windows with OpenGL or OpenGL ES contexts and receiving many kinds of input. It is easy to integrate into existing applications and does not lay claim to the main loop.
    There are guides for each of the various areas of the API.

       - Introduction to the API – initialization, error handling and high-level design
       - Window guide – creating and working with windows and framebuffers
       - Context guide – working with OpenGL and OpenGL ES contexts
       - Monitor guide – enumerating and working with monitors and video modes
       - Input guide – receiving events, polling and processing input

- egl : EGL则是OpenGL ES在嵌入式平台上（WGL,GLX,AGL）的等价物。EGL假设OS会提供窗口系统，但EGL与平台无关，并不局限于任何特定的窗口系统,即是为了移植目的而存在。关联窗口系统：在Windows上是WGL，在Linux上是GLX，在 Apple OS上是AGL等。 
    egl作为 OpenGL 和原生窗口系统之间的桥梁,其主要作用是为 OpenGL 指令 创建 Context 、绘制目标 Surface 、配置 FrameBuffer 属性、Swap 提交绘制结果 等

    ![典型的 EGL 系统布局图](https://pic002.cnblogs.com/img/afilover/201001/2010010922403158.png)

    使用流程
      1. 获取display(即创建连接) :`EGLDisplay eglDisplay(EGLNativeDisplayType displayId);`。 displayId在X-Window下是XDisplay ID，在MS Windows下是Window DC。通常使用EGL_DEFAULT_DISPLAY
      
      2. 初始化egl(即初始化连接) :`EGLBoolean eglInitialize(EGLDisplay display, EGLint *majorVersion, EGLint *minorVersion);`
    
      3. 获取配置 :`EGLBoolean eglChooseChofig(EGLDispay display,const EGLint *attribList,EGLConfig *config,EGLint maxReturnConfigs,ELGint *numConfigs );`
         
         所谓 Config实际指的是FrameBuffer的参数，在MS Windows下对应于PixelFormat，在X-Window下对应Visual。其中attr_list是以EGL_NONE结束的参数数组，通常以id,value依次存放，对于个别标识性的属性可以只有 id，没有value。例如定义`auto attribList = std::vector<EGLint>{EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, EGL_NONE};`,其中Surface类型，可选值为：EGL_WINDOW_BIT，EGL_PIXMAP_BIT，EGL_PBUFFER_BIT

         另一个办法是用`EGLboolean eglGetConfigs(...)` 来获得所有config。

      4. 设置绘制API : 设置当前 thread 的绘制 API,后面创建的 surface 和 context 要与这个 API 相匹配。通过`EGLBoolean EGLAPIENTRY eglBindAPI (EGLenum api)`设置，比如 EGL_OPENGL_API、EGL_OPENGL_ES_API, 或者 EGL_OPENVG_API。

      4. 构造Surface(创建渲染区域) : 可以创建三种surface：windows(onscreen)，pbuffer(offscreen)，pixmap(离屏渲染，但本地渲染API也可以访问)。 对应构造是通过`EGLSurface eglCreateWindowSurface(...); `,`EGLSurface eglCreatePbufferSurface(...); `,`EGLSurface eglCreatePixmapSurface(...); `,

        构造surface后，surface attributes可以通过eglSurfaceAttrib()设置、eglQuerySurface()读取。
   
      5. 创建Context(创建渲染上下文) :  `EGLContext eglCreateContext(EGLDisplay display,EGLConfig config,EGLContext shareContext,const EGLint* attribList);`
    
      6. 关联上下文和渲染区域 :`EGLBoolean eglMakeCurrent(EGLDisplay display,EGLSurface draw,EGLSurface read,EGLContext context);`

      7. 绘制。应用程序通过OpenGL API进行绘制，一帧完成之后，调用eglSwapBuffers(EGLDisplay dpy, EGLContext ctx)来显示。 这个过程通常位于mainloop中。


- glad/glew : opengl GLEW库 glew*,需要下载配置安装.跨平台的C++扩展库，它的出现是为了方便的管理平台与opengl版本不匹配，以及方便的解决不同显卡特有的硬件接口支持。只要包含一个glew.h头文件，你就能使用gl,glu,glext,wgl,glx的全 部函数。 glad是glew的升级版。用哪个都行。就是glew比较老，glad比较新。
- glad :opengl库glad glad*,基于官方规格的多语言GL / GLES / EGL / GLX / WGL装载机 - 生成器。一般结合GLFW使用。

  [GLAD绘制方式](https://blog.csdn.net/zjz520yy/article/details/83904360)

- gl3w : opengl核心文件加载库[gl3w] gl3w*,gl3w是获取OpenGL核心配置文件规范所提供功能的最简单方法。

    gl3w is the easiest way to get your hands on the functionality offered by the OpenGL core profile specification.

    Its main part is a simple gl3w_gen.py Python script that downloads the Khronos supported glcorearb.h header and generates gl3w.h and gl3w.c from it. Those files can then be added and linked (statically or dynamically) into your project.

    The gl3w API consists of just three functions:

    - `int gl3wInit(void)` ：Initializes the library. Should be called once after an OpenGL context has been created. Returns 0 when gl3w was initialized successfully, non-zero if there was an error.
    - `int gl3wIsSupported(int major, int minor)` ：Returns 1 when OpenGL core profile version major.minor is available and 0 otherwise. 
    - `GL3WglProc gl3wGetProcAddress(const char *proc)` ：Returns the address of an OpenGL extension function. Generally, you won't need to use it since gl3w loads all functions defined in the OpenGL core profile on initialization. It allows you to load OpenGL extensions outside of the core profile.


## 渲染过程

![渲染过程](https://learnopengl-cn.github.io/img/01/04/pipeline.png)

- vertex shader :图形渲染管线的第一个部分是顶点着色器(Vertex Shader)，它把一个单独的顶点作为输入。顶点着色器主要的目的是把3D坐标转为另一种3D坐标（后面会解释），同时顶点着色器允许我们对顶点属性进行一些基本处理。

- shape assembly: 图元装配(Primitive Assembly)阶段将顶点着色器输出的所有顶点作为输入（如果是GL_POINTS，那么就是一个顶点），并所有的点装配成指定图元的形状；本节例子中是一个三角形。

- geometry shader: 图元装配阶段的输出会传递给几何着色器(Geometry Shader)。几何着色器把图元形式的一系列顶点的集合作为输入，它可以通过产生新顶点构造出新的（或是其它的）图元来生成其他形状。例子中，它生成了另一个三角形。

- rasterization stage: 几何着色器的输出会被传入光栅化阶段(Rasterization Stage)，这里它会把图元映射为最终屏幕上相应的像素，生成供片段着色器(Fragment Shader)使用的片段(Fragment)。在片段着色器运行之前会执行裁切(Clipping)。裁切会丢弃超出你的视图以外的所有像素，用来提升执行效率。

- fragment shader : 片段着色器的主要目的是计算一个像素的最终颜色，这也是所有OpenGL高级效果产生的地方。通常，片段着色器包含3D场景的数据（比如光照、阴影、光的颜色等等），这些数据可以被用来计算最终像素的颜色。在计算机图形中颜色被表示为有4个元素的数组：红色、绿色、蓝色和alpha(透明度)分量，通常缩写为RGBA。

- alpha and blending :在所有对应颜色值确定以后，最终的对象将会被传到最后一个阶段，我们叫做Alpha测试和混合(Blending)阶段。这个阶段检测片段的对应的深度（和模板(Stencil)）值（后面会讲），用它们来判断这个像素是其它物体的前面

其中“ vertex shader” 与 “fragment shader” 必须由用户提供， GPU中没有默认提供。

### 标准设备坐标 与屏幕空间坐标


一旦你的顶点坐标已经在顶点着色器中处理过，它们就应该是标准化设备坐标(Normalized Device Coordinates, NDC)。 标准化设备坐标是一个x、y和z值在-1.0到1.0的一小段空间。

屏幕空间坐标(Screen-space Coordinates)，这是使用你通过glViewport函数提供的数据，进行视口变换(Viewport Transform)完成的。所得的屏幕空间坐标又会被变换为片段输入到片段着色器中。

### VAO, VBO, EBO

- 顶点数组对象：Vertex Array Object，VAO
- 顶点缓冲对象：Vertex Buffer Object，VBO , 对应对象类型是GL_ARRAY_BUFFER
- 索引缓冲对象：Element Buffer Object，EBO或Index Buffer Object，IBO， 对应对象类型是GL_ELEMENT_ARRAY_BUFFER

### OPENGL如何解释GPU内存中的数据

OpenGL还不知道它该如何解释内存中的顶点数据，以及它该如何将顶点数据链接到顶点着色器的属性上。我们需要告诉OpenGL怎么做。

当我们特别谈论到顶点着色器的时候，每个输入变量也叫顶点属性(Vertex Attribute)

如果每个顶点对应一个顶点属性，每个顶点属性具有3个components(分量)，即VBO 数据希望被解析为下面的样子：
![](https://learnopengl-cn.github.io/img/01/04/vertex_attribute_pointer.png)

我们希望opengl按照上面的样子解析VBO数据，则通过`glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);`指令告诉opengl。

```c++
/**
Parameters
- index :Specifies the index of the generic vertex attribute to be modified.
        它是vertx-glsl中要填充属性(vertex attribute)的index
- size :Specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4. Additionally, the symbolic constant GL_BGRA is accepted by glVertexAttribPointer. The initial value is 4.
        
- type : Specifies the data type of each component in the array. The symbolic constants GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, and GL_UNSIGNED_INT are accepted by glVertexAttribPointer and glVertexAttribIPointer. Additionally GL_HALF_FLOAT, GL_FLOAT, GL_DOUBLE, GL_FIXED, GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV and GL_UNSIGNED_INT_10F_11F_11F_REV are accepted by glVertexAttribPointer. GL_DOUBLE is also accepted by glVertexAttribLPointer and is the only token accepted by the type parameter for that function. The initial value is GL_FLOAT.

- normalized :For glVertexAttribPointer, specifies whether fixed-point data values should be normalized (GL_TRUE) or converted directly as fixed-point values (GL_FALSE) when they are accessed.

- stride :Specifies the byte offset between consecutive generic vertex attributes. If stride is 0, the generic vertex attributes are understood to be tightly packed in the array. The initial value is 0.

- pointer :Specifies a offset of the first component of the first generic vertex attribute in the array in the data store of the buffer currently bound to the GL_ARRAY_BUFFER target. The initial value is 0.
*/
void glVertexAttribPointer(	GLuint index,
 	GLint size,
 	GLenum type,
 	GLboolean normalized,
 	GLsizei stride,
 	const void * pointer);
```

例子：
```c++
GLfloat data[] = {
  //position   //color        //texcoords
  1.0, 0.0,    0.5,0.5,0.5,   0.5,0.5,   
  0.0, 0.0,    0.2,0.8,0.0,   0.0,1.0,   
}

// 对应我们在GPU vert-glsl中定义的属性是
// #version 100
// attribute highp vec2 aPosition;
// attribute highp vec3 aColor;
// attribute highp vec2 aTexcoord;

// 我们需要下面的语句告诉gpu应该如何来解析
glVertexAttribPointer(0, 2, GL_FLOAT,GL_FALSE,
                      7*sizeof(GLfloat), (GLvoid*)0);
glEnableVertexAttribArray(0);

glVertexAttribPointer(1, 3, GL_FLOAT,GL_FALSE,
                      7*sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)));
glEnableVertexAttribArray(1);

glVertexAttribPointer(2, 2, GL_FLOAT,GL_FALSE,
                      7*sizeof(GLfloat), (GLvoid*)(5*sizeof(GLfloat)));
glEnableVertexAttribArray(2);                    
                      
```


## Textures纹理

我们已经了解到，我们可以为每个顶点添加颜色来增加图形的细节，从而创建出有趣的图像。但是，如果想让图形看起来更真实，我们就必须有足够多的顶点，从而指定足够多的颜色。这将会产生很多额外开销，因为每个模型都会需求更多的顶点，每个顶点又需求一个颜色属性。

推荐使用纹理，因为它可以让物体非常精细而不用指定额外的顶点。

例如对三角形上要贴个纹理：为了能够把纹理映射(Map)到三角形上，我们需要指定三角形的每个顶点各自对应纹理的哪个部分。这样每个顶点就会关联着一个纹理坐标(Texture Coordinate)，用来标明该从纹理图像的哪个部分采样（译注：采集片段颜色）。之后在图形的其它片段上进行片段插值(Fragment Interpolation)。

### 创建纹理，使用纹理



创建纹理之后，如果要使用多纹理，下面是使用方法的示例：
```c++
  glBindTextureUnit(0, this->textureID1);
  glBindTextureUnit(1, this->textureID2);
  // or use below method
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, this->textureID1);
    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, this->textureID2);
```

### 纹理坐标

纹理坐标起始于(0, 0)，也就是纹理图片的左下角，终始于(1, 1)，即纹理图片的右上角。下面的图片展示了我们是如何把纹理坐标映射到三角形上的。

![纹理坐标演示](https://learnopengl-cn.github.io/img/01/06/tex_coords.png)

OpenGL的纹理坐标起点在于左下角. 而图像纹理坐标的起点在于左上角。因此必要的时候在加载纹理图像时需要翻转y轴方式加载图像数据。

创建texture object与其他vao，vbo...之间没有关系。 在渲染的时候通过` glBindTexture(GL_TEXTURE_2D, this->texture);`绑定了哪个texture，就会使用哪个纹理，当然这里没有提到纹理坐标，通常我们将各个vertex顶点所对应的纹理坐标直接通过vbo传递进去，然后再顶点渲染器中读取，并将该纹理坐标传递给fragment shader去使用

在fragment shader有可能也需要texture object， 这个可以通过unform直接传递，在fragment shader中有内置的纹理类型支持，例如sampler1D、sampler3D。 下面是一个fragment shader例子：
```glsl
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}
```

一旦纹理被绑定到一个特定的纹理目标，在删除之前就一直保持着绑定状态。

纹理单元(Texture Unit) 这个概念需要澄清下。 

显卡中有N个纹理单元（GL_TEXTURE0，GL_TEXTURE1，GL_TEXTURE2…），每个纹理单元中保存着很多texture target(例如GL_TEXTURE_2D)

glActiveTexture(GLenum textureUnit)就是设置当前活动的纹理单元

glBindTexture(GLenum textureTarget, GLuint textureObject)就是将纹理对象ID赋值给当前活动的纹理单元的对应的纹理目标。

在opengl中，采样器(sampler)变量对应着纹理单元

```c++
//OpenGL版本低于4.5的glBindTextureUnit的替代品

glBindTextureUnit(unit, texture) 几乎与下面相同

glActiveTexture(GL_TEXTURE0 + unit); //激活纹理单元
glBindTexture(GL_TEXTURE_2D, texture);
唯一的区别是第二个版本将改变活动纹理全局状态。 当使用二维纹理以外的其他纹理时 glBindTexture 可能需要调整。
```


# GLSL

[GLSL Versions和GLSL ES Versions 对比](https://www.cnblogs.com/beeasy/p/6339313.html)

在高版本GLSL（从330开始）中可以通过layout location来使用显式属性位置
```c
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoords;
```

在[index](https://developer.gnome.org/gtk3/stable/api-index-full.html)中，Signals are indicated by ClassName::signal-name

https://developer.gnome.org/gtk3/stable/ch01s04.html

https://developer.gnome.org/gtk3/stable/GtkWidget.html#GtkWidget.signals
http://www.huzheng.org/ebook/gtk2-tut/x408.html

对低版本，只能在链接阶段，只是你
```c
  glBindAttribLocation( lightingShader_prog.progid, 0, "aPos" );
  glBindAttribLocation( lightingShader_prog.progid, 1, "aTexCoords" );
```


gulong g_signal_connect_swapped( gpointer     *object,
                                 const gchar  *name,
                                 GCallback    func,
                                 gpointer     *slot_object );
void callback_func( GtkWidget *widget,
                    gpointer   callback_data );

如果采用event，那event 的callback应是如下：
gint callback_func( GtkWidget *widget,
                    GdkEvent  *event,
                    gpointer   callback_data );



# python opengl

[万字长文详解如何用Python玩转OpenGL](https://cloud.tencent.com/developer/article/1546505)

pip install pyopengl

from OpenGL.GL import *
from OpenGL.GLUT import *

