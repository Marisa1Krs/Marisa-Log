
#include "mylog.h"
#include <stdarg.h>
mylog::mylog()
{

}
mylog::mylog(string& filePath,size_t bufSize,LogLevel logLevel)
:logDir(filePath)
,bufSize(bufSize)
,logLevel(logLevel)
,helper(std::bind(&mylog::worker, this))
,running(1)
,isEmpty()
,isFull()
,logMutex()
{
    
}
void mylog::init(string& filePath,size_t bufSize,LogLevel logLevel){
    if(_ptr==nullptr)
    _ptr=new mylog(filePath,bufSize,logLevel);
}
mylog* mylog::getPtr(){
    return _ptr;
}
void mylog::setLogLevel(LogLevel logLevel){
    this->logLevel=logLevel;
}
void mylog::setBufferSize(size_t size){
    bufSize=size;
}
void mylog::writeLog(LogLevel level,const char* file, int line,const char* fmt, ...){
     // 1. 校验参数合法性
     if (level < this->logLevel || level >= LOG_LEVEL_MAX || fmt == NULL) {
        return;
    }

    // 2. 临时缓冲区：存储单条完整日志（时间+级别+文件行号+内容）
    char log_buf[4096] = {0};
    size_t buf_pos = 0;

    // 3. 拼接当前时间（格式：YYYY-MM-DD HH:MM:SS.sss）
    char time_buf[32] = {0};
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm tm;
    localtime_r(&ts.tv_sec, &tm);  // 线程安全
    snprintf(time_buf, sizeof(time_buf),
             "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
             tm.tm_hour, tm.tm_min, tm.tm_sec, ts.tv_nsec / 1000000);
    
    buf_pos += snprintf(log_buf + buf_pos, sizeof(log_buf) - buf_pos,
                       "[%s] [%s] [%s:%d] ",
                       time_buf, g_log_level_names[level],
                       (strrchr(file, '/') ? strrchr(file, '/') + 1 : file), line);

    // 4. 拼接日志内容（处理可变参数）
    va_list args;
    va_start(args, fmt);
    buf_pos += vsnprintf(log_buf + buf_pos, sizeof(log_buf) - buf_pos, fmt, args);
    va_end(args);

    // 5. 拼接换行符（每条日志独立一行）
    if (buf_pos < sizeof(log_buf) - 1) {
        log_buf[buf_pos++] = '\n';
        log_buf[buf_pos] = '\0';
    }

    // 6. 将完整日志写入bufferA（循环存储）
    {
        lock_guard<mutex> temp(logMutex);
        writeBufferA(log_buf);
    }
}
void mylog::writeBufferA(string logtext){
    bufferA.push_back(logtext);
    isEmpty.notify_one();
}
void mylog::worker(){
    while(running){
        unique_lock<mutex> temp(logMutex);
        while(!bufferA.size()||!running){
            isEmpty.wait(temp);
        }
        if (!running) break;

            bufferB.swap(bufferA);
            temp.unlock(); 
        for(auto &t:bufferB){
            printf("%s\n",t.c_str());
            fflush(stdout);
        }
        bufferB.clear();
    }
}
mylog::~mylog()
{
    running=0;
    helper.join();
    isEmpty.notify_one();
}