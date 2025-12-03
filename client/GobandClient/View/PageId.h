#ifndef PAGEID_H
#define PAGEID_H

#include <QVector>

//用于存储界面id，方便后期维护
enum class PageId
{
    Login = 0,//登录界面
    Register,//注册页面

    Count
};

//创建全局 PageId数组，方便遍历
const QVector<PageId> VPageid =
{
    PageId::Login,
    PageId::Register,

    PageId::Count
};

#endif // PAGEID_H
