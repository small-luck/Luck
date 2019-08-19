#include <iostream>
#include "../src/config.h"
#include "../src/log.h"

Luck::ConfigVar<int>::ptr g_int_value_config = Luck::Config::LookUp("system.port", (int)8080, "system port");

void test_1() 
{
    LUCK_LOG_INFO(LUCK_LOG_ROOT()) << g_int_value_config->getValue();
    LUCK_LOG_INFO(LUCK_LOG_ROOT()) << g_int_value_config->toString();
}

int main(int argc, const char* argv[])
{
    test_1();    
    return 0;
}

