#pragma once

#ifdef MODEL_EXPORTS
#define MODEL_API __declspec(dllexport)
#else
#define MODEL_API __declspec(dllimport)
#endif
#include "../Metric/Metric.h"
#include <string>
#include <vector>
#include <map>
using namespace std;



class MODEL_API Model {
private:

    string name;
    string modeling_class;
    string next_model_name = "";
    map <string, Model*> components;
    vector <Metric*> metrics;
    map <string, double> parametrs;
    map <string, vector<double>> parallel_parametrs;
    map <string, string> components_models_adresses;
    

public:

    Model();
    
    explicit Model(const std::string& model_name, const std::string& model_class);
    
    virtual ~Model() = default;
    
    virtual double modeling_way() = 0;
    
    virtual double collecting_metrics() = 0;
    
    virtual double demonstrate_model() = 0;
    
    void print_name();
    
    void print_class();
    
    void add_component(string model_name, Model* model);
    
    Model* get_component(string model_name);
    
    void add_metric(Metric* metric);
    
    Metric* get_metric(int metric_nomer);
    
    void set_parametr(string parametr_name, double parametr_value);
    
    double get_parametr(string parametr_name);
    
    void set_parallel_parametr(string parametr_name, vector<double> parametr_value);
    
    vector<double>* get_parallel_parametr(string parametr_name);

    map <string, vector<double>> get_all_parallel_parametrs();

    void add_component_model_adress(string name_model, string model_adress);

    string get_component_model_adress(string name_model);
    
    void metrics_output();
    
    int parametrs_size();

    int parallel_parametrs_size();

    int components_size();

    int metrics_size();

    int all_components_metrics_size();

    string get_name();

    string get_class();

    vector <Metric*> get_all_components_metrics();

    void add_next_model_name(string next_model);

    string get_next_model_name();

    map<string, double> get_all_parametrs();
    
    void clear_metrics();
};
