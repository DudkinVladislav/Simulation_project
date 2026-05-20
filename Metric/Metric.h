#pragma once

#ifdef METRIC_EXPORTS
#define METRIC_API __declspec(dllexport)
#else
#define METRIC_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <map>
using namespace std;



class METRIC_API Metric {
private:
    map <string, double> main_parametrs;
    map <string, double> parametrs;
    map <string, double> fixed_main_parametrs;
public:
    string name;
    string type;
    double value = 0;
    virtual ~Metric() = default;
    virtual double calculate_metric() = 0;
    virtual void set_value(double val) = 0;
    virtual double demonstrate_metric() = 0;
    double get_value();
    void add_parametr(string parametr_name, double parametr_value);
    double get_parametr(string name);
    void add_main_parametr(string main_parametr_name, double main_parametr_value);
    double get_main_parametr(string name);
    void add_fixed_main_parametr(string fixed_main_parametr_name, double fixed_main_parametr_value);
    double get_fixed_main_parametr(string name);
    int fixed_main_parametrs_size();
    void print_name();
    void print_type();
    int main_parametrs_size();
    int parametrs_size();
    string get_name();
    string get_type();
    void metric_output();
    map <string, double> get_all_main_parametrs();
};
