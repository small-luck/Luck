#include <iostream>
#include "../src/config.h"
#include "../src/log.h"
#include <yaml-cpp/yaml.h>

Luck::ConfigVar<int>::ptr g_int_value_config = Luck::Config::LookUp("system.port", (int)8080, "system port");
Luck::ConfigVar<float>::ptr g_float_value_config = Luck::Config::LookUp("system.value", (float)10.22, "system value");

void test_1() 
{
    LUCK_LOG_INFO(LUCK_LOG_ROOT()) << g_int_value_config->getValue();
    LUCK_LOG_INFO(LUCK_LOG_ROOT()) << g_int_value_config->toString();
}

void print_node(const YAML::Node& node, int level)
{
    if (node.IsScalar()) {
        LUCK_LOG_INFO(LUCK_LOG_ROOT()) << std::string(level * 4, ' ') << node.Scalar() << " - " << node.Type();
    } else if (node.IsNull()) {
        LUCK_LOG_INFO(LUCK_LOG_ROOT()) << std::string(level * 4, ' ') << " Null " << " - " << node.Type();
    } else if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); it++) {
            LUCK_LOG_INFO(LUCK_LOG_ROOT()) << std::string(level * 4, ' ') << it->first << " - " << it->second.Type();
            print_node(it->second, level++);
        }
    } else if (node.IsSequence()) {
        for (size_t i = 0; i < node.size(); i++) {
            LUCK_LOG_INFO(LUCK_LOG_ROOT()) << std::string(level * 4, ' ') << i << node[i].Type();
            print_node(node[i], level++);
        }
    }
}

void test_yaml()
{
    YAML::Node root = YAML::LoadFile("/mnt/study/Luck/doc/conf/log.yml");
   // LUCK_LOG_INFO(LUCK_LOG_ROOT()) << root;
   print_node(root, 0);
}

void test_config()
{
    LUCK_LOG_INFO(LUCK_LOG_ROOT()) << "before: " << g_int_value_config->toString();
    LUCK_LOG_INFO(LUCK_LOG_ROOT()) << "before: " << g_float_value_config->toString();

    YAML::Node root = YAML::LoadFile("/mnt/study/Luck/doc/conf/log.yml");
    Luck::Config::LoadFromYaml(root);

    LUCK_LOG_INFO(LUCK_LOG_ROOT()) << "after: " << g_int_value_config->toString();
    LUCK_LOG_INFO(LUCK_LOG_ROOT()) << "after: " << g_float_value_config->toString();


}

int main(int argc, const char* argv[])
{
    // test_1();    
    //test_yaml();
    test_config();
    return 0;
}

