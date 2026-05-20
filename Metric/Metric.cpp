#define METRIC_EXPORTS
#include "pch.h"
#include "Metric.h"
#include <iostream>

using namespace std;

double Metric::get_value() {
    return value;
}

void Metric::print_name() {
    cout << name;
}

void Metric::print_type() {
    cout << type;
}

string Metric::get_name() {
    return name;
}

string Metric::get_type() {
    return type;
}

void Metric::add_parametr(string parametr_name, double parametr_value) {
    parametrs[parametr_name] = parametr_value;
}

double Metric::get_parametr(string parametr_name) {
    return parametrs[parametr_name];
}
void Metric::add_main_parametr(string main_parametr_name, double main_parametr_value) {
    main_parametrs[main_parametr_name] = main_parametr_value;
}

double Metric::get_main_parametr(string name) {
    return main_parametrs[name];
}

void Metric::add_fixed_main_parametr(string fixed_main_parametr_name, double fixed_main_parametr_value) {
    fixed_main_parametrs[fixed_main_parametr_name] = fixed_main_parametr_value;
}

double Metric::get_fixed_main_parametr(string name) {
    return fixed_main_parametrs[name];
}

int Metric::fixed_main_parametrs_size() {
    return fixed_main_parametrs.size();
}

int Metric::main_parametrs_size() {
    return main_parametrs.size();
}

int Metric::parametrs_size() {
    return parametrs.size();
}

void Metric::metric_output() {
    for (std::map<std::string, double>::iterator it = main_parametrs.begin(); it != main_parametrs.end(); ++it)
    {
        cout << "Параметр '" << it->first << "' = " << get_main_parametr(it->first) << endl;
    }
    cout << "Метрика '";
    print_name();
    cout << "'=";
    cout << calculate_metric() << endl;
}

map <string, double> Metric::get_all_main_parametrs() {
    return main_parametrs;
}
