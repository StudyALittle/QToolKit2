#ifndef NETPAGEST_H
#define NETPAGEST_H

/// 分页结构体模板

#include <QList>
#include <memory>

namespace wkit {

/// T_ONEDATA: 一条数据信息
template<typename T_ONEDATA>
struct NetPageSt {
    using Ptr = std::shared_ptr<NetPageSt>;

    int totalSize;
    int pageIndex;
    int pageSize;
    QList<std::shared_ptr<T_ONEDATA> > datas;

    NetPageSt() {
        totalSize = 0;
        pageIndex = 1;
        pageSize = 10;
    }

    NetPageSt(const NetPageSt &that) {
        copyToThis(that);
    }

    NetPageSt& operator = (const NetPageSt &that) {
        copyToThis(that);
        return *this;
    }

    void copyToThis(const NetPageSt &that) {
        if(&that != this){
            totalSize = that.totalSize;
            pageIndex = that.pageIndex;
            pageSize = that.pageSize;
            datas = that.datas;
        }
    }
};

}

#endif // NETPAGEST_H
