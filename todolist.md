# 图像处理软件

**注意：图像处理必须是串行流程，即从Load Image->Convert to Grayscale后，所以操作的输出都是下一次操作的输入，所以之后的操作都应该在灰度图上。只有Reset Image会重置图像。**

## 技术栈
- UI使用https://github.com/Dovyski/cvui
- 图像处理使用C++ OpenCV4.x

功能清单如下：
注意：只支持串行流程，比如

Load Image (3通道彩色) 
    ↓
Convert to Grayscale (1通道灰度)
    ↓
Segmentation (阈值化) - 
    ↓
Clean-Up (孔洞填充/特征过滤) - 保持1通道
    ↓
Morphology (形态学操作) - 保持1通道
    ↓
Measurements (测量分析) - 基于1通道二值图像

## 1. Image (图片加载与显示)

### 1.1 Functions (功能)

- **Load (加载图片)**: 加载的图片要显示在框中

## 2. Color (彩色图像处理)

- **Convert to Grayscale (灰度转换)**: 将彩色图像转换为灰度图像
- **Color Select (颜色选择)**: 根据色调、饱和度和值的范围在彩色图像中选择对应的像素点
- **Color Cluster (颜色分类)**: 根据k-means 算法将当前彩色图像分割成不同的类别，适用于具有2类或者更多特征类型的彩色图像，其中不同的颜色定义不同的特征类型
- **Color Deconvolution (颜色反褶积)**: 将彩色图像分离为非基本红、蓝、绿通道的通道(颜色)；此功能是预处理彩色图像的强大工具，可以生成灰度通道操作，并进一步过滤选择感兴趣的特征区域
- **Channel Operation (通道操作)**: 对打开的图像通道执行算术运算，仅适用于具有多通道的彩色图像

## 3. Pre-Processing (预处理)

### 3.1 CONTRAST (对比度调节)

- **Adjust Contrast (对比度调整)**: 可调节图像的亮度和对比度
- **Histogram Equalization (直方图均衡化)**: 均衡整个图像的亮度和对比度
- **Flatten Background (淡化背景)**: 通过高斯模糊，来去除当前图像中的亮度梯度

### 3.2 NOISE-REDUCTION (降噪处理)

- **Media Filter (中值滤波器)**: 将当前图像中的每个像素等于其指定邻域的中位数像素值来进行图形降噪；在降噪的同时，可以有效保留特征区域边界
- **Wiener Filter (维纳滤波器)**: 减少稀疏噪声，同时保留特征区域边缘
- **Non-Local Means (非局部均值)**: 通过比较图像中的像素与其邻域内的像素来消除噪声；建议使用时尝试多种不同滤波器，以找到最优的方法

### 3.3 BLUR (模糊处理)

- **Gaussian Blur (高斯模糊)**: 通过设置每个像素等于其指定高斯邻域的加权平均像素值来模糊当前图像
- **Average Blur (平均模糊)**: 通过将每个像素设置为其指定邻域的平均像素值来模糊当前图像
- **Sum Filter (累加滤波器)**: 通过将每个像素设置为其指定邻域的像素值总和来模糊当前图像
- **Grayscale Dilate (灰度扩张)**: 将当前像素设置为指定邻域中的最大像素值
- **Grayscale Erode (灰度收缩)**: 将当前像素设置为指定邻域中的最小像素值

### 3.4 EDGES (突出边界)

- **StdDev Filter (标准偏差过滤器)**: 将当前图像像素设置为其指定邻域像素值的标准偏差，进而使像素变亮来突出特征区域的边界
- **Entropy Filter (熵过滤器)**: 将当前图像像素设置为其指定邻域像素值的熵，进而使像素变亮来突出特征区域的边界
- **Gradient Filter (梯度过滤器)**: 将当前图像中的每个像素设置为与其指定邻域的平均梯度大小成反比
- **Highlight Lines (高亮线)**: 突出显示图像中的线性类型特征；使用此功能时，建议先将图形进行降噪的预处理

### 3.5 TEXTURE (突出纹理特征)

- **Bright Texture (也称为顶帽滤波器)**: 突出图像中亮的纹理特征；可用于增加图像中灰度特征的对比度，也可用于去除荧光成像中较大的过度曝光区域
- **Dark Texture (也称为底帽滤波器)**: 突出图像中暗的纹理特征
- **Advanced Texture (高级纹理)**: 突出图像中的不同纹理特征
- **Similarity (相似性)**: 包含处理图像内或两个图像之间的相似性和对称性的多种滤波器

### 3.6 CORRECTION (图像修正)

- **Sharpen (锐化)**: 用于恢复被去噪过滤器减弱的各个特征对比度，锐化当前图像
- **FFT Filter (FFT滤波器)**: 使用自定义或预设滤镜修改当前图像的傅里叶变换；有很多应用场景，比如去除背景、边缘强化以及减少伪影
- ***Grayscale Interpolation (灰度插值)***: 需要Companion 图像存在，通过在指定邻域内的平均已知像素来重建缺失像素
- ***Grayscale Reconstruction (灰度重构)***: 根据Companion 图像中的特征来更改当前图像灰度值

## 4. Segmentation (图像分割)

### 4.1 THRESHOLD (阈值标记)

- **Basic Threshold (基本阈值标记)**: 根据像素值低于或高于某个特定像素值来进行特征的标记；一般使用此步骤前，建议先对图像进行预处理操作
- **Range Threshold (范围阈值标记)**: 根据像素位于指定的像素值范围之间来进行特征的标记；一般使用此步骤前，建议先对图像进行预处理操作
- **Adaptive Threshold (自适应阈值标记)**: 根据像素相对于周围邻域的某些统计值的强度来进行特征的标记；也可以在Companion 图像标记的基础上再进行识别标记
- **OTSU Threshold (E-M阈值标记)**: 如果一个区域内的像素，每个像素都高于距离最近的局部最小值的阈值，则将像素全部标记
- ***Local Threshold (局部阈值标记)***: 需要Companion 图像存在；如果像素能满足 Companion 图像中的每个特征像素标准，则对像素进行特征标记

### 4.2 EDGES (边界识别)

- **Watershed (分水岭分割)**: 预处理（降噪、灰度化、阈值）作为输入，根据分水岭算法来进行图像的分割；分割结果使用红色轮廓线在原始图像上标记，终端输出分割后的个数
- **Find Edges (识别边界)**: 使用指定的边缘算法将边缘像素设置为空，以突出边界
- **Find Circle (识别圆形特征)**: 基于特定的算法，在图像中寻找不同直径范围的圆形特征
- **Find Lines (识别线条)**: 识别图像中的线条结构

### 4.3 SNAP

- **Auto Segmentation (自动分割)**: 基于两个种子图像特征进行自动识别，一个种子图像大致识别特征，另一个种子图像识别背景，进而将特征与背景边界进行分割
- ***Region Grow (区域生长)***: 从Companion 图像中获取种子点，种子点根据特定阈值范围进行生长
- ***Fast Marching Method (快速行进法)***: 可以理解为是 Region Grow的加强版，种子点根据其周围的局部灰度差值增长
- ***Active Contour (活动轮廓)***: 需要 Companion Image 的存在，可以识别特征区域的边界

### 4.4 EXTREMA (极值)

- **Find Global Maximum (全局最大值)**: 选择当前图像中的整体最大值
- **Find Global Minimum (全局最小值)**: 选择当前图像中的整体最小值
- **Find Local Maxima (局部最大值)**: 选择当前图像中的局部最大值
- **Find Local Minima (局部最小值)**: 选择当前图像中的局部最小值

## 5. Morphology (形态学)

### 5.1 Dilate and Erode (扩大和缩小)

#### Dilation (扩大)
- **Dilate Uniform**: 特征区域以指定的像素值整体全部扩大
- **Dilate Smart**: 对大于或等于指定阈值的某些像素点，进行选择性扩大
- **Dilate Retain**: 特征区域以指定像素值整体扩大的同时，会避免相邻的特征区域合并到一起

#### Erode (缩小)
- **Erode Uniform**: 特征区域以指定的像素值整体全部缩小
- **Erode Smart**: 对大于或等于指定阈值的某些像素点，进行选择性缩小或移除
- **Erode Retain**: 特征区域以指定像素值整体缩小的同时，会避免一个特征区域被分割成多个

### 5.2 EDGES (边界调节)

- **Clean Boundaries (边界处理)**: 将特征区域整体扩大，最终使彼此间的边界之间保留特定的像素间隔
- **Smooth Features (平滑功能)**: 平滑结构特征
- **Extend Features (扩展功能)**: 沿局部方向按设定的长度和厚度扩展结构特征
- **Perimeter Pixels (周边像素处理)**: 将特征结构的周边像素设置为空，同时选中图像中其他所有像素；一般都和 Invert步骤一起使用

### 5.3 THIN (细化处理)

- **Skeletonization (骨架化)**: 将特征结构细化成只保留1个像素大小的中心线和分支，而不容许特征结构分离
- **Thin (细化)**: 细化结构特征，比骨架化处理能产生更少的分支
- **Pixel Lines (像素线)**: 向单像素线添加像素，使线中的每个像素在其面上相连。这确保了线条在默认的"4像素"标签下是完全连接的特征
- **Distance Map (距离图)**: 生成当前分割图像的灰度距离图，其中每个像素根据其到最近的空像素的欧几里得距离着色。白色=0距离，黑色=255距离

## 6. Clean-Up (清除功能)

### 6.1 REJECT (删除)

- **Reject Features (删除特征)**: 根据设定的阈值大小，删除某些结构特征
- **Fill All Holes (填充所有孔洞)**: 填充特征中包含的所有孔洞
- **Relative Size Filter (相对大小过滤器)**: 如果特征区域在所有区域中所占的百分比高于或者低于指定的百分比，则对其进行删除
- **Remove Edge Features (移除边缘特征)**: 移除与图像边缘相接触的结构特征

### 6.2 Replace (替代)

- **Replace with (替代)**: 用圆形、椭圆、方形、直径等代表性结构来代替选中的特征
- **Mark Center (中心标记)**: 用选定的像素标记空白图像的中心

## 7. Memory (记忆图层)

- **Set Companion Image (设置伴随图像)**: 将当前图像保存为伴随图像
- **Load Companion Image (加载伴随图像)**: 从文件中加载图像作为伴随图像和当前图像
- **Call Companion Image (调用伴随图像)**: 将伴随图像调用为当前图像
- **Set Memory Image 1~6 (设置内存图像#1-6)**: 将当前图像作为记忆图像进行保存(多个记忆图像可保存为不同的编号，方便后续调用)
- **Call Memory Image 1~6 (调用内存图像#1-6)**: 随时可调用记忆图像作为当前图像
- **Call Original Image (调用原始图像)**: 将原始图像调用为当前图像

## 8. Math (数学计算)

### 8.1 SELECTION (选择)

- ***Union (组合)***: 将当前图像特征和伴随图像特征结合起来
- ***Minus (相减)***: 从当前图像特征中减去伴随图像特征
- ***Intersection (相交作用)***: 将当前图像和伴随图像中重叠的特征区域保留下来
- ***Keep Mutual (相互作用)***: 将当前图像与伴随图像中相接触或重叠的特征区域保留下来
- ***Keep Exclusive (排除作用)***: 将当前图像不与伴随图像中接触或重叠的特征区域保留下来
- **Make Grayscale (灰度)**: 将二进制的当前图像转换成灰度图像。选中的像素值为0，未选中的灰度值为255

### 8.2 Grayscale (对灰度图进行处理)

- ***Add (相加)***: 将当前图像灰度值添加到伴随图像
- ***Average (平均化)***: 取当前图像与伴随图像的灰度平均值
- ***Divide (相除)***: 将同伴图像除以当前图像
- ***Multiply (相乘)***: 将同伴图像乘以当前图像
- ***Subtract (相减)***: 从当前图像中减去伴随图像的灰度值
- **Add Value (增加值)**: 向当前灰度图像中的每个像素添加一个常量整数值。可能是正数，也可能是负数。所得像素值低于0或者高于255则会被裁剪成0或者255

### 8.3 Both (对黑白图像和灰度图进行处理)

- ***Merge Darker Pixels (合并较暗像素)***: 将当前图像与伴随图像中最暗的像素值进行合并
- ***Merge Lighter Pixels (合并较亮像素)***: 将当前图像与伴随图像中最亮的像素值进行合并

## 9. Measurements (测量)

功能选项卡对分割后的整个图形区域进行统计

### 9.1 PER IMAGE (整体测量值，针对每张图片)

- **Area (面积大小)**: 测量当前图像中选定像素的总面积
- **Area Fraction (面积分数)**: 测量当前图像中选定像素占整个图形的面积百分比
- **Count (计数)**: 测量当前图像中选定特征的总数
- **Count Fraction (计数占比)**: 测量当前图像中的特征计数相对于所选内存映像中特征技术的百分比
- **Estimate Count (估计计数)**: 估计特征的数量为图层中选定像素的总面积除以所选伴随图像中特征的平均面积
- **Intercepts (截距)**: 通过在当前图像中的要素中绘制指定数量的随机线或旋转线来测量诸如平均截距之类的度量
- **Image Dimensions (图像大小)**: 测量当前图像的X和Y尺寸
- **Number Density (数密度)**: 测量当前图像中每个区域或所选伴随图像中每个选择区域内的独立特征总数
- **Perimeter (周长)**: 测量当前图像中所有特征周围的总周长
- **Perimeter Fraction (周长占比)**: 测量当前图像中特征的总周长分数。它的计算方法是将总周长除以图像中的总像素数，或伴随图像中所选像素的总数
- **Path Length (路径长度)**: 测量当前图像中特征的总路径长度
- ***Intensity Mean (平均灰度强度)***: 基于伴随图像，测定灰度强度在选定范围内的平均值
- ***Intensity StdDev (灰度强度标准偏差)***: 基于伴随图像，测定灰度强度在选定范围内的标准偏差
- ***Intensity Sum (灰度强度总和)***: 基于伴随图像，测定灰度强度在选定范围内的总和
- ***Correlation Coefficient (相关系数)***: 测量当前图像与伴随图像之间的归一化和绝对相关系数
- ***Mutual Information (互信息)***: 测量当前图像与伴随图像之间互信息的归一化和经典绝对值

### 9.2 Features Measures (特征测量值，针对每个结构特征)

对图像或图层中的每个结构特征进行各种定量测定

#### 9.2.1 Size (尺寸测量值)

- **Area (面积)**: 每个结构特征的面积大小
- **Area Fraction (面积分数)**: 每个特征相对于整个图像占用的面积分数
- **Caliper Diameter (卡尺直径)**: 每个结构特征的最长边
- **Minimum Diameter (最小直径)**: 与每个结构特征相切的两条平行线之间的最小线长
- **Equivalent Diameter (等效直径)**: 将每个特征转换成相同面积的圆，每个圆的直径则为每个特征的直径
- **Filled Area (填充区域)**: 每个特征上填满孔的区域
- **Length-X (长度-X)**: 每个特征在X方向上的长度
- **Length-Y (长度-Y)**: 每个特征在Y方向上的长度
- **Major Axis Length (长轴长度)**: 将每个特征看做椭圆的长轴长度
- **Minor Axis Length (短轴长度)**: 将每个特征看做椭圆的短轴长度
- **Intercepts (截距)**: 测量每个结构特征的截距

#### 9.2.2 Shape (形状相关测量值)

- **Roughness (粗糙度)**: 最紧密拟合凸包的面积与每个特征的面积之比
- **Eccentricity (偏心率)**: 描述每个特征偏长或偏圆形程度；0代表圆，1代表一条直线
- **Aspect Ratio (纵横比)**: 长短轴长度的比率
- **Roundness (圆度)**: 等效直径和卡尺直径的比率
- **Perimeter (周长)**: 每个结构特征的周长
- **Perimeter/Area (周长/面积)**: 每个特征其周长和面积的比率
- **First Moment of Area (截面面积矩)**: 描述从每个特征质心向外延伸的面积

#### 9.2.3 Location (位置相关测量值)

- **Centroid (质心)**: 每个结构特征的质心位置
- **Orientation (方向)**: 将特征看做椭圆，椭圆相对于正X轴的拟合角度。正角是顺时针旋转，负角是逆时针旋转
- **Tilt (倾斜度)**: 每个结构特征的卡尺直径相对于正X轴的角度。正角是顺时针旋转，负角是逆时针旋转
- **Nearest Neighbor (最邻近距离)**: 每个特征到相邻其他特征的最近距离，根据特征的质心来计算
- **Average Neighbor (平均邻近距离)**: 每个特征到相邻特征的平均距离
- **Edge Feature (边缘特征)**: 布尔状态，确定特征是否与图像边缘有接触

#### 9.2.4 Based on Companion (基于伴随图像的测量值)

Companion ID, Companion Features, Companion Area, Companion Perimeter, Companion Path Length, Text, Intensity Mean, Intensity StdDev, Intensity Sum 具体测量值的含义可参见电子版说明书

### 9.3 Local Measures (局部测量值，针对每个像素)

对图像中的单个像素进行各种定量测定

- **Anisotropy (各向异性)**: 测量图像中每个特征像素的局部方向性，0表示方向完全随机，1表示高度方向性
- **Area Fraction (面积分数)**: 测量图像中每个像素在所选特征中的局部面积分数
- **Count (计数)**: 测量每个像素周围的特征数量
- **Curvature (曲率)**: 通过在每个像素处拟合一个圆来生成特征周长的局部曲率
- **Thickness (厚度)**: 测量特征的局部厚度，方法是测量可以匹配特征的最大圆的直径，圆心位于特征骨架的每个点上
- **Orientation (方向性)**: 测量图像中每个像素的局部特征方向，从-90度到90度

Intensity Mean, Intensity StdDev, Nearest Distance 等测量值含义可参见电子版说明书

### 9.4 Custom Measurements (自定义创建测量值)

如果现有的测量选项无法满足定量需求，可以通过自定义测量值的设定来创建新的公式，进行新测量值的测定。

---

## 问题修复记录

### 已修复问题
1. **PreProcessing模块参数重置问题** - 修复了功能切换时参数不重置的问题
2. **UI组件变量作用域问题** - 修复了switch语句中变量声明的编译错误
3. **模块化架构完善** - 所有新模块都遵循统一的设计模式
4. **灰度转换通道问题** (2025-01-15) - 修复了convertToGrayscale后图像仍为3通道的问题
   - 问题: `cv::cvtColor(grayImage, currentImage, cv::COLOR_GRAY2BGR)` 将灰度图像转回3通道
   - 修复: 保持单通道灰度图像，在显示时自动转换为3通道
5. **Clean-Up功能不可用问题** (2025-01-15) - 修复了Fill All Holes和Reject Features功能
   - 问题: 二值化处理不够有效，孔洞检测算法不准确
   - 修复:
     - 使用Otsu自动阈值替代固定阈值127
     - 重新设计孔洞检测算法，使用flood fill方法
     - 改进特征检测的形状分析
     - 添加详细的调试输出用于问题诊断
     - 修复显示系统以正确处理单通道图像
6. **OpenCV copyTo错误和串行流程问题** (2025-01-15) - 修复通道不匹配和违背串行流程的问题
   - 问题1: `copyTo` 错误 - Segmentation模块强制转换通道导致不匹配
   - 问题2: 违背串行流程 - 各模块独立进行预处理操作
   - 修复:
     - 移除所有模块中的强制通道转换，保持输入输出一致性
     - 移除CleanUp和Measurements模块中的内部阈值化操作
     - 确保每个模块专注于自己的核心功能
     - 实现真正的串行处理流程: Load → Grayscale → Pre-Processing → Segmentation → Clean-Up → Morphology → Measurements
     - 添加详细的通道调试信息
7. **cvui::image copyTo错误的最终修复** (2025-01-15) - 彻底解决灰度工作流程中的显示问题
   - 问题根源: `cvui::image()`函数在处理单通道图像时出现通道不匹配的copyTo错误
   - 具体位置: UIComponents::renderPreviewArea 和 ImageProcessingApp::renderImageDisplay
   - 修复方案:
     - 在所有cvui::image调用前确保图像是3通道的
     - UIComponents::renderPreviewArea: 单通道图像自动转换为3通道显示
     - ImageProcessingApp::renderImageDisplay: 主显示区域的单通道图像自动转换
     - ImageProcessor::applyPreProcessedImage: 简化逻辑，直接接受通道变化
     - 颜色功能限制: Color Select/Cluster/Deconvolution在灰度图上显示警告并跳过
   - 实现需求: Load Image(3通道) → Convert to Grayscale(1通道) → 所有后续操作都在1通道上 → Reset恢复3通道
8. **分水岭分割算法实现错误** (2025-01-15) - 彻底重写分水岭算法，修复多个关键错误
   - 问题分析:
     - 二值化方向错误: 使用THRESH_BINARY而非THRESH_BINARY_INV
     - 标记生成方法错误: 使用findContours而非connectedComponents
     - 未知区域处理错误: 没有正确设置为0
     - 距离变换未归一化: 导致阈值参数无意义
     - 缺少标准的markers+1操作
   - 修复方案:
     - 使用标准二值化: THRESH_BINARY_INV | THRESH_OTSU (前景为白色)
     - 使用connectedComponents进行标记生成 (标准方法)
     - 正确处理未知区域: markers.setTo(0, unknown)
     - 距离变换归一化: normalize(0, 1.0, NORM_MINMAX)
     - 添加标准markers+1操作确保背景从1开始
     - 新增6步骤可视化画布 (1200x800) 显示完整处理过程
     - 完全符合OpenCV官方分水岭算法标准实现

9. **分水岭算法参数和可视化优化** (2025-01-15) - 解决参数使用和可视化时机问题
   - 问题分析:
     - 未使用的minArea参数仍在函数签名中
     - 可视化画布在Apply时显示，应该在Update Preview时显示
     - 画布中缺少最终分割结果显示
   - 修复方案:
     - 移除minArea参数，改为showVisualization控制参数
     - 修改可视化时机: Preview时显示(showVisualization=true)，Apply时不显示(showVisualization=false)
     - 扩展画布为2x4网格(1600x800)，新增第7步显示最终分割结果
     - 新增第8步显示统计信息(连通组件数、对象数、边界像素数、阈值)
     - 优化UI参数显示，移除minArea相关控件

10. **分水岭算法串行流程适配** (2025-01-15) - 修复违背串行流程设计的问题
   - 问题分析:
     - 违背串行流程: 分水岭算法重新进行灰度转换和二值化，应该直接使用预处理后的二值图像
     - 文字位置问题: cv::putText在图像下方，可能被遮挡
     - colorImage来源错误: 使用转换后的图像而非原始图像执行分水岭算法
   - 修复方案:
     - 串行流程适配: 输入验证单通道二值图像，跳过重复的预处理步骤，从形态学开运算开始
     - 文字位置调整: 所有cv::putText从图像下方(stepHeight-10)移到上方(Y=20)
     - 原始图像使用: 添加getOriginalImage()方法，分水岭算法使用原始彩色图像
     - 函数签名修改: watershed(image, originalImage, distanceThreshold, showVisualization)
     - 直接调用优化: 在ImageProcessingApp中直接调用watershed而非通过applyFunction
     - 可视化更新: 步骤1显示原始图像，步骤2显示输入的预处理二值图像

11. **跨平台构建支持** (2025-01-15) - 添加Windows和Linux跨平台构建支持
   - 问题分析: 当前代码只支持Windows系统构建，需要支持Linux和macOS
   - 实现方案:
     - 修改CMakeLists.txt支持跨平台编译器和库检测
     - 添加平台特定的库链接 (Windows: comdlg32, Linux: X11/GTK3, macOS: Cocoa)
     - 创建跨平台构建脚本 (build.sh for Unix, build.bat for Windows)
     - 添加自动OpenCV检测 (pkg-config for Linux, Homebrew for macOS, 默认路径 for Windows)
     - 创建GitHub Actions工作流进行自动化跨平台测试
     - 添加安装规则和桌面入口文件 (Linux)
     - 创建详细的跨平台构建文档 (BUILD.md)
     - 添加构建测试脚本 (test_build.sh)

### 测试验证
- 创建了 `test_cleanup_functions.md` 测试指南
- 创建了 `serial_workflow_test.md` 串行流程测试指南
- 创建了 `grayscale_workflow_test.md` 灰度工作流程测试指南
- 创建了 `watershed_segmentation_test.md` 分水岭分割测试指南
- 创建了 `corrected_watershed_test.md` 修正后分水岭算法测试指南
- 创建了 `final_watershed_test.md` 最终修复的分水岭分割测试指南
- 创建了 `serial_watershed_test.md` 串行流程分水岭分割测试指南
- 创建了 `BUILD.md` 详细的跨平台构建指南
- 创建了 `build.sh` 和 `build.bat` 跨平台构建脚本
- 创建了 `test_build.sh` 构建验证脚本
- 创建了 `.github/workflows/build.yml` GitHub Actions自动化测试
- 所有Clean-Up功能现在应该正常工作
- 灰度转换现在正确保持单通道格式
- 串行工作流程现在完全符合设计要求
- copyTo错误已彻底解决，支持完整的灰度工作流程
- 分水岭分割算法现在完全符合OpenCV标准实现和软件串行流程设计，包含完整的8步骤可视化，正确的使用时机和参数控制
- 项目现在支持Windows、Linux和macOS跨平台构建，包含自动化测试和详细的构建文档
