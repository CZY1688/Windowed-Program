# Windowed-Program

## 生成番茄钟素材（ICO/BMP）

仓库提供了一个素材生成程序，可自动生成：

- `main.ico`（应用图标，直接被 `template.rc` 引用）
- `assets/pomodoro_frames/tomato_00.bmp ~ tomato_11.bmp`（番茄小动画帧）

运行方式（需要 Python 3）：

```bash
python scripts/generate_assets.py
```

说明：

- 该脚本不依赖第三方库，使用标准库直接输出 `bmp/ico` 文件。
- 你可以先用自动生成的素材联调功能，再替换成自己下载的素材。

## 番茄钟完整程序（当前工程）

当前 `main.cpp` 已实现完整番茄钟流程：

- 工作/休息双阶段自动切换
- 开始/暂停/重置/跳过
- 轮次统计
- 键盘快捷键：`Space` 开始/暂停，`R` 重置，`N` 跳过
- 使用 `assets/pomodoro_frames/*.bmp` 做番茄动画帧显示

使用步骤：

1. 先执行素材生成：
   ```bash
   python scripts/generate_assets.py
   ```
2. 用 Visual Studio 打开并构建 `VC2010简单Windows程序模板.sln`
3. 运行程序即可看到完整番茄钟界面与动画
