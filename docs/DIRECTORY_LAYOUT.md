# X-JVM 仓库目录说明

约定：**在仓库根目录**执行 `cmake` / `ctest` / `scripts/`*（脚本内已自动 `cd` 到根目录）。


| 路径                                                                              | 说明                                       |
| ------------------------------------------------------------------------------- | ---------------------------------------- |
| `CMakeLists.txt`、`main.cpp`                                                     | 顶层构建与程序入口                                |
| `classfile/`、`runtime/`、`instructions/`、`interpret/`、`lang/`、`common/`、`utils/` | JVM 核心源码（C++）                            |
| `tests/`                                                                        | 单元测试源码与 `CMakeLists.txt`                 |
| `third_party/`                                                                  | 第三方源码：嵌入式 zlib、minizip(unzip)、GoogleTest |
| `scripts/`                                                                      | Windows 批处理 / PowerShell、Unix `build.sh` |
| `docs/`                                                                         | 项目文档（含故障排除）                              |
| `assets/`                                                                       | 静态资源（如 README 用图）                        |
| `tools/`                                                                        | 可选小工具、本地实验代码（默认不参与主构建）                   |
| `.github/workflows/`                                                            | CI 配置                                    |


可选本地构建目录：`build/`、`out/` 等（勿提交，见 `.gitignore`）。