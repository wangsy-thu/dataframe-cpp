#include <iostream>
#include "libs/data_table.h"
#include "libs/date_time.h"


/**
 * 演示如何使用 data_table 类
 */
void data_table_build() {

    // 1, 从文件中构造
    df::data_table test_table("../data/left_tb.csv");

    // 2, 基础属性
    std::cout << "row size: " << test_table.row_size() << std::endl;  // 行数
    std::cout << "column size: " << test_table.col_size() << std::endl;  // 列数

    // 3, 访问表
    // 3-1, 访问列
    df::data_column col_names = test_table.get_column("name");
    std::cout << "names: ";
    for(auto name : col_names) {
        std::cout << std::get<1>(name) << " " ;
    }
    std::cout << std::endl;

    // 3-2, 访问行
    df::data_row row_person = test_table.get_row(0);
    std::cout << "person: ";
    for(auto kv : row_person) {
        std::string attr_value = kv.second.index() == 1 ?
                std::get<1>(kv.second) : std::to_string(std::get<2>(kv.second));
        std::cout << "(" << kv.first << ":" << attr_value << ") ";
    }
    std::cout << std::endl;

    // 3-3, 访问元素
    df::data_item row1_name = test_table.loc(0, "name");
    std::cout << "test_table[0]['name'] = " << std::get<1>(row1_name);
}


int main() {
    data_table_build();
    return 0;
}
