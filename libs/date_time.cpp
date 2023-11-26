#include "date_time.h"

namespace df {
    date_time::date_time(int year, int month, int day) {
        this->day = day;
        this->month = month;
        this->year = year;
    }

    date_time::date_time(const std::string &str_date_time) {
        std::istringstream iss(str_date_time);
        std::string token;
        std::vector<std::string> res;
        while(std::getline(iss, token, '/')) {
            res.emplace_back(token);
        }
        assert(res.size() == 3);
        this->year = std::stoi(res[0]);
        this->month = std::stoi(res[1]);
        this->day = std::stoi(res[2]);
    }

    bool date_time::operator>(const date_time &dt) const {
        // 枚举所有可能大于的情况
        if((this->year > dt.year) ||
            (this->year == dt.year && this->month > dt.month) ||
            (this->year == dt.year && this->month == dt.month && this->day > dt.day)) {
            return true;
        } else {
            return false;
        }
    }

    bool date_time::operator==(const date_time &dt) const {
        return this->year == dt.year && this->month == dt.month && this->day == dt.day;
    }

    bool date_time::operator>=(const date_time &dt) const {
        // 枚举所有可能大于等于的情况
        if((this->year >= dt.year) ||
           (this->year == dt.year && this->month >= dt.month) ||
           (this->year == dt.year && this->month == dt.month && this->day >= dt.day)) {
            return true;
        } else {
            return false;
        }
    }

    bool date_time::operator<=(const date_time &dt) const {
        // 枚举所有可能小于等于的情况
        if((this->year <= dt.year) ||
           (this->year == dt.year && this->month <= dt.month) ||
           (this->year == dt.year && this->month == dt.month && this->day <= dt.day)) {
            return true;
        } else {
            return false;
        }
    }

    bool date_time::operator<(const date_time &dt) const {
        // 枚举所有可能小于的情况
        if((this->year < dt.year) ||
           (this->year == dt.year && this->month < dt.month) ||
           (this->year == dt.year && this->month == dt.month && this->day < dt.day)) {
            return true;
        } else {
            return false;
        }
    }

    bool date_time::operator!=(const date_time &dt) const {
        return this->year != dt.year || this->month != dt.month || this->day != dt.day;
    }

    date_time &date_time::operator+=(int days) {
        assert(days >= 0);

        this->day += days;
        while(this->day > df::date_time::get_month_day(this->year, this->month)){
            this->day -= df::date_time::get_month_day(this->year, this->month);
            this->month ++;
            if(this->month == 13) {
                this->month = 1;
                this->year += 1;
            }
        }
        return *this;
    }

    date_time &date_time::operator-=(int days) {
        assert(days >= 0);

        this->day -= days;
        while(this->day <= 0){
            this->month --;
            if(this->month == 0) {
                this->month = 12;
                this->year -= 1;
            }
        }
        return *this;
    }

    date_time date_time::operator+(int days) {
        auto temp = date_time(*this);
        return temp += days;
    }

    date_time date_time::operator-(int days) {
        auto temp = date_time(*this);
        return temp +- days;
    }

    int date_time::get_month_day(int year, int month) {
        static int days[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int days_num = days[month];
        if(month == 2
            && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
            days_num += 1;
        }
        return days_num;
    }

    bool date_time::is_valid() const {
        return !(this->year < 1
                || this->month > 12
                || this->day < 1
                || this->day > get_month_day(this->year, this->month));
    }

    date_time::date_time(const date_time &date) {
        this->day = date.day;
        this->month = date.month;
        this->year = date.year;
    }

    int date_time::operator-(date_time &dateTime) {
        //先假设 *this > d
        int flag = 1;
        date_time max = *this;
        date_time min = dateTime;
        if (*this < dateTime)
        {
            max = dateTime;
            min = *this;
            flag = -1;
        }
        int count = 0;

        // 让最小的每次加 1 天，直到和另一个相等
        while (min != max)
        {
            min += 1;
            ++count;
        }
        return count * flag;
    }

    int date_time::days_in_year() {
        date_time year_first_day(this->year, 1, 1);
        return *this - year_first_day + 1;
    }

    int date_time::full_year_days() const {
        return ((this->year % 4 == 0 && this->year % 100 != 0) || (this->year % 400 == 0)) ? 366 : 365;
    }

    std::string date_time::to_string(const std::string& separator) const {
        return std::to_string(this->year) + separator +
        std::to_string(this->month) + separator +
        std::to_string(this->day);
    }

    bool date_time::is_leap() const {
        return ((this->year % 4 == 0 && this->year % 100 != 0) || (this->year % 400 == 0));
    }

    date_time& date_time::operator=(const date_time &d) = default;

    date_time::date_time() = default;
} // df