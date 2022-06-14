#include "datautil.h"

DataUtil::DataUtil()
{

}

///
/// \brief cstrToNumber: char数据转到数字
/// \param dest：目标
/// \param src：源
/// \param pos：在src中的偏移量
/// \param bSameEndien：是否与本机大小端一样
///
//template<class NUM>
//void DataUtil::cstrToNumber(NUM &dest, const char *src, int &pos, bool bSameEndien)
//{
//    cstrToNumber1(dest, src + pos, bSameEndien);
//    pos += sizeof (NUM);
//}
///
//template<class NUM>
//void DataUtil::cstrToNumber1(NUM &dest, const char *src, bool bSameEndien)
//{
//    if(bSameEndien) {
//        memcpy(&dest, src, sizeof (NUM));
//    }else {
//        QByteArray tmpArray;
//        int ns = sizeof (NUM);
//        for(int n = 0, n1 = ns - 1; n < ns; n ++, n1 --) {
//            tmpArray.append(src + n1, 1);
//        }
//    }
//}

/////
///// \brief cstrTo1Vector: 拷贝二进制数据到vector
///// \param lt
///// \param count
///// \param data
///// \param pos
///// \param bSameEndien
/////
//template<class NUM>
//void DataUtil::cstrTo1Vector(std::vector<NUM> &lt, int count, const char *data, int &pos, bool bSameEndien)
//{
//    lt.resize(count);
//    for(int n = 0; n < count; n ++) {
//        NUM num;
//        DataUtil::cstrToNumber<NUM>(num, data, pos, bSameEndien);
//        lt[n] = num;
//    }
//}
