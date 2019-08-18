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

namespace Luck {

/* 配置参数的基类 */
class ConfigVarBase {
public:
    /* 定义智能指针 */
    typedef std::shared_ptr<ConfigVarBase> ptr;

    /* 构造函数，初始化成员变量 */
    ConfigVarBase(const std::string& name, const std::string& description = "")
        : m_name(name)
        , m_description(description) {}
    
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

/* 配置参数的派生类 */
//是个模板类，因为参数值的类型可能是各种基本类型或者stl，或者自定义类型
template<class T>
class ConfigVar : public ConfigVarBase {
public:
    /* 定义智能指针 */
    typedef std::shared_ptr<ConfigVar> ptr;

    /* 构造函数 */
    ConfigVar(const std::string& name, const std::string& default_value, const std::string& description = "")
        : ConfigVarBase(name, description)
        , m_value(default_value) {
    }

    /* 将配置参数转为string,使用boost库中的lexical_cast*/
    std::string toString() override {
        try {
            return boost::lexical_cast<std::string>(m_value);
        }catch (std::exception& e) {
            std::cout << "toString error" << std::endl;
        }

        return "";
    }

    /* 将string转为配置参数的值 */
    bool fromString(const std::string& val) override {
        try {
            m_value = boost::lexical_cast<T>(val);
            return true;
        } catch (std::exception& e) {
            std::cout << "fromString error" << std::endl;
        }

        return false;
    }

    /* 设置参数的值 */
    void setValue(const T& value) { m_value = value; }

    /* 获取value的值 */
    const T& getValue() const { return m_value; }

private:
    /* 配置参数的值 */
    T m_value;
};

/* 配置管理器 */
class Config {
public:
    /* 设置一个类型来存储配置参数 */
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    
    /* 创建一个参数 */ 
    template<class T>
    typename ConfigVar<T>::ptr LookUp(const std::string& name, const std::string& default_value, const std::string& description = "") {
        //先判断map中是否存在
        auto it = s_datas.find(name);
        if (it != s_datas.end()) {
            //转换，判断是否合法
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
            if (tmp) {
                std::cout << "LookUp name: " << name << " exists" << std::endl;
                return tmp;
            } else {
                std::cout << "LookUp name: " << name << " exists but type is invalid" << std::endl;
                return nullptr;
            }
        }

        //如果没有找到，创建一个放入map中
        typename ConfigVar<T>::ptr value(new ConfigVar<T>(name, default_value, description));
        s_datas[name] = value;
        return value;
    }

    /* 根据参数名称查找这个参数在map中是否存在 */
    template<class T>
    typename ConfigVar<T>::ptr LookUp(const std::string& name) {
        auto it = s_datas.find(name);
        return (it == s_datas.end() ? std::dynamic_pointer_cast<ConfigVar<T>>(it->second) : nullptr);
    }


private:
    /* 存储配置参数容器 */
    ConfigVarMap s_datas;
};

}

#endif