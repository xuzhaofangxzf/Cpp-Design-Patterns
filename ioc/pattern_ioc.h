

#ifndef  PATTERN_IOC_H
#define  PATTERN_IOC_H

template <typename Root>
class PatternIOC
{
    typedef void * (*ct_t)();	//内存分配和构造执行函数
    typedef void (* cs_t)(void *);//仅执行构造函数
    typedef size_t (* gs_t)();//获取类的大小

    //反转控制的结构体回调
    struct node_t
    {
        ct_t ct;
        cs_t cs;
        gs_t gs;
    };

    typedef boost::unordered_map<std::string, node_t> HashMap;
public:
    /**
     * @brief 根据名字创建实体
     *
     * @param [in] name   : const std::string&	实体的类名
     * @param [in] items   : size_t	创建个数
     * @return  Root* 	返回实体的基类指针数组, NULL表示创建失败
     * @retval   
     * @see 
     * @author xiaowei
     * @date 2009/06/01 13:09:30
     **/
    Root * create (const std::string &name);
    /**
     * @brief 根据名字，和指针，初始化类
     *
     * @param [in] name   : const std::string&	实体的类名
     * @param [in/out] ptr   : void*	实体的指针
     * @param [in] items   : size_t	数组的大小
     * @return  int 0表示成功，其他失败
     * @retval   
     * @see 
     * @author xiaowei
     * @date 2009/06/01 13:10:18
     **/
    int construct(const std::string &name, void *ptr);

    /**
     * @brief 根据类名获取类的大小
     *
     * @param [in] name   : const std::string& 类名
     * @return  size_t 	类大小，0表示查询错误
     * @retval   
     * @see 
     * @author xiaowei
     * @date 2009/06/01 13:10:54
     **/
    size_t get_size(const std::string &name);

    /**
     * @brief 根据基类指针数组销毁实体
     *
     * @param [in/out] ptr   : Root*  基类指针数组
     * @return  void 
     * @retval   
     * @see 
     * @author xiaowei
     * @date 2009/06/01 13:11:31
     **/
    void destroy(Root *ptr);

    /**
     * @brief 根据基类指针数组数组，析构实体
     *
     * @param [in/out] ptr   : Root*	基类指针数组
     * @param [in/out] items   : size_t	数组个数
     * @return  void 
     * @retval   
     * @see 
     * @author xiaowei
     * @date 2009/06/01 13:11:48
     **/
    void destruct(Root *ptr);

    /**
     * @brief 将反转函数注册到ioc容器中
     *
     * @param [in/out] name   : const std::string& 类名
     * @param [in/out] ct   : ct_t	内存分配和构造执行函数
     * @param [in/out] cs   : cs_t	仅执行构造函数
     * @param [in/out] gs   : gs_t	获取类的大小的函数
     * @return  int 	注册成功返回0，其他失败
     * @retval   
     * @see 
     * @author xiaowei
     * @date 2009/06/01 13:12:49
     **/
    int add(const std::string &name, ct_t ct, cs_t cs, gs_t gs);

    /**
     * @brief check the class name whether exist in this ioc
     *
     * @param [in] name   :  class name
     * @return  int 
     * @retval   
     * @see 
     * @note 
     * @author tanghaiyuan
     * @date 2016/05/27 11:13:41
    **/
    bool check_class_name(const std::string& name);

private:
    SBRWLock _lock;
    HashMap _map;
};

template <typename Root>
Root *PatternIOC<Root>::create(const std::string &str)
{
    SBRWLockGuard __l(_lock, BT_RLOCK);
    typename HashMap::iterator iter = _map.find(str);
    if (iter == _map.end()) {
        return NULL;
    }
    try {
        Root * root = static_cast<Root*> (iter->second.ct());
        return root;
    } catch (...) {
        LOG(WARNING) << "[PatternIOC<Root>::create] create " << str << "error";
    }
    return NULL;
}

template <typename Root>
int PatternIOC<Root>::construct(const std::string &str, void *p)
{
    SBRWLockGuard __l(_lock, BT_RLOCK);
    typename HashMap::iterator iter = _map.find(str);
    if (iter == _map.end()) {
        return -1;
    }
    iter->second.cs(p);
    return 0;
}

template <typename Root>
size_t PatternIOC<Root>::get_size(const std::string &str)
{
    SBRWLockGuard __l(_lock, BT_RLOCK);
    typename HashMap::iterator iter = _map.find(str);
    if (iter == _map.end()) {
        return 0;
    }
    return iter->second.gs();
}

template <typename Root>
void PatternIOC<Root>::destroy(Root *ptr)
{
    delete ptr;
}

template <typename Root>
void PatternIOC<Root>::destruct(Root *ptr)
{
    ptr->~Root();
}

template <typename Root>
int PatternIOC<Root>::add(const std::string &str, ct_t ct, cs_t cs, gs_t gs)
{
    SBRWLockGuard __l(_lock, BT_WLOCK);
    try {
        node_t cb;
        cb.ct = ct;
        cb.cs = cs;
        cb.gs = gs;
        _map.insert(std::make_pair(str, cb));
    } catch (...) {
        LOG(WARNING) << "register " << str << " error.";
        return -1;
    }
    return 0;
}

template <typename Root>
bool PatternIOC<Root>::check_class_name(const std::string& name)
{
    SBRWLockGuard __l(_lock, BT_RLOCK);
    if (0 == _map.count(name)) {
        return false;
    }

    return true;
}
// register ioc object
extern void register_all_ioc_object();

#endif  //SPEECH_CONTROLLER_SPEECH_IOC_H

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
