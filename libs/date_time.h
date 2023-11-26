#ifndef BOND_CPP_DATE_TIME_H
#define BOND_CPP_DATE_TIME_H
#include <string>
#include <istream>
#include <sstream>
#include <vector>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <cmath>
#include "../utils/utils.h"

namespace df {

    /**
     * 针对业务设计的 C++ 日期类
     */
    class date_time {
    public:

        int year{};  // 年
        int month{};  // 月
        int day{};  // 日

        /**
         * 默认构造函数
         */
        explicit date_time();

        /**
         * 直接构造方法
         * @param year 年
         * @param month 月
         * @param day 日
         */
        explicit date_time(int year, int month, int day);

        /**
         * 从字符串构造日期
         * @param str_date_time 日期字符串 yyyy/mm/dd 格式
         */
        explicit date_time(const std::string & str_date_time);

        /**
         * 拷贝构造函数
         * @param date 其他日期
         */
        date_time(const date_time& date);

        /**
         * 是否为闰年
         * @param year 年份
         * @return bool 是否为闰年
         */
        [[nodiscard]] bool is_leap() const;

        /**
         * 获取某年某月的天数
         * @param year 年份
         * @param month 月份
         * @return 天数
         */
        static int get_month_day(int year, int month);

        /**
         * 判断日期是否合法
         * @return 是否合法
         */
        [[nodiscard]] bool is_valid() const;

        /**
         * 重载赋值运算符
         * @param d 其他日子
         */
        date_time& operator=(const date_time &d);

        // 日期类之间的判断
        /**
         * 判断是否大于
         * @param dt 其他日期
         * @return bool
         */
        bool operator>(const date_time & dt) const;

        /**
         * 判断是否等于
         * @param dt 其他日期
         * @return bool
         */
        bool operator==(const date_time & dt) const;

        /**
         * 判断是否大于等于
         * @param dt 其他日期
         * @return bool
         */
        bool operator>=(const date_time & dt) const;

        /**
         * 判断是否小于等于
         * @param dt 其他日期
         * @return bool
         */
        bool operator<=(const date_time & dt) const;

        /**
         * 判断是否小于
         * @param dt 其他日期
         * @return bool
         */
        bool operator<(const date_time & dt) const;

        /**
         * 判断是否不等于
         * @param dt 其他日期
         * @return bool
         */
        bool operator!=(const date_time & dt) const;

        /**
         * 日期加天数，得到一个新的日期
         * @param days 天数
         * @return 新的日期
         */
        date_time operator+(int days);

        /**
         * 对日期本身加天数
         * @param days 天数
         * @return 日期本身
         */
        date_time& operator+=(int days);

        /**
         * 对日期本身减天数
         * @param days 天数
         * @return 日期本身
         */
        date_time& operator-=(int days);

        /**
         * 日期减天数，得到一个新的日期
         * @param days 天数
         * @return 日期本身
         */
        date_time operator-(int days);

        /**
         * 计算两个日期之间的差值
         * @param dateTime 其他日期
         * @return 差的天数
         */
        int operator-(date_time& dateTime);

        /**
         * 计算该年中含有的天数
         * @return 天数
         */
        int days_in_year();

        /**
         * 这一年有多少天
         * @return 天数
         */
        [[nodiscard]] int full_year_days() const;

        /**
         * 将日期转换成字符串
         * @param separator 分隔符
         * @return date str
         */
        [[nodiscard]] std::string to_string(const std::string& separator) const;
    };

} // df

#endif //BOND_CPP_DATE_TIME_H
