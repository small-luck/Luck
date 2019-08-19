#ifndef __LUCK_SINGLETON_H__
#define __LUCK_SINGLETON_H__

#include <memory>

namespace Luck {

/* 单例模式封装类 */
template<class T>
class Singleton {
public:
    static T* GetInstance() {
        static T v;
        return &v;
    }
};

/* 单例模式智能指针封装类 */
template<class T>
class SingletonPtr {
public:
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> v(new T);
        return v;
    } 
};


}

#endif /*__LUCK_SINGLETON_H__*/
