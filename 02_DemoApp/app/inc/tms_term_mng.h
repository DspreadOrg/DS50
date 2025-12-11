#ifndef _TMS_TERM_MNG
#define _TMS_TERM_MNG

#define TERM_ENVIRONMENT_PARAM_FILE		"/ext/envparam.txt"
#define TERM_PARAM_FILE		"/ext/termparam.txt"  //param from tms

enum{
	PRO_ENVIRONMENT = 1, //生产环境
	TEST_ENVIRONMENT,	//测试环境
	DEV_ENVIRONMENT		//开发环境
};

//获取配置 
int tms_term_setConfigFlag(char* flag);
int tms_set_default_env_ip();
int tms_switch_environment(char* environment);
#endif
