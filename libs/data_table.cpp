#include "data_table.h"

df::data_table::data_table() {
    this->_data_table = main_table();
    this->tb_size = 0;
    this->index_map = field_index_map();
    this->col_names = column_name_list();
    this->col_type = field_type_map();
}

df::data_table::data_table(const df::data_table &tb) {
    this->_data_table = tb._data_table;
    this->tb_size = tb.tb_size;
    this->index_map = tb.index_map;
    this->col_names = tb.col_names;
    this->col_type = tb.col_type;
}

[[maybe_unused]] df::data_table::data_table(const std::string& file_name) {
    std::ifstream csv_data_file(file_name, std::ios::in);
    std::string line, token;
    this->index_map = field_index_map();
    this->tb_size = 0;
    std::vector<bool> is_defined;

    // 1, 读取字段名称
    std::getline(csv_data_file, line);
    // 2, 解析字段表
    this->col_names = column_name_list();
    this->col_type = field_type_map();
    std::istringstream iss(line);
    std::istringstream tss;
    while(std::getline(iss, token, ',')) {
        this->col_names.emplace_back(token);
        is_defined.emplace_back(false);
    }
    iss.clear();

    // 3, 按字段表初始化数据表
    this->_data_table = main_table();
    for(auto & col_name : this->col_names) {
        // 遍历所有的字段名称
        data_column col;
        this->_data_table.emplace(col_name, col);
    }

    // 4, 按行读取数据
    while(std::getline(csv_data_file, line)) {
        iss.str(line);
        int col_idx = 0;

        while(std::getline(iss, token, ',')) {

            // 每次加入，判断类型并加入类型表
            if(!is_defined[col_idx]) {
                if(std::regex_match(token, DOUBLE_REGEX)) {
                    // 如果是 double 类型
                    if(this->col_type.find(this->col_names[col_idx]) == this->col_type.end()) {
                        this->col_type.insert(std::make_pair(this->col_names[col_idx], 2));
                    } else {
                        this->col_type[this->col_names[col_idx]] = 2;
                    }
                    is_defined[col_idx] = true;
                } else {
                    // 否则就是 std::string 类型
                    if(token.empty()) {
                        this->col_type.insert(std::make_pair(this->col_names[col_idx], 1));
                    } else {
                        this->col_type.insert(std::make_pair(this->col_names[col_idx], 1));
                        is_defined[col_idx] = true;
                    }
                }
            }

            if(this->col_type[this->col_names[col_idx]] == 2) {
                // 如果是 double 类型，且非空
                if(token.empty()) {
                    this->_data_table[this->col_names[col_idx]].emplace_back(token);
                } else {
                    this->_data_table[this->col_names[col_idx]].emplace_back(std::stod(token));
                }
            } else {
                // 否则就是 std::string 类型
                this->_data_table[this->col_names[col_idx]].emplace_back(token);
            }
            col_idx ++;
        }
        if(col_idx == this->col_names.size() - 1) {
            // 末尾数据缺失，默认以空串存储
            this->_data_table[this->col_names[col_idx]].emplace_back("");
        }
        this->tb_size ++;
        iss.clear();
    }
    // 如果到最后还没定下来数据类型，默认为字符
    for(int i = 0; i < is_defined.size(); i ++) {
        if(!is_defined[i]) {
            this->col_type.emplace(this->col_names[i], 1);
        }
    }
}

df::u_int df::data_table::row_size() const {
    return this->tb_size;
}

df::u_int df::data_table::col_size() const {
    return this->col_names.size();
}

void df::data_table::build_index(const std::string& col_name) {
    // TODO: 将 Hash 表索引修改成平衡树桶索引
    data_column build_index_target = this->_data_table[col_name];
    bucket_field_index f_idx;
    u_int line_idx = 0;
    // 构建索引
    for(auto & it : build_index_target) {
        // 首先判断一下是否有桶
        if(f_idx.find(it) == f_idx.end()) {
            std::set<u_int> bucket;
            f_idx.emplace(it, bucket);
        }
        f_idx[it].emplace(line_idx);
        line_idx ++;
    }
    // 绑定索引名
    this->index_map.emplace(col_name, f_idx);
}

df::idx_list df::data_table::eq_filter_idx(const std::vector<eq_predicate>& predicates) {
    idx_list res;
    idx_list temp_res;
    bool first = true;
    // 遍历所有的谓词
    for(auto & predicate : predicates) {
        if(this->index_map.find(predicate.first) != index_map.end()) {
            // 有索引，直接走索引
            bucket_field_index f_idx = this->index_map[predicate.first];
            // 根据索引找到对应的 ID
            temp_res = f_idx[predicate.second];
        } else {
            // 没有索引，走循环遍历
            data_column predicate_col = this->_data_table[predicate.first];
            for(u_int idx = 0; idx < this->tb_size; idx++) {
                if(predicate_col[idx] == predicate.second) {
                    temp_res.emplace(idx);
                }
            }
        }
        idx_list intersection_tmp;
        // 求交集
        if(first) {
            // 第一次不用交，直接赋值
            res = temp_res;
            first = false;
            temp_res.clear();
        } else {
            std::set_intersection(res.begin(), res.end(), temp_res.begin(), temp_res.end(),
                                  std::insert_iterator<idx_list>(intersection_tmp, intersection_tmp.begin()));
            res = intersection_tmp;
            temp_res.clear();
            intersection_tmp.clear();
        }

        // 若集合大小变成 0，提前结束
        if(res.empty()) {
            break;
        }
    }
    return res;
}

df::data_column df::data_table::fetch_column(const std::string& col_name, const std::set<u_int>& idx_lst) {
    data_column source_column = this->_data_table[col_name];
    data_column result;
    for(u_int id : idx_lst) {
        result.emplace_back(source_column[id]);
    }
    return result;
}

void df::data_table::add_column(const std::string &col_name, const df::data_column& col_data) {
    // 若表格为空，需要初始化避免异常
    assert(!col_data.empty());
    // 处理类型
    this->col_type.insert(std::make_pair(col_name, col_data[0].index()));
    if(this->tb_size == 0 && this->col_names.empty()) {
        this->tb_size = col_data.size();
    }
    if(col_data.size() != this->tb_size) {
        std::cout << "Number of Item is not equal to row size." << std::endl;
        return;
    }
    this->_data_table.emplace(col_name, col_data);
    this->col_names.emplace_back(col_name);
}

df::data_column df::data_table::get_column(const std::string &col_name) {
    return this->_data_table[col_name];
}

void df::data_table::save_to_csv(const std::string& file_name) {
    std::ofstream csv_data_file(file_name, std::ios::out);
    // 将字段名称写入 csv 中
    csv_data_file << col_names[0];
    for(int col = 1; col < this->col_names.size(); col ++) {
        csv_data_file << "," << col_names[col];
    }
    csv_data_file << std::endl;

    // 将数据逐行写入 csv 文件
    for(int line = 0; line < this->tb_size; line++) {
        // 逐字段写入

        if(this->col_type[this->col_names[0]] == 1) {
            csv_data_file << std::get<1>(this->_data_table[col_names[0]][line]);
        } else {
            if(this->_data_table[col_names[0]][line].index() == 1 && std::get<1>(this->_data_table[col_names[0]][line]).empty()) {
                // 如果是空数据，则直接打印
                csv_data_file << std::get<1>(this->_data_table[col_names[0]][line]);
            } else {
                csv_data_file << std::get<2>(this->_data_table[col_names[0]][line]);
            }
        }
        for(int col = 1; col < this->col_names.size(); col ++) {
            if(this->col_type[this->col_names[col]] == 1) {
                csv_data_file << "," << std::get<1>(this->_data_table[col_names[col]][line]);
            } else {
                if(this->_data_table[col_names[col]][line].index() == 1 && std::get<1>(this->_data_table[col_names[col]][line]).empty()) {
                    // 如果是空数据，则直接打印
                    csv_data_file << "," << std::get<1>(this->_data_table[col_names[col]][line]);
                } else {
                    csv_data_file << "," << std::get<2>(this->_data_table[col_names[col]][line]);
                }
            }
        }
        csv_data_file << std::endl;
    }
    csv_data_file.close();
}

df::data_row df::data_table::get_row(df::u_int idx) {
    data_row res_row;
    assert(idx < this->tb_size);  // 判断行索引是否超过表格大小

    for(auto & col_name : this->col_names) {
        res_row.insert(std::make_pair(col_name,
                                      this->_data_table[col_name][idx]));
    }
    return res_row;
}

df::data_item df::data_table::loc(df::u_int idx, const std::string &col_name) {
    // id 没越界 且 能找到这个列
    assert(idx < this->tb_size);
    assert(this->_data_table.find(col_name) != this->_data_table.end());
    return this->_data_table[col_name][idx];
}
std::vector<int> df::data_table::sort_idx(const std::string &col_name, const std::string &order) {
    assert(std::find(this->col_names.begin(), this->col_names.end(), col_name) != this->col_names.end());
    assert(order == "asc" || order == "desc");
    std::vector<int> idx(this->_data_table[col_name].size());
    for(int i = 0; i < idx.size(); i ++) {
        idx[i] = i;
    }
    std::sort(idx.begin(), idx.end(), [this, &col_name, &order](int i1, int i2) -> bool {
        if (order == "asc") {
            return this->_data_table[col_name][i1] <= this->_data_table[col_name][i2];
        } else {
            return this->_data_table[col_name][i1] >= this->_data_table[col_name][i2];
        }
    });
    return idx;
}

df::data_table
df::data_table::join_table(df::data_table &left_table, df::data_table &right_table,
                           std::vector<std::string>& on, const std::string& how) {
    assert(how == "inner" || how =="left" || how == "right");
    data_table t;
    int left_count = (int) left_table.tb_size;
    int right_count = (int) right_table.tb_size;

    if(how == "inner") {
        // 最简单的内连接
        for(int idx_left = 0; idx_left < left_count; idx_left ++) {
            for(int idx_right = 0; idx_right < right_count; idx_right ++) {
                if(data_table::_can_join(idx_left, idx_right, [&left_table, &right_table, &on](int left, int right) -> bool {
                    for(const std::string& col_name : on) {
                        if(left_table.loc(left, col_name) == right_table.loc(right, col_name)) {
                            continue;
                        } else {
                            return false;
                        }
                    }
                    return true;
                })) {
                    data_row left_row = left_table.get_row(idx_left);
                    data_row right_row = right_table.get_row(idx_right);
                    data_row new_row = data_table::_merge_row(left_row, right_row);
                    t._add_row_join(left_table.col_type, right_table.col_type, new_row);
                }
            }
        }
    } else if(how == "left") {
        // 左外连接，遍历左表，如果右表没有数据对应，则创建空值
        for(int idx_left = 0; idx_left < left_count; idx_left ++) {
            int left_target = 0;
            for(int idx_right = 0; idx_right < right_count; idx_right ++) {
                if(data_table::_can_join(idx_left, idx_right, [&left_table, &right_table, &on](int left, int right) -> bool {
                    for(const std::string& col_name : on) {
                        if(left_table.loc(left, col_name) == right_table.loc(right, col_name)) {
                            continue;
                        } else {
                            return false;
                        }
                    }
                    return true;
                })) {
                    data_row left_row = left_table.get_row(idx_left);
                    data_row right_row = right_table.get_row(idx_right);
                    data_row new_row = data_table::_merge_row(left_row, right_row);
                    t._add_row_join(left_table.col_type, right_table.col_type, new_row);
                    left_target++;
                }
            }
            if(left_target == 0) {
                data_row left_row = left_table.get_row(idx_left);
                std::vector<std::string> right_names = right_table.col_names;
                for(const std::string& c_name : right_names) {
                    if(left_row.find(c_name) == left_row.end()) {
                        left_row.emplace(c_name, "");
                    }
                }
                t._add_row_join(left_table.col_type, right_table.col_type, left_row);
            }
        }
    } else if(how == "right") {
        // 右外连接，遍历右表，如果左表没有数据对应，则创建空值
        for (int idx_right = 0; idx_right < right_count; idx_right++) {
            int right_target = 0;
            for(int idx_left = 0; idx_left < left_count; idx_left ++) {
                if (data_table::_can_join(idx_left, idx_right,[&left_table, &right_table, &on](int left, int right) -> bool {
                      for (const std::string &col_name: on) {
                          if (left_table.loc(left, col_name) == right_table.loc(right, col_name)) {
                              continue;
                          } else {
                              return false;
                          }
                      }
                      return true;
              })) {
                    data_row left_row = left_table.get_row(idx_left);
                    data_row right_row = right_table.get_row(idx_right);
                    data_row new_row = data_table::_merge_row(left_row, right_row);
                    t._add_row_join(left_table.col_type, right_table.col_type, new_row);
                    right_target++;
                }
            }
            if (right_target == 0) {
                data_row right_row = right_table.get_row(idx_right);
                std::vector<std::string> left_names = left_table.col_names;
                for (const std::string &c_name: left_names) {
                    if (right_row.find(c_name) == right_row.end()) {
                        right_row.emplace(c_name, "");
                    }
                }
                t._add_row_join(left_table.col_type, right_table.col_type, right_row);
            }
        }
    }
    return t;
}

bool df::data_table::_can_join(int idx_left, int idx_right, const df::join_predicate& j_pre) {
    return j_pre(idx_left, idx_right);
}

df::data_row df::data_table::_merge_row(df::data_row &row1, df::data_row &row2) {
    data_row res;
    for(auto kv : row1) {
        res.emplace(kv.first, kv.second);
    }
    for(auto kv : row2) {
        res.emplace(kv.first, kv.second);
    }
    return res;
}

void df::data_table::add_row(df::data_row& row) {
    if(this->tb_size == 0 && this->col_names.empty()) {
        for(const auto& kv : row) {
            this->col_names.emplace_back(kv.first);
            this->_data_table[kv.first].emplace_back(kv.second);
            this->col_type.emplace(kv.first, kv.second.index());
        }
        this->tb_size++;
    } else {
        // 该表中有数据，则需要判定数据是否可加
        for(const auto& kv : row) {
            if(std::find(this->col_names.begin(), this->col_names.end(), kv.first) == this->col_names.end()) {
                // 数据添加失败
                return;
            }
        }
        for(const auto& kv : row) {
            this->_data_table[kv.first].emplace_back(kv.second);
        }
        this->tb_size++;
    }
}

void df::data_table::_add_row_join(field_type_map &left_types, field_type_map &right_types,
                                   df::data_row &row) {

    if(this->tb_size == 0 && this->col_names.empty()) {
        field_type_map new_types;
        for(const auto& kv : left_types) {
            new_types.emplace(kv.first, kv.second);
        }
        for(const auto& kv : right_types) {
            new_types.emplace(kv.first, kv.second);
        }
        for(const auto& kv : row) {
            this->col_names.emplace_back(kv.first);
            this->_data_table[kv.first].emplace_back(kv.second);
        }
        this->col_type = new_types;
        this->tb_size++;
    } else {
        // 该表中有数据，则需要判定数据是否可加
        for(const auto& kv : row) {
            if(std::find(this->col_names.begin(), this->col_names.end(), kv.first) == this->col_names.end()) {
                // 数据添加失败
                return;
            }
        }
        for(const auto& kv : row) {
            this->_data_table[kv.first].emplace_back(kv.second);
        }
        this->tb_size++;
    }
}

df::data_table df::data_table::eq_filter(const std::vector<eq_predicate>& predicates) {
    data_table new_table;
    df::idx_list res_idx = this->eq_filter_idx(predicates);
    for(df::u_int idx = 0; idx < this->tb_size; idx ++) {
        if(res_idx.find(idx) != res_idx.end()) {
            data_row row_filtered = this->get_row(idx);
            new_table.add_row(row_filtered);
        }
    }
    return new_table;
}

df::data_table df::data_table::sort(const std::string &sort_column_name, const std::string &order) {
    assert(std::find(this->col_names.begin(), this->col_names.end(), sort_column_name) != this->col_names.end());
    assert(order == "asc" || order == "desc");
    std::vector<int> idx(this->_data_table[sort_column_name].size());
    data_table new_table;
    for(int i = 0; i < idx.size(); i ++) {
        idx[i] = i;
    }
    std::sort(idx.begin(), idx.end(), [this, &sort_column_name, &order](int i1, int i2) -> bool {
        if (order == "asc") {
            return this->_data_table[sort_column_name][i1] <= this->_data_table[sort_column_name][i2];
        } else {
            return this->_data_table[sort_column_name][i1] >= this->_data_table[sort_column_name][i2];
        }
    });
    for(df::u_int i : idx) {
        data_row r = this->get_row(i);
        new_table.add_row(r);
    }
    return new_table;
}

df::data_table df::data_table::projection(const std::vector<std::string> &pro_names) {
    data_table new_table;
    for(const std::string& name : pro_names) {
        data_column pro_col = this->get_column(name);
        new_table.add_column(name, pro_col);
    }
    return new_table;
}

bool df::data_table::is_nan(df::data_item item) {
    return item.index() == 1 && std::get<1>(item).empty();
}

double interp1(double interp_x, df::data_column& x, df::data_column& y) {
    assert(x.size() == y.size());
    assert(!x.empty());
    assert(x[0].index() == 2);
    assert(y[0].index() == 2);
    double result = 0;  // 结果变量

    // 首先对 y 进行排序
    std::vector<int> idx(x.size());
    for(int i = 0; i < idx.size(); i ++) {
        idx[i] = i;
    }
    std::sort(idx.begin(), idx.end(), [&x](int i1, int i2) -> bool {
        return x[i1] < x[i2];
    });
    if(interp_x < std::get<2>(x[idx[0]])) {
        return std::get<2>(y[idx[0]]);
    }
    if(interp_x > std::get<2>(x[*(idx.end() - 1)])) {
        return std::get<2>(y[*(idx.end() - 1)]);
    }
    for(int i = 0; i < x.size(); i ++) {
        if(interp_x == std::get<2>(x[idx[i]])) {
            // 与横坐标等值
            result = std::get<2>(y[idx[i]]);
            break;
        }
        if(interp_x < std::get<2>(x[idx[i]])) {
            double k = (std::get<2>(x[idx[i]]) - interp_x) / (std::get<2>(x[idx[i]]) - std::get<2>(x[idx[i - 1]]));
            result = std::get<2>(y[idx[i]]) - (std::get<2>(y[idx[i]]) - std::get<2>(y[idx[i - 1]])) * k;
            break;
        }
    }
    return result;
}
