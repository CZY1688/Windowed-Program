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
