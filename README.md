# Marisa⭐Daze 日志库

轻量级 C++ 异步日志库，基于四缓冲架构实现高性能日志记录。

## 架构设计

### 四缓冲模型

```
┌─ 前端线程（调用者线程） ──────────────────────┐
│  front_write_buf  ──swap──►  front_swap_buf    │
└─────────────────────────────────────────────────┘
                          │
                      swap │ （加锁互斥）
                          ▼
┌─ 后端线程（worker 线程） ──────────────────────┐
│  back_recv_buf  ──swap──►  back_flush_buf      │
│                                   │            │
│                         printf / write(fd)     │
└─────────────────────────────────────────────────┘
```

- **前端双缓冲**：`front_write_buf`（当前写入）+ `front_swap_buf`（待交换）
- **后端双缓冲**：`back_recv_buf`（接收）+ `back_flush_buf`（刷盘）
- 前后端通过两次 `std::swap` 完成无锁化数据传递（仅在 swap 时加锁）

### 核心特性

| 特性 | 说明 |
|------|------|
| **自动扩容** | `front_write_buf` 达到 `bufSize` 阈值时自动翻倍扩容 |
| **水位线保护** | `watermark = bufSize × 8`，超过水位线时丢弃日志，防止内存溢出 |
| **异步写文件** | 后端线程独立写文件/终端，不阻塞调用者 |
| **无锁消费** | 数据通过 swap 转移到后端后，后端在无锁状态下批量写入 |
| **内存回收** | 刷盘后若容量膨胀过大，自动回收多余内存 |

## 快速开始

```cpp
#include "mylog.h"

int main() {
    // 初始化（文件路径，缓冲区阈值，日志级别）
    mylog::init("./logs/app.log", 1024, LOG_INFO);
    
    int user_id = 1001;
    LOG_INFO("用户登录成功：user_id=%d", user_id);
    LOG_ERROR("数据库连接失败：host=%s, port=%d", "127.0.0.1", 3306);
    
    return 0;
}
```

### 日志级别

| 宏 | 级别 | 说明 |
|----|------|------|
| `LOG_DEBUG` | DEBUG | 调试信息 |
| `LOG_INFO` | INFO | 一般信息 |
| `LOG_WARN` | WARN | 警告 |
| `LOG_ERROR` | ERROR | 错误 |
| `LOG_FATAL` | FATAL | 致命错误 |

## 编译

```bash
mkdir build && cd build
cmake ..
make
```

生成静态库 `libmyLog.a`，链接时加入 `-lmyLog -lpthread`。

## 缓冲区配置

```cpp
// bufSize: 触发扩容和前后端交换的阈值
// watermark = bufSize × 8，超过则丢弃日志
mylog::init("./log.txt", 4096, LOG_INFO);
```
