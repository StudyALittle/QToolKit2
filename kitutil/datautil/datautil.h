#ifndef DATAUTIL_H
#define DATAUTIL_H

#include <QByteArray>
#include <vector>
#include <memory>

namespace wkit {

class DataUtil
{
public:
    DataUtil();

    template<class C_T>
    static void cBinaryToChart(C_T *dest, const char *src, int &pos, int len, bool bSameEndien = true)
    {
        if(bSameEndien) {
            memcpy(dest, src, len);
        }else {
            for(int n = 0; n < len; n ++) {
                *(dest + n) = *(pos + src + len - 1 - n);
            }
        }
        pos += len;
    }

    ///
    /// \brief cstrToNumber: char 二进制数据转到数字
    /// \param dest：目标
    /// \param src：源
    /// \param pos：在src中的偏移量
    /// \param bSameEndien：是否与本机大小端一样
    ///
    template<class NUM>
    static void cBinaryToNumber(NUM &dest, const char *src, int &pos, bool bSameEndien = true)
    {
        cBinaryToNumber1(dest, src + pos, bSameEndien);
        pos += sizeof (NUM);
    }
    ///
    template<class NUM>
    static void cBinaryToNumber1(NUM &dest, const char *src, bool bSameEndien = true)
    {
        if(bSameEndien) {
            memcpy(&dest, src, sizeof (NUM));
        }else {
            QByteArray tmpArray;
            int ns = sizeof (NUM);
            for(int n = 0, n1 = ns - 1; n < ns; n ++, n1 --) {
                tmpArray.append(src + n1, 1);
            }
            memcpy(&dest, tmpArray.data(), sizeof (NUM));
        }
    }

    ///
    /// \brief cstrTo1Vector: 拷贝二进制数据到vector
    /// \param lt
    /// \param count
    /// \param data
    /// \param pos
    /// \param bSameEndien
    ///
    template<class NUM>
    static void cBinaryTo1Vector(std::vector<NUM> &lt, int count, const char *data, int &pos, bool bSameEndien = true)
    {
        lt.resize(count);
        for(int n = 0; n < count; n ++) {
            NUM num;
            DataUtil::cBinaryToNumber<NUM>(num, data, pos, bSameEndien);
            lt[n] = num;
        }
    }
    template<class NUM>
    static void cBinaryTo2Vector(std::vector<std::vector<NUM> > &lt1,
                              int count1, int count2, const char *data, int &pos, bool bSameEndien = true)
    {
        lt1.resize(count1);
        for(int j = 0; j < count1; j ++){
            auto &m = lt1[j];
            cBinaryTo1Vector<NUM>(m, count2, data, pos, bSameEndien);
        }
    }
    template<class NUM>
    static void cBinaryTo3Vector(std::vector<std::vector<std::vector<NUM> > > &dest,
                     int count1, int count2, int count3,
                          const char *src, int &pos, bool bSameEndien = true) {
        dest.resize(count1);
        for(int i = 0; i < count1; i ++){
            auto &hv = dest[i]; //获取句柄
            cBinaryTo2Vector<NUM>(hv, count2, count3, src, pos, bSameEndien);
        }
    }

    ///
    /// \brief numToByteArray: 把数组/数据添加到qbytearray中
    /// \param data
    /// \param d
    ///
    template<class NUM>
    static void numAppendToArray(std::shared_ptr<QByteArray> data, NUM d) {
        data->append((const char*)(&d), sizeof (NUM));
    }
    template<class NUM>
    static void numV1AppendToArray(const std::vector<NUM> &lt, std::shared_ptr<QByteArray> data) {
        for(double d : lt) {
            numAppendToArray<NUM>(data, d);
        }
    }
    template<class NUM>
    static void numV2AppendToArray(const std::vector<std::vector<NUM> > &lt, std::shared_ptr<QByteArray> data) {
        for(const auto &v1 : lt) {
            numV1AppendToArray<NUM>(v1, data);
        }
    }
    template<class NUM>
    static void numV3AppendToArray(const std::vector<std::vector<std::vector<NUM> > > &lt, std::shared_ptr<QByteArray> data) {
        for(const auto &v2 : lt) {
            numV2AppendToArray<NUM>(v2, data);
        }
    }

    // 判断本机大小端模式
    static inline bool isBigEndien()
    {
        short iValue = 0x01;
        char cValue = '\n';
        memcpy(&cValue, (char*)&iValue, (size_t)1);

        if (1 == cValue)
            return false;
        else
            return true;
    }
};

} // end namespace

#endif // DATAUTIL_H
