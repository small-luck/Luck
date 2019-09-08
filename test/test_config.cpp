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
            LUCK_LOG_INFO(LUCK_LOG_ROOT()) << #prefix " " #name << " " << i; \
        } \
        LUCK_LOG_INFO(LUCK_LOG_ROOT()) << #prefix " " #name "yaml: " << g_var->toString(); \
    }

#define XX_M(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for (auto& i : v) { \
            LUCK_LOG_INFO(LUCK_LOG_ROOT()) << #prefix " " #name << " :{" << \
                i.first << ":" << i.second << "}"; \
        } \
        LUCK_LOG_INFO(LUCK_LOG_ROOT()) << #prefix " " #name "yaml: " << g_var->toString(); \
    }

    XX(g_int_vec_value_config, int_vec, before);
    XX(g_int_list_value_config, int_list, before);
    XX(g_int_set_value_config, int_set, before);
    XX(g_int_uset_value_config, int_uset, before);
    XX_M(g_str_int_map_value_config, g_str_int_map, before);
    XX_M(g_str_int_umap_value_config, g_str_int_umap, before);


    /* YAML::Node root = YAML::LoadFile("/mnt/study/Luck/doc/conf/log.yml"); */
    YAML::Node root = YAML::LoadFile("/mnt/hgfs/share/Luck/doc/conf/log.yml");
    Luck::Config::LoadFromYaml(root);

    LUCK_LOG_INFO(LUCK_LOG_ROOT()) << "after: " << g_int_value_config->toString();
    LUCK_LOG_INFO(LUCK_LOG_ROOT()) << "after: " << g_float_value_config->toString();

    XX(g_int_vec_value_config, int_vec, after);
    XX(g_int_list_value_config, int_list, after);
    XX(g_int_set_value_config, int_set, after);
    XX(g_int_uset_value_config, int_uset, after);
    XX_M(g_str_int_map_value_config, g_str_int_map, after);
    XX_M(g_str_int_umap_value_config, g_str_int_umap, after);
}

class Person {
public:
    std::string m_name;
    int m_age = 0;
    bool m_sex = false;
    
    std::string toString() const {
        std::stringstream ss;
        ss << "[person name = " << m_name
            << "age = " << m_age
            << "sex = " << m_sex
            << "]";

        return ss.str();
    }

    bool operator== (const Person& other) const  {
        return m_name == other.m_name 
            && m_sex == other.m_sex
            && m_age == other.m_age;
    }
};


namespace Luck {

/* 支持vector转换,做偏特化, 将string转为vector */
template<>
class LexicalCast<std::string, Person> {
public:
    Person operator()(const std::string& val) {
        YAML::Node node = YAML::Load(val);
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};

/* 支持将vector<T>转换为string */
template<>
class LexicalCast<Person, std::string> {
public:
    std::string operator()(const Person& p) {
        YAML::Node node;
        node["name"] = p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

}

Luck::ConfigVar<Person>::ptr g_person = Luck::Config::LookUp("class.person", Person(), "class person");

void test_class() {
    g_person->addListener(10, [](const Person& old_value, const Person& new_value){
        LUCK_LOG_INFO(LUCK_LOG_ROOT()) << "old_value = " << old_value.toString()
                << " - " << "new_value = " << new_value.toString();
    });

    LUCK_LOG_INFO(LUCK_LOG_ROOT()) << "before" << g_person->getValue().toString() << "-" << g_person->toString();
    
    YAML::Node root = YAML::LoadFile("/mnt/hgfs/share/Luck/doc/conf/log.yml");
    Luck::Config::LoadFromYaml(root);

    LUCK_LOG_INFO(LUCK_LOG_ROOT()) << "after" << g_person->getValue().toString() << "-" << g_person->toString();
}

int main(int argc, const char* argv[])
{
    // test_1();    
    //test_yaml();
    //test_config();
    test_class();
    return 0;
}

