# 示例文件说明

这个目录包含了用于测试图像处理算法的示例文件和脚本。

## 目录结构

```
examples/
├── README.md           # 本说明文件
├── test_all.bat       # 测试所有算法的批处理脚本
├── sample_images/     # 示例图片目录（需要用户添加）
└── output/            # 输出结果目录
```

## 使用说明

### 1. 准备测试图片

请在 `sample_images/` 目录中放置一些测试图片，支持的格式包括：
- JPEG (.jpg, .jpeg)
- PNG (.png)
- BMP (.bmp)
- TIFF (.tiff, .tif)

建议的测试图片：
- `sample.jpg` - 一张彩色照片
- `landscape.jpg` - 风景照片
- `portrait.jpg` - 人像照片
- `texture.jpg` - 纹理图片

### 2. 运行测试脚本

编译完成后，可以使用以下方式测试算法：

**方法1：使用测试脚本**
```batch
cd examples
test_all.bat
```

**方法2：手动运行单个算法**
```batch
# 灰度转换
..\build\bin\grayscale_converter.exe -i sample_images\sample.jpg -o output\gray.jpg -v

# 边缘检测
..\build\bin\edge_detector.exe -i sample_images\sample.jpg -o output\edges.jpg -v

# 图像模糊
..\build\bin\image_blur.exe -i sample_images\sample.jpg -o output\blur.jpg -v
```

### 3. 查看结果

处理完成后，结果图片将保存在 `output/` 目录中。您可以使用任何图片查看器来比较原图和处理后的效果。

## 性能测试

对于性能测试，建议使用不同尺寸的图片：
- 小图片：640x480 或更小
- 中等图片：1920x1080
- 大图片：4K (3840x2160) 或更大

## 批量处理示例

如果需要批量处理多张图片，可以创建自定义脚本：

```batch
@echo off
for %%f in (sample_images\*.jpg) do (
    echo 处理: %%f
    ..\build\bin\grayscale_converter.exe -i "%%f" -o "output\gray_%%~nf.jpg"
    ..\build\bin\edge_detector.exe -i "%%f" -o "output\edges_%%~nf.jpg"
    ..\build\bin\image_blur.exe -i "%%f" -o "output\blur_%%~nf.jpg"
)
echo 批量处理完成！
```

## 注意事项

1. 确保 `sample_images/` 目录中有测试图片
2. `output/` 目录会自动创建
3. 如果遇到权限问题，请以管理员身份运行
4. 大图片处理可能需要较长时间，请耐心等待
