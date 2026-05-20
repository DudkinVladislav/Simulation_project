#define MODEL_EXPORTS_EXPORTS
#include "pch.h"
#include "Model.h"
#include <iostream>
#include <map>
#include <string>
#pragma comment(lib, "../x64/Debug/Metric.lib")
using namespace std;

Model::Model() {
    name = "Model";
    modeling_class = "Model";
}

Model::Model(const string& model_name, const std::string& model_class) {
    name = model_name;
    modeling_class = model_class;
}

void Model::print_name() {
    cout << name << endl;
}

void Model::print_class() {
    cout << modeling_class << endl;
}

void Model::add_component(string model_name, Model* model) {
    components[model_name] = model;
}

Model* Model::get_component(string model_name) {
    return components[model_name];
}

int Model::components_size() {
    return components.size();
}

void Model::add_metric(Metric* metric) {
    metrics.push_back(metric);
}

Metric* Model::get_metric(int metric_nomer) {
    return metrics[metric_nomer];
}

int Model::metrics_size() {
    return metrics.size();
}

void Model::set_parametr(string parametr_name, double parametr_value) {
    parametrs[parametr_name] = parametr_value;
}

int Model::parametrs_size() {
    return parametrs.size();
}

double Model::get_parametr(string parametr_name) {
    return parametrs[parametr_name];
}

void Model::set_parallel_parametr(string parametr_name, vector<double> parametr_value) {
    parallel_parametrs[parametr_name] = parametr_value;
}

vector<double>* Model::get_parallel_parametr(string parametr_name) {
    return &parallel_parametrs[parametr_name];
}

map <string, vector<double>> Model::get_all_parallel_parametrs() {
    return parallel_parametrs;
}

void Model::add_component_model_adress(string name_model, string model_adress) {
    components_models_adresses[name_model] = model_adress;
}

string Model::get_component_model_adress(string name_model) {
    return components_models_adresses[name_model];
}

string Model::get_name() {
    return name;
}

string Model::get_class() {
    return modeling_class;
}

int Model::parallel_parametrs_size() {
    return parallel_parametrs.size();
}

int Model::all_components_metrics_size() {
    int size = metrics.size();
    if (components.size() > 0)
    {
        for (std::map<std::string, Model*>::iterator it = (components).begin(); it != (components).end(); ++it)
        {
            size += (*get_component(it->first)).all_components_metrics_size();
        }
    }
    return size;
}

void Model::metrics_output() {
    if (components.size() > 0)
    {
        for (std::map<std::string, Model*>::iterator it = components.begin(); it != components.end(); ++it)
        {
            (*get_component(it->first)).metrics_output();
        }
    }
    for (int metric_index = 0; metric_index < metrics.size(); metric_index++)
    {
        (*get_metric(metric_index)).metric_output();
    }
}

vector <Metric*> Model::get_all_components_metrics() {
    vector <Metric*> all_metrics;
    all_metrics.insert(all_metrics.end(), metrics.begin(), metrics.end());
    if (components.size() > 0)
    {
        for (std::map<std::string, Model*>::iterator it = components.begin(); it != components.end(); ++it)
        {
           vector <Metric*> all_metrics_component = (*get_component(it->first)).get_all_components_metrics();
           all_metrics.insert(all_metrics.end(), all_metrics_component.begin(), all_metrics_component.end());
        }
    }
    return all_metrics;
}

void Model::add_next_model_name(string next_model) {
    next_model_name = next_model;
}

string Model::get_next_model_name() {
    return next_model_name;
}

map<string, double> Model::get_all_parametrs() {
    return parametrs;
}

void Model::clear_metrics() {
    metrics.resize(0);
}
