#include "config.h"
#include <list>

namespace Luck {

Config::ConfigVarMap Config::s_datas;

/* 根据名称在参数容器中查找是否存在对应的ptr */
ConfigVarBase::ptr Config::LookUpBase(const std::string& name)
{
    auto it = s_datas.find(name);
    return it == s_datas.end() ? nullptr : it->second;
}

//system.port
//system:
//   port: 8080
void ListAllMembers(const std::string& prefix, const YAML::Node& node, std::list<std::pair<std::string, const YAML::Node>>& output)
{
    //先校验prefix是否合法
    if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos) {
        LUCK_LOG_ERROR(LUCK_LOG_ROOT()) << "config prefix error" << " - prefix: " << prefix;
    }
    
    output.push_back(std::make_pair(prefix, node));

    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); it++) {
            ListAllMembers(prefix.empty() ? it->first.Scalar() : prefix + '.' + it->first.Scalar(), it->second, output);
        }
    }
}

/* 从yaml配置文件中获取参数，放入参数容器中 */
void Config::LoadFromYaml(const YAML::Node& root)
{   
    //先将yaml中的参数扁平化
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    ListAllMembers("", root, all_nodes);
    
    //扁平化ok，根据参数节点的key，判断有没有更新的，如果有，则更新
    for (auto& i : all_nodes) {
        std::string key = i.first;
        if (key.empty())
            continue;

        //将key小写化
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);

        //在参数容器中根据key查找
        ConfigVarBase::ptr var = LookUpBase(key);
        if (var) {
            if (i.second.IsScalar()) {
                var->fromString(i.second.Scalar());
            } else {
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
}



}
