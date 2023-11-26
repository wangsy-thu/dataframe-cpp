#include "utils.h"

std::string df::strip(const std::string& str) {
    return str;
}

std::string df::to_lower(const std::string& str) {
    std::string tmp;
    for(char it : str) {
        tmp.push_back((char)tolower(it));
    }
    return tmp;
}

std::string df::to_upper(const std::string &str) {
    std::string tmp;
    for(char it : str) {
        tmp.push_back((char) toupper(it));
    }
    return tmp;
}

std::vector<std::string> df::str_split(const std::string& str, char delim) {
    std::istringstream iss(str);
    std::string token;
    std::vector<std::string> result;
    while(std::getline(iss, token, delim)) {
        result.emplace_back(token);
    }
    return result;
}

double df::f_solve(const df::target_func& func, double x0, int max_iter) {
    double x, epsilon, delta_x;
    epsilon = 0.000001;
    delta_x = 0.000001;
    int k = 0;
    do {
        x = x0;
        x0 = x - func(x) / ((func(x + delta_x) - func(x - delta_x)) / (2 * delta_x));
        k ++;
        if(k >= max_iter) {
            break;
        }
    }while(fabs(x - x0) >= epsilon);
    return x0;
}
