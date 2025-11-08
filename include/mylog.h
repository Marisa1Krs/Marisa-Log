#ifndef MYLOG_H
#define MYLOG_H
#pragma once
#include<allfun_of_linux.h>
#include<iostream>
#include<vector>
#include<thread>
#include<memory>
#include<condition_variable>
#include<mutex>
#include <time.h>
#include<functional>
using std::string;
using std::vector;
using std::condition_variable;
using std::thread;
using std::mutex;
using std::unique_lock;
using std::lock_guard;
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL,
    LOG_LEVEL_MAX
} LogLevel;

// 日志级别名称映射表
static const char* g_log_level_names[LOG_LEVEL_MAX] = {
    "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};
class mylog
{
public:
    static void init(string& filePath,size_t bufSize,LogLevel logLevel);
    static mylog* getPtr();
    void setLogLevel(LogLevel logLevel);
    void setBufferSize(size_t size);
    void writeLog(LogLevel longLevel,const char* file, int line,const char* fmt, ...);
    bool isRunning(){
        return this->running;
    }
    ~mylog();
    mylog operator =(const mylog& temp)=delete;
    mylog(const mylog& temp)=delete;
private:
    string logDir;
    string logName;
    size_t bufSize;
    LogLevel logLevel;
    bool enableFile;
    thread helper;
    condition_variable isFull;
    condition_variable isEmpty;
    mutex logMutex;
    vector<string> bufferA;
    vector<string> bufferB;
    bool running;
    static mylog* _ptr;
    mylog();
    mylog(string& filePath,size_t bufSize,LogLevel logLevel);
    void worker();
    void writeBufferA(string logtext);
    void operator delete(void* _ptr){
        ::delete (mylog*)_ptr;
    }
};
void worker();
#endif
#define LOG_DEBUG(fmt, ...) mylog::getPtr()->writeLog(LOG_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  mylog::getPtr()->writeLog(LOG_INFO,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  mylog::getPtr()->writeLog(LOG_WARN,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) mylog::getPtr()->writeLog(LOG_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) mylog::getPtr()->writeLog(LOG_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)