#include <iostream>
#include "../src/config.h"
#include "../src/log.h"
#include <yaml-cpp/yaml.h>

Luck::ConfigVar<int>::ptr g_int_value_config = Luck::Config::LookUp("system.port", (int)8080, "system port");
Luck::ConfigVar<float>::ptr g_float_value_config = Luck::Config::LookUp("system.value", (float)10.22, "system value");
Luck::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config = Luck::Config::LookUp("system.int_vec", std::vector<int>{1,2}, "system int_vec");
Luck::ConfigVar<std::list<int>>::ptr g_int_list_value_config = Luck::Config::LookUp("system.int_list", std::list<int>{9,10}, "system list_vec");
Luck::ConfigVar<std::map<std::string, int>>::ptr g_str_int_map_value_config = Luck::Config::LookUp("system.str_int_map", std::map<std::string, int>{{"k",2}}, "system str_int_map");
Luck::ConfigVar<std::unordered_map<std::string, int>>::ptr g_str_int_umap_value_config = Luck::Config::LookUp("system.str_int_umap", std::unordered_map<std::string, int>{{"k",2}}, "system str_int_umap");
Luck::ConfigVar<std::set<int>>::ptr g_int_set_value_config = Luck::Config::LookUp("system.int_set", std::set<int>{1,2}, "system int_set");
Luck::ConfigVar<std::unordered_set<int>>::ptr g_int_uset_value_config = Luck::Config::LookUp("system.int_uset", std::unordered_set<int>{1,2}, "system int_uset");

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

#define XX(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for (auto& i : v) { \
            LUCK_LOG_INFO(LUCK_LOG_ROOT()) << #prefix " " #name << i; \
        } \
        LUCK_LOG_INFO(LUCK_LOG_ROOT()) << #prefix " " #name "yaml: " << g_var->toString(); \
    }

    XX(g_int_vec_value_config, int_vec, before);
    XX(g_int_list_value_config, int_list, before);

    YAML::Node root = YAML::LoadFile("/mnt/study/Luck/doc/conf/log.yml");
    Luck::Config::LoadFromYaml(root);

    LUCK_LOG_INFO(LUCK_LOG_ROOT()) << "after: " << g_int_value_config->toString();
    LUCK_LOG_INFO(LUCK_LOG_ROOT()) << "after: " << g_float_value_config->toString();

    XX(g_int_vec_value_config, int_vec, after);
    XX(g_int_list_value_config, int_list, after);
}

int main(int argc, const char* argv[])
{
    // test_1();    
    //test_yaml();
    test_config();
    return 0;
}

