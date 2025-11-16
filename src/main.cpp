// #include<iostream>
// #include"mylog.h"
// using namespace std;
// mylog* mylog::_ptr=nullptr;
// int main(){
//     string temp="/home/marisa/code1/log-library/log/log.txt";
//     mylog::init(temp,4096,LOG_INFO);
//     int user_id = 1001;
//     const char* username = "test_user";
//     LOG_DEBUG("用户登录调试信息：user_id=%d, username=%s\n", user_id, username);
//     LOG_INFO("用户登录成功：user_id=%d", user_id);
//     LOG_WARN("用户密码即将过期：username=%s", username);
//     LOG_ERROR("数据库连接失败：host=127.0.0.1, port=3306");
//     LOG_FATAL("配置文件缺失，程序退出");
//     fflush(stdout);
//     sleep(20);
//     return 0;
// }