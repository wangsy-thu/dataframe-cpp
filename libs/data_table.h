#include <variant>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <iostream>
#include <set>
#include <map>
#include <cstdarg>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <regex>


#ifndef FXOPT_CPP_DATA_TABLE_H
#define FXOPT_CPP_DATA_TABLE_H

const std::regex DOUBLE_REGEX("([0-9]+)|([0-9]+.[0-9]+)|(-[0-9]+)|(-[0-9]+.[0-9]+)");


namespace df {

    // 定义数据类型
    typedef unsigned int u_int;
    typedef std::variant<int, std::string, double> data_item;  // 单元格数据类型
    typedef std::vector<data_item> data_column; // 数据列
    typedef std::map<data_item, std::set<u_int>> bucket_field_index; // 平衡树桶索引
    typedef std::unordered_map<std::string, data_column> main_table;  // 主要数据表
    typedef std::vector<std::string> column_name_list;  // 表格字段名称集合
    typedef std::unordered_map<std::string, bucket_field_index> field_index_map;  // 字段索引管理器
    typedef std::unordered_map<std::string, u_int> field_type_map;  // 字段类型映射
    typedef std::set<u_int> idx_list;  // 表格的 id 列表
    typedef struct predicate {
        std::string col_name;  // 列名
        std::string opt;  // 谓词函数，gt(>) gte(>=) lt(<) lte(<=) eq(==)
        df::data_item value;  // 谓词目标，比较的目标
    } normal_predicate;
    typedef std::pair<std::string, data_item> eq_predicate;  // 等值谓词
    typedef std::map<std::string, data_item> data_row;  // 数据行
    typedef std::function<bool(int, int)> join_predicate;

    /**
     * 自定义数据表类
     */
    class data_table {
    private:
        // 数据表
        main_table _data_table;
        // 主要索引，从 0 开始
        u_int tb_size;
        // 其他字段索引管理，一般用不上
        field_index_map index_map;
        // 列名称
        column_name_list col_names;
        // 字段类型表
        field_type_map col_type;

        /**
         * 判断两行是否可以连接
         * @param idx_left 左侧id
         * @param idx_right 右侧id
         * @param j_pre 连接谓词
         * @return 是否连接
         */
        static bool _can_join(int idx_left, int idx_right, const join_predicate& j_pre);

        /**
         * 为了连表算法设计的添加行
         * @param left_types 左侧数据类型
         * @param right_types 右侧数据类型
         * @param row 新加行
         */
        void _add_row_join(field_type_map& left_types, field_type_map& right_types, data_row& row);

        /**
         * 混合两行
         * @param row1 左表对应的一行
         * @param row2 右表对应的一行
         * @return 合并后的行
         */
        static data_row _merge_row(data_row& row1, data_row& row2);

    public:
        /**
         * 默认构造方法
         */
        explicit data_table();

        /**
         * 从其他 data table 拷贝初始化
         * @param tb 其他 data_table_F 实例
         */
        data_table(const data_table& tb);

        /**
         * 从 csv 文件中初始化
         * @param file_name 文件名
         */
        [[maybe_unused]] explicit data_table(const std::string& file_name);

        /**
         * 返回表格的行数
         * @return 行数 Unsigned int
         */
        u_int row_size() const;

        /**
         * 返回表格的列数
         * @return 列数 Unsigned int
         */
        u_int col_size() const;

        /**
         * 根据字段名称与索引列表取数值
         * @param col_name 字段名称
         * @param idx_lst 索引列表
         * @return 数据行
         */
        data_column fetch_column(const std::string& col_name, const std::set<u_int>& idx_lst);

        /**
         * 挑选字段建立索引
         * @param col_name 要建立索引的列名
         */
        void build_index(const std::string& col_name);

        /**
         * 等职谓词过滤
         * @param predicates 等值谓词列表
         * @return 满足条件的 id 列表
         */
        idx_list eq_filter_idx(const std::vector<eq_predicate>& predicates);

        /**
         * 添加列
         * @param col_name 列名称
         * @param col_data 列数据
         */
        void add_column(const std::string& col_name, const data_column& col_data);

        /**
         * 根据列名获取列数据
         * @param col_name 列名
         * @return 列数据
         */
        data_column get_column(const std::string& col_name);

        /**
         * 将 data_table 保存到 csv 文件中
         * @param file_name 文件路径名称
         */
        void save_to_csv(const std::string& file_name);

        /**
         * 获取 data_table 中的一行
         * @param idx 需要的行 ID
         * @return 一行数据 data_row
         */
        data_row get_row(u_int idx);

        /**
         * 定位表格某个元素的值
         * @param idx 行 id
         * @param col_name 列名称
         * @return 元素值
         */
        data_item loc(u_int idx, const std::string& col_name);

        /**
         * 根据列进行排序，获得排序后的索引
         * @param col_name 待排序的列
         * @param order 排序顺序
         * @return 排序后的索引
         */
        std::vector<int> sort_idx(const std::string& col_name, const std::string& order);

        /**
         * 连表算法
         * @param left 左表
         * @param right 右表
         * @param on 哪列连接
         * @param how 连表方式
         * @return 新的表
         */
        static data_table join_table(data_table& left_table, data_table& right_table,
                                     std::vector<std::string>& on, const std::string& how);

        /**
         * 给表格添加一行数据
         * @param row 数据行
         */
        void add_row(data_row& row);

        /**
         * 等值过滤，这里需要生成过滤后的表
         * @param predicates 谓词列表
         * @return 过滤后的新表
         */
        data_table eq_filter(const std::vector<eq_predicate>& predicates);

        /**
         * 对表格排序，生成排序后的新表格
         * @param sort_column_name 排序的列
         * @param order 排序顺序
         * @return 排序后的新表格
         */
        data_table sort(const std::string& sort_column_name, const std::string& order);

        /**
         * 表格的投影
         * @param pro_names 投影列
         * @return 投影后的表格
         */
        data_table projection(const std::vector<std::string>& pro_names);

        /**
         * 判空函数
         * @param item 数据表格元素
         * @return 是否为空值
         */
        static bool is_nan(data_item item);
    };
}

/**
 * 一维线性插值函数
 * @param interp_x 插值的横坐标
 * @param x 横坐标序列
 * @param y 纵坐标序列
 * @return 插值横坐标对应的纵坐标
 */
double interp1(double interp_x, df::data_column& x, df::data_column& y);



#endif //FXOPT_CPP_DATA_TABLE_H
