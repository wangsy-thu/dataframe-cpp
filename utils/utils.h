#include <string>
#include <algorithm>
#include <vector>
#include <istream>
#include <cassert>
#include <sstream>
#include <functional>
#include <cmath>
#ifndef BOND_CPP_UTILS_H
#define BOND_CPP_UTILS_H

namespace df {

    typedef std::function<double(double)> target_func;
    /**
     * 字符串去空格
     * @param str 原字符串
     * @return 新字符串
     */
    std::string strip(const std::string& str);

    /**
     * 字符串转换小写
     * @param str 字符串
     * @return 新字符串
     */
    std::string to_lower(const std::string& str);

    /**
     * 字符串转换大写
     * @param str 字符串
     * @return 新字符串
     */
    std::string to_upper(const std::string& str);

    /**
     * 字符串切分
     * @param str 待切分字符串
     * @param delim 切分元素
     * @return 切分结果
     */
    std::vector<std::string> str_split(const std::string& str, char delim);

    /**
     * 对列表进行排序，返回对应位置索引
     * @tparam T 数据类型
     * @param lst 列表
     * @param order 顺序 asc: 升序 desc: 降序
     * @return 对应索引顺序
     */
    template<typename T>
    std::vector<int> sort_index(std::vector<T>& lst, const std::string& order){
        assert(order == "asc" || order == "desc");
        std::vector<int> idx(lst.size());
        for(int i = 0; i < idx.size(); i ++) {
            idx[i] = i;
        }
        std::sort(idx.begin(), idx.end(), [&lst, &order](int i1, int i2) -> bool {
            if (order == "asc") {
                return lst[i1] <= lst[i2];
            } else {
                return lst[i1] >= lst[i2];
            }
        });
        return idx;
    }

    /**
     * 牛顿法求解方程根
     * @param func 方程
     * @param x0 初始结果
     * @return
     */
    double f_solve(const target_func& func, double x0, int max_iter);
}

#endif //BOND_CPP_UTILS_H
