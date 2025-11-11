## 工作流程
1. 了解现状
2. 给出方案
3. 给出计划
4. 每次只执行一个任务
每个流程结束等待review，OK后继续

## 语言规范
1. 对话以简体中文回复
2. 文档以markdown格式输出
3. 以 UTF-8 格式编码输出代码、注释

---

# UNet-PyTorch 项目结构分析

## 一、项目概述

**项目名称**: unet-pytorch
**项目类型**: 语义分割模型 (U-Net in PyTorch)
**主要用途**: 医学图像分割、VOC数据集分割
**支持的主干网络**: VGG16、ResNet50

---

## 二、目录结构详解

```
unet-pytorch/
├── 核心训练脚本
│   ├── train.py                    # VOC数据集训练主程序
│   ├── train_medical.py            # 医学数据集训练程序
│   └── predict.py                  # 预测推理脚本(单图、视频、FPS测试)
│
├── 推理和预处理
│   ├── unet.py                     # UNet推理类(核心推理接口)
│   ├── get_miou.py                 # mIoU指标评估脚本
│   ├── json_to_dataset.py          # Labelme JSON转VOC格式工具
│   ├── voc_annotation.py           # VOC数据集注解生成(VOC格式)
│   ├── voc_annotation_medical.py   # 医学数据集注解生成
│   └── summary.py                  # 模型摘要工具
│
├── 网络架构 (nets/)
│   ├── __init__.py
│   ├── unet.py                     # UNet主网络架构
│   ├── unet_training.py            # 损失函数、优化器配置
│   ├── vgg.py                      # VGG16主干网络
│   └── resnet.py                   # ResNet50主干网络
│
├── 工具函数 (utils/)
│   ├── __init__.py
│   ├── dataloader.py               # VOC数据集加载器
│   ├── dataloader_medical.py       # 医学数据集加载器
│   ├── utils.py                    # 通用工具函数
│   ├── utils_fit.py                # 训练循环函数(fit_one_epoch)
│   ├── utils_metrics.py            # 评估指标(mIoU、F-Score等)
│   └── callbacks.py                # 回调函数(LossHistory、EvalCallback)
│
├── 数据集目录
│   ├── VOCdevkit/                  # VOC格式数据集标准结构
│   │   └── VOC2007/
│   │       ├── JPEGImages/         # 原始图片(jpg)
│   │       ├── SegmentationClass/  # 标签分割图(png)
│   │       └── ImageSets/
│   │           └── Segmentation/   # 数据集划分列表
│   │               ├── train.txt
│   │               ├── val.txt
│   │               ├── trainval.txt
│   │               └── test.txt
│   │
│   ├── datasets/                   # 自定义数据集(示例)
│   │   ├── JPEGImages/             # 原始图片
│   │   ├── SegmentationClass/      # 标签分割图
│   │   └── before/                 # Labelme标注的JSON文件
│   │
│   └── Medical_Datasets/           # 医学图像数据集
│       ├── Images/                 # 医学原始图(30张示例)
│       ├── Labels/                 # 医学标签图(30张示例)
│       └── ImageSets/
│           └── Segmentation/
│               ├── train.txt
│               └── trainval.txt
│
├── 模型文件 (model_data/)
│   ├── README.md
│   └── unet_vgg_medical.pth        # 预训练医学数据集模型(~99MB)
│
├── 日志输出 (logs/)
│   └── README.MD
│   └── loss_*/                     # 训练日志目录(TensorBoard)
│
├── 测试图片 (img/)
│   ├── street.jpg                  # VOC数据集测试图
│   └── cell.png                    # 医学数据集测试图
│
├── 配置文件
│   ├── requirements.txt            # Python依赖包
│   ├── .gitignore                  # Git忽略文件
│   └── CLAUDE.md                   # 项目说明(中文)
│
├── 文档
│   ├── README.md                   # 项目文档(中英文)
│   ├── 常见问题汇总.md              # 常见问题解答(中文)
│   └── LICENSE                     # 开源许可证
│
└── 其他
    ├── tmp/                        # 临时文件目录
    │   └── unet_vgg_voc.pth       # VOC数据集模型(临时)
    └── .venv/                      # Python虚拟环境(开发用)
```

---

## 三、主要Python模块及功能详解

### 3.1 训练相关模块

| 文件 | 功能 | 核心类/函数 |
|-----|------|----------|
| `train.py` | VOC数据集训练主程序 | 支持: VGG/ResNet主干、SGD/Adam优化器、学习率衰减、分布式训练、混合精度 |
| `train_medical.py` | 医学数据集训练程序 | 针对非VOC格式医学数据的特殊训练脚本 |
| `nets/unet.py` | U-Net网络架构 | `Unet`类、`unetUp`上采样模块 |
| `nets/unet_training.py` | 损失函数与优化器 | `CE_Loss`、`Focal_Loss`、`Dice_loss`、`get_lr_scheduler` |

### 3.2 推理与预测模块

| 文件 | 功能 | 核心方法 |
|-----|------|--------|
| `unet.py` | 推理封装类 | `__init__`、`generate`、`detect_image`、`get_miou_png` |
| `predict.py` | 多模式预测脚本 | 单图预测、视频检测、FPS测试、批量目录预测、ONNX导出 |
| `get_miou.py` | 评估脚本 | mIoU计算、性能指标评估 |

### 3.3 数据处理模块

| 文件 | 功能 | 核心类 |
|-----|------|-------|
| `utils/dataloader.py` | VOC数据集加载器 | `UnetDataset`、`unet_dataset_collate` |
| `utils/dataloader_medical.py` | 医学数据集加载器 | `UnetDataset`、`unet_dataset_collate` |
| `json_to_dataset.py` | Labelme转VOC工具 | JSON→PNG标签转换 |
| `voc_annotation.py` | VOC注解生成 | 生成train/val/test/trainval.txt列表 |
| `voc_annotation_medical.py` | 医学注解生成 | 医学数据集划分 |

### 3.4 工具与指标模块

| 文件 | 功能 | 核心函数 |
|-----|------|---------|
| `utils/utils.py` | 通用工具 | `cvtColor`、`preprocess_input`、`resize_image`、`show_config` |
| `utils/utils_fit.py` | 训练循环 | `fit_one_epoch`、`fit_one_epoch_no_val` |
| `utils/utils_metrics.py` | 评估指标 | `compute_mIoU`、`f_score`、`show_results` |
| `utils/callbacks.py` | 回调函数 | `LossHistory`、`EvalCallback` |

### 3.5 网络骨干模块

| 文件 | 功能 | 描述 |
|-----|------|------|
| `nets/vgg.py` | VGG16主干 | 预训练VGG16特征提取，分层输出5个特征图 |
| `nets/resnet.py` | ResNet50主干 | 预训练ResNet50特征提取，分层输出5个特征图 |

---

## 四、关键配置文件

### 4.1 requirements.txt
```
torch                 # PyTorch核心库
torchvision          # 计算机视觉工具
tensorboard          # 训练可视化
scipy                # 科学计算
numpy                # 数据处理
matplotlib           # 绘图
opencv-python        # 图像处理
tqdm                 # 进度条
Pillow               # 图像库
h5py                 # 数据格式
labelme              # 图像标注工具
```

### 4.2 模型参数配置 (unet.py中的_defaults字典)

```python
{
    "model_path": 'model_data/unet_vgg_medical.pth',  # 模型权重路径
    "num_classes": 2,                                  # 分类数+1
    "backbone": "vgg",                                 # vgg或resnet50
    "input_shape": [512, 512],                         # 输入大小
    "mix_type": 0,                                     # 混合模式(0=混合,1=仅结果,2=仅前景)
    "cuda": False,                                     # 是否使用GPU
}
```

---

## 五、数据集结构

### 5.1 VOC格式标准结构
```
VOCdevkit/VOC2007/
├── JPEGImages/           # 原始RGB图像
│   └── 0001.jpg, 0002.jpg, ...
├── SegmentationClass/    # 像素级标签(PNG)
│   └── 0001.png, 0002.png, ...
└── ImageSets/Segmentation/
    ├── train.txt         # 训练集列表(无扩展名)
    ├── val.txt           # 验证集列表
    ├── test.txt          # 测试集列表
    └── trainval.txt      # 训练+验证集列表
```

### 5.2 医学数据集结构
```
Medical_Datasets/
├── Images/               # 医学原始图像(PNG/JPG)
│   └── 0.png, 1.png, ...
├── Labels/               # 对应的分割标签(PNG)
│   └── 0.png, 1.png, ...
└── ImageSets/Segmentation/
    ├── train.txt
    └── trainval.txt
```

---

## 六、工作流程

### 6.1 完整训练流程

```
1. 数据准备
   └─> 使用Labelme标注图像
   └─> 运行json_to_dataset.py转换为VOC格式
   └─> 运行voc_annotation.py生成数据列表

2. 训练配置 (修改train.py参数)
   ├─> num_classes: 分类数+1
   ├─> backbone: vgg或resnet50
   ├─> model_path: 预训练模型路径
   ├─> batch_size、learning_rate等

3. 执行训练
   └─> python train.py

4. 模型保存
   └─> 权重文件保存到logs/目录
   └─> 损失曲线保存到logs/loss_*/目录

5. 评估性能
   ├─> 修改get_miou.py的num_classes
   └─> python get_miou.py (计算mIoU)

6. 预测使用
   ├─> 修改unet.py的model_path、num_classes、backbone
   └─> python predict.py (单图/视频/FPS测试)
```

### 6.2 医学数据集训练流程

```
1. 准备医学数据
   ├─> Images/: 原始医学图像
   └─> Labels/: 对应分割标签

2. 运行医学训练
   └─> python train_medical.py

3. 模型推理
   ├─> 修改unet.py模型参数
   └─> python predict.py
```

---

## 七、核心类和函数

### 7.1 Unet推理类 (unet.py)

**核心方法**:
- `__init__(**kwargs)`: 初始化模型、加载权重
- `generate()`: 创建并加载预训练模型
- `detect_image(image)`: 单图预测
- `get_miou_png(image)`: 获取mIoU用的标签图
- `get_fps(image, test_interval)`: 计算FPS

### 7.2 数据加载器 (dataloader.py)

**UnetDataset类**:
- `__init__`: 初始化数据集参数
- `__getitem__`: 返回(图像、标签、one-hot编码)
- `get_random_data`: 数据增强(翻转、缩放、色彩抖动)

### 7.3 训练函数 (utils_fit.py)

**fit_one_epoch函数**:
- 输入: 模型、优化器、数据加载器、epoch信息等
- 输出: 训练损失、验证损失、F-score
- 支持: fp16混合精度、分布式训练、多种损失函数

### 7.4 评估指标 (utils_metrics.py)

**主要函数**:
- `compute_mIoU()`: 计算平均IoU
- `f_score()`: 计算F1分数
- `show_results()`: 结果可视化

---

## 八、关键特性

### 8.1 网络特性
- 支持VGG16、ResNet50两种主干网络
- 编码器-解码器架构，跳跃连接
- 支持预训练权重加载

### 8.2 训练特性
- 支持CE Loss、Focal Loss、Dice Loss多种损失函数
- 学习率自适应调整(step、cos衰减)
- 支持SGD、Adam优化器
- 支持混合精度训练(fp16)
- 支持单机多卡分布式训练(DP/DDP模式)

### 8.3 数据增强
- 随机缩放、翻转、旋转
- HSV色彩空间抖动
- 中心裁剪、边界填充

### 8.4 推理特性
- 单图预测
- 视频实时检测
- FPS性能测试
- 批量目录预测
- ONNX模型导出

---

## 九、模型文件说明

| 模型文件 | 大小 | 训练数据集 | 用途 |
|---------|------|---------|------|
| `unet_vgg_medical.pth` | ~99MB | Medical医学数据集 | 医学图像分割预训练权重 |
| `unet_vgg_voc.pth` | (tmp目录) | VOC12+SBD | VOC通用分割预训练权重 |

---

## 十、项目使用场景

1. **医学图像分割**: 利用Medical_Datasets进行医学图像的语义分割
2. **VOC数据集**: 进行通用物体分割任务
3. **自定义数据集**: 通过Labelme标注→转换→训练的完整流程
4. **模型部署**: 通过ONNX导出进行跨平台推理

---

## 十一、总结

这是一个完整的、生产级别的U-Net语义分割框架，具有：

- **完整的训练管道**: 从数据准备到模型评估
- **灵活的网络设计**: 支持多种主干网络
- **丰富的功能**: 训练、预测、评估、导出
- **中文文档支持**: 适合中文用户学习使用
- **医学和通用应用**: 支持医学图像和VOC通用数据集

所有代码均为UTF-8编码，注释详细清晰，适合学习和实际应用。