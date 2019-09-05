/*
 * config.h: 配置模块封装
 * author: liupei
 * date: 2019:08:18
 * */
#ifndef __LUCK_CONFIG_H__
#define __LUCK_CONFIG_H__

#include <iostream>
#include <string>
#include <memory>
#include <boost/lexical_cast.hpp>
#include <map>
#include <vector>
#include <list>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <yaml-cpp/yaml.h>
#include <functional>
#include "log.h"

namespace Luck {

/* 配置参数的基类 */
class ConfigVarBase {
public:
    /* 定义智能指针 */
    typedef std::shared_ptr<ConfigVarBase> ptr;

    /* 构造函数，初始化成员变量 */
    ConfigVarBase(const std::string& name, const std::string& description = "")
        : m_name(name) //强制转换为小写
        , m_description(description) {
            std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
        }

    /* 析构函数 */
    virtual ~ConfigVarBase() {}

    /* 获取参数名称 */
    const std::string& getName() const { return m_name; }

    /* 获取参数描述 */
    const std::string& getDescription() const { return m_description;  }

    /* 将配置参数转为string */
    virtual std::string toString() = 0;

    /* 将string转为配置参数的值 */
    virtual bool fromString(const std::string& val) = 0;
protected:
    /* 配置参数名称 */
    std::string m_name;

    /* 配置参数描述 */
    std::string m_description;
};

/* 基本类型的转换 */
template<class F, class T>
class LexicalCast {
public:
    T operator()(const F& v) {
        return boost::lexical_cast<T>(v);
    }
};

/* 支持vector转换,做偏特化, 将string转为vector */
template<class T>
class LexicalCast<std::string, std::vector<T>> {
public:
    std::vector<T> operator()(const std::string& val) {
        YAML::Node node = YAML::Load(val);
        std::vector<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }

        return vec;
    }
};

/* 支持将vector<T>转换为string */
template<class T>
class LexicalCast<std::vector<T>, std::string> {
public:
    std::string operator()(const std::vector<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/* 支持从string-->std::list<T> 转换*/
template<class T>
class LexicalCast<std::string, std::list<T>> {
public:
    std::list<T> operator()(const std::string& str) {
        YAML::Node node = YAML::Load(str);
        std::list<T> lst;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            lst.push_back(LexicalCast<std::string, T>()(ss.str()));
        }

        return lst;
    }
};

/* 支持从std::list<t>-->std::string转换 */
template<class T>
class LexicalCast<std::list<T>, std::string> {
public:
    std::string operator()(const std::list<T>& lst) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : lst) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/* 支持从std::string-->std::map<T>转换  */
template<class T>
class LexicalCast<std::string, std::map<std::string, T>> {
public:
    std::map<std::string, T> operator()(const std::string& str) {
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;
        std::map<std::string, T> v;
        for (auto it = node.begin(); it != node.end(); it++) {
            ss.str("");
            ss << it->second;
            v.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return v;
    }
};

/* 支持从std::map<T>-->std::string转换 */
template<class T>
class LexicalCast<std::map<std::string, T>, std::string> {
public:
    std::string operator()(const std::map<std::string, T>& v) {
        YAML::Node node(YAML::NodeType::Map);
        for (auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};


/* 支持从std::string-->std::unordered_map<T>转换  */
template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T>> {
public:
    std::unordered_map<std::string, T> operator()(const std::string& str) {
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;
        std::unordered_map<std::string, T> v;
        for (auto it = node.begin(); it != node.end(); it++) {
            ss.str("");
            ss << it->second;
            v.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return v;
    }
};


/* 支持从std::unordered_map<T>-->std::string转换  */
template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
public:
    std::string operator()(const std::unordered_map<std::string, T>& v) {
        YAML::Node node(YAML::NodeType::Map);
        for (auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};


/* 支持从std::string-->std::set<T>转换  */
template<class T>
class LexicalCast<std::string, std::set<T>> {
public:
    std::set<T> operator()(const std::string& str) {
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;
        std::set<T> v;
        for (size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            v.insert(LexicalCast<std::string, T>()(ss.str()));
        }

        return v;
    }
};

/* 支持从std::set<T>-->std::string转换  */
template<class T>
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator()(const std::set<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/* 支持从std::string-->std::unordered_set转换  */
template<class T>
class LexicalCast<std::string, std::unordered_set<T>> {
public:
    std::unordered_set<T> operator()(const std::string& str) {
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;
        std::unordered_set<T> v;
        for (size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            v.insert(LexicalCast<std::string, T>()(ss.str()));
        } 
        return v;
    }
};

/* 支持从std::unordered_set<T>-->std::string转换  */
template<class T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
    std::string operator()(const std::unordered_set<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/* 配置参数的派生类 */
//是个模板类，因为参数值的类型可能是各种基本类型或者stl，或者自定义类型
//Fromstr: T operastor()(const F&)
//Tostr: F operator()(const T&)
template<class T, class Fromstr = LexicalCast<std::string, T>, class Tostr = LexicalCast<T, std::string>>

class ConfigVar : public ConfigVarBase {
public:
    /* 定义智能指针 */
    typedef std::shared_ptr<ConfigVar> ptr;

    /* 定义配置变更回调函数 */
    typedef std::function<void(const T& old_value, const T& new_value)> on_change_cb;

    /* 构造函数 */
    ConfigVar(const std::string& name, const T& default_value, const std::string& description = "")
        : ConfigVarBase(name, description)
        , m_value(default_value) {
        }

    /* 将配置参数转为string,使用boost库中的lexical_cast*/
    std::string toString() override {
        try {
            //return boost::lexical_cast<std::string>(m_value);
            return Tostr()(m_value);
        }catch (std::exception& e) {
            LUCK_LOG_ERROR(LUCK_LOG_ROOT()) << "ConfigVar::toString exception"
                << e.what() << "convert: " << typeid(m_value).name() << "to string";
        }

        return "";
    }

    /* 将string转为配置参数的值 */
    bool fromString(const std::string& val) override {
        try {
            //m_value = boost::lexical_cast<T>(val);
            setValue(Fromstr()(val));
            return true;
        } catch (std::exception& e) {
            LUCK_LOG_ERROR(LUCK_LOG_ROOT()) << "ConfigVar::fromString exception"
                << e.what() << "convert:string to " << typeid(m_value).name();
        }

        return false;
    }

    /* 设置参数的值 */
    void setValue(const T& value) {
        if (m_value == value)
            return;

        for (auto& i : m_cbs) {
            i.second(m_value, value);
        }

        m_value = value;
    }

    /* 获取value的值 */
    const T& getValue() const { return m_value; }

    /* 添加回调函数 */
    void addListener(uint64_t key, on_change_cb cb) {
        m_cbs[key] = cb;
    }

    /* 删除回调函数 */
    void deleteListener(uint64_t key) {
        m_cbs.erase(key);
    }

    /* 获取回调函数 */
    on_change_cb getListener(uint64_t key) {
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }

    /* 清除所有回调函数 */
    void clearListener() {
        m_cbs.clear();
    }

private:
    /* 配置参数的值 */
    T m_value;

    /* 定义配置变更回调函数数组 */
    std::map<uint64_t, on_change_cb> m_cbs;
};

/* 配置管理器 */
class Config {
public:
    /* 设置一个类型来存储配置参数 */
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;

    /* 创建一个参数 */ 
    template<class T>
        static typename ConfigVar<T>::ptr LookUp(const std::string& name, const T& default_value, const std::string& description = "") {
            //先判断map中是否存在
            auto it = s_datas.find(name);
            if (it != s_datas.end()) {
                //转换，判断是否合法
                auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
                if (tmp) {
                    LUCK_LOG_INFO(LUCK_LOG_ROOT()) << "LookUp name = " << name << " exists";
                    return tmp;
                }
            }

            //如果不存在，先判断参数的名称是否合法
            if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos) {
                LUCK_LOG_ERROR(LUCK_LOG_ROOT()) << "LookUp name invalid" << name;
                throw std::invalid_argument(name);
            }

            //如果没有找到，创建一个放入map中
            typename ConfigVar<T>::ptr value(new ConfigVar<T>(name, default_value, description));
            s_datas[name] = value;
            return value;
        }

    /* 根据参数名称查找这个参数在map中是否存在 */
    template<class T>
        static typename ConfigVar<T>::ptr LookUp(const std::string& name) {
            auto it = s_datas.find(name);
            return (it == s_datas.end() ? std::dynamic_pointer_cast<ConfigVar<T>>(it->second) : nullptr);
        }

    /* 将yaml文件内容放入配置参数容器中 */
    static void LoadFromYaml(const YAML::Node& node);

    /* 根据名称在参数容器中查找是否存在对应的ptr */
    static ConfigVarBase::ptr LookUpBase(const std::string& name);
private:
    /* 存储配置参数容器 */
    static ConfigVarMap s_datas;
};

}

#endif
