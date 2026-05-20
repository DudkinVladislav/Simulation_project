
#include "pch.h"
#define NETWORK_PARALLEL_EXPORTS
#define _USE_MATH_DEFINES
#include "Network_parallel.h"
#include <numeric>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <math.h>
#include <random>
#pragma comment(lib, "../x64/Debug/Model.lib")
#pragma comment(lib, "../x64/Debug/Area_parallel.lib")
using namespace std;

//  онструкторы
Network_parallel::Network_parallel()
    : Model("New_Network_parallel", "Network_parallel") {
}

Network_parallel::Network_parallel(const std::string& model_name)
    : Model(model_name, "Network_model") {
}

Network_parallel::Network_parallel(const std::string& model_name, const std::string& model_class)
    : Model(model_name, model_class) {
}

// ƒеструктор
Network_parallel::~Network_parallel() {

}

double Network_parallel::modeling_way() {
    random_device rd;
    mt19937 gen(rd());
    
    metrics.resize(0);
    cout << endl;
    cout << "¬ведите параметры модели сети:" << endl;
    cout << endl;

    string adress_area_model, area_model_name;
    cout << "¬ведите наименование модели области:";
    cin >> area_model_name;
    add_next_model_name(area_model_name);
    cout << "¬ведите адрес модели области:";
    cin >> adress_area_model;
    add_component_model_adress(area_model_name, adress_area_model);
    Area_parallel* ar = new Area_parallel(area_model_name, "area_model");
    double length, width, start_number, number, step_number, start_radius, max_radius, step_radius, time, simulation_number, max_time_move, max_speed, max_angle, fixed_number, fixed_radius;

    cout << "¬ведите длину области:";
    cin >> length;
    (*ar).set_parametr("length", length);

    cout << "¬ведите ширину области:";
    cin >> width;
    (*ar).set_parametr("width", width);

    cout << "¬ведите минимальное количество узлов сети:";
    cin >> start_number;
    (*ar).set_parametr("start_number", start_number);

    cout << "¬ведите максимальное количество узлов сети:";
    cin >> number;
    (*ar).set_parametr("max_number", number);

    cout << "¬ведите шаг изменени€ количества узлов сети:";
    cin >> step_number;
    (*ar).set_parametr("step_number", step_number);

    vector <double> numbers = { start_number, step_number, number }; //пор€док об€зателен

    set_parallel_parametr("number", numbers);

    cout << "¬ведите фиксированное значение количества узлов сети дл€ графика:";
    cin >> fixed_number;
    (*ar).set_parametr("fixed_number", fixed_number);

    cout << "¬ведите минимальный радиус узла:";
    cin >> start_radius;
    (*ar).set_parametr("start_radius", start_radius);

    cout << "¬ведите максимальный радиус узла:";
    cin >> max_radius;
    (*ar).set_parametr("max_radius", max_radius);

    cout << "¬ведите шаг изменени€ радиуса узла:";
    cin >> step_radius;
    (*ar).set_parametr("step_radius", step_radius);

    vector <double> radius = { start_radius, step_radius, max_radius }; //пор€док об€зателен

    set_parallel_parametr("radius", radius);

    cout << "¬ведите фиксированное значение радиуса узла дл€ графика:";
    cin >> fixed_radius;
    (*ar).set_parametr("fixed_radius", fixed_radius);

    cout << "¬ведите врем€ симул€ции:";
    cin >> time;
    (*ar).set_parametr("time", time);

    cout << "¬ведите количество симул€ций:";
    cin >> simulation_number;
    (*ar).set_parametr("simulation_number", simulation_number);

    cout << "¬ведите максимальное врем€ непрерывного движени€ узла:";
    cin >> max_time_move;
    (*ar).set_parametr("max_time_move", max_time_move);

    cout << "¬ведите максимальную скорость узла:";
    cin >> max_speed;
    (*ar).set_parametr("max_speed", max_speed);

    cout << "¬ведите максимальный угол поворота узла:";
    cin >> max_angle;
    (*ar).set_parametr("max_angle", max_angle);

    add_component(ar->get_name(), ar);

    //(*get_component(ar->get_name())).modeling_way();

    return 2;
}

double Network_parallel::collecting_metrics() {
    return 0;
}

double Network_parallel::demonstrate_model() {
    cout << "\n=== ƒемонстраци€ работы с моделью сети ===\n";

    // ѕечатаем им€
    cout << "»м€ модели (print_name): ";
    print_name();
    cout << " ласс модели (print_type): ";
    print_class();

    modeling_way();
    metrics_output();
    return 0;
}

extern "C" {

    __declspec(dllexport) void* CreateModel(const char* model_name, const char* model_class) {
        try {
            if (model_name && strlen(model_name) > 0 && model_class && strlen(model_class) > 0) {
                return new Network_parallel(model_name, model_class);
            }
            else {
                return new Network_parallel();
            }
        }
        catch (...) {
            return nullptr;
        }
    }

    __declspec(dllexport) void DestroyModel(void* model) {
        if (model) {
            delete static_cast<Network_parallel*>(model);
        }
    }

    __declspec(dllexport) double ModelingWay(void* model) {
        if (model) {
            return static_cast<Network_parallel*>(model)->modeling_way();
        }
        return 0.0;
    }

    __declspec(dllexport) double CollectingMetric(void* model) {
        if (model) {
            return static_cast<Network_parallel*>(model)->collecting_metrics();
        }
        return 0.0;
    }

    __declspec(dllexport) void AddComponent(void* model, const char* model_name, Model* current_model) {
        if (model) {
            static_cast<Network_parallel*>(model)->add_component(model_name, current_model);
        }
    }

    __declspec(dllexport) Model* GetComponent(void* model, const char* model_name) {
        if (model) {
            return static_cast<Network_parallel*>(model)->get_component(model_name);
        }
        else
            return nullptr;
    }

    __declspec(dllexport) int ComponentsSize(void* model) {
        if (model) {
            return static_cast<Network_parallel*>(model)->components_size();
        }
        return 0;
    }

    __declspec(dllexport) void AddMetric(void* model, Metric* metric) {
        if (model) {
            static_cast<Network_parallel*>(model)->add_metric(metric);
        }
    }

    __declspec(dllexport) Metric* GetMetric(void* model, int metric_nomer) {
        if (model) {
            return static_cast<Network_parallel*>(model)->get_metric(metric_nomer);
        }
        else
            return nullptr;
    }

    __declspec(dllexport) void AddComponentModelAdress(void* model, const char* model_name, string model_adress) {
        if (model) {
            static_cast<Network_parallel*>(model)->add_component_model_adress(model_name, model_adress);
        }
    }

    __declspec(dllexport) string GetComponentModelAdress(void* model, const char* model_name) {
        if (model) {
            return static_cast<Network_parallel*>(model)->get_component_model_adress(model_name);
        }
        else
            return "";
    }

    __declspec(dllexport) string GetNextModelName(void* model) {
        if (model) {
            return static_cast<Network_parallel*>(model)->get_next_model_name();
        }
        else
            return "";
    }

    __declspec(dllexport) int MetricsSize(void* model) {
        if (model) {
            return static_cast<Network_parallel*>(model)->metrics_size();
        }
        return 0;
    }

    __declspec(dllexport) int AllComponentsMetricsSize(void* model) {
        if (model) {
            return static_cast<Network_parallel*>(model)->all_components_metrics_size();
        }
        return 0;
    }

    __declspec(dllexport) void SetParametr(void* model, const char* param_name, double param_value) {
        if (model && param_name && param_value) {
            static_cast<Network_parallel*>(model)->set_parametr(param_name, param_value);
        }
    }

    __declspec(dllexport) double GetParametr(void* model, const char* param_name) {
        if (model && param_name) {
            return static_cast<Network_parallel*>(model)->get_parametr(param_name);
        }
        return 0.0;
    }

    __declspec(dllexport) void SetParallelParametr(void* model, const char* param_name, vector<double> param_value) {
        if (model && param_name && param_value.size() == 3) {
            static_cast<Network_parallel*>(model)->set_parallel_parametr(param_name, param_value);
        }
    }

    __declspec(dllexport) vector<double>* GetParallelParametr(void* model, const char* param_name) {
        if (model && param_name) {
            return static_cast<Network_parallel*>(model)->get_parallel_parametr(param_name);
        }
        else {
            vector<double>* empty=nullptr;
            return empty;
        }
    }

    __declspec(dllexport) int ParallelParametrsSize(void* model) {
        if (model) {
            return  static_cast<Network_parallel*>(model)->parallel_parametrs_size();
        }
        return 0;
    }

    __declspec(dllexport) void PrintName(void* model) {
        if (model) {
            static_cast<Network_parallel*>(model)->print_name();
        }
    }

    __declspec(dllexport) void PrintClass(void* model) {
        if (model) {
            static_cast<Network_parallel*>(model)->print_class();
        }
    }

    __declspec(dllexport) const char* GetName(void* model) {
        if (model) {
            static std::string name;
            name = static_cast<Network_parallel*>(model)->get_name();
            return name.c_str();
        }
        return "";
    }

    __declspec(dllexport) void MetricsOutput(void* model) {
        if (model) {
            static_cast<Network_parallel*>(model)->metrics_output();
        }
    }

    __declspec(dllexport) void DemonstrateModel(void* model) {
        if (model) {
            static_cast<Network_parallel*>(model)->demonstrate_model();
        }
    }

    __declspec(dllexport) vector <Metric*> GetAllComponentsMetrics(void* model) {
        if (model) {
            return static_cast<Network_parallel*>(model)->get_all_components_metrics();
        }
        else {
            vector <Metric*> empty;
            empty.resize(0);
            return empty;
        }
    }

    __declspec(dllexport) map <string, double> GetAllParametrs(void* model) {
        if (model) {
            return static_cast<Network_parallel*>(model)->get_all_parametrs();
        }
        else {
            map <string, double> empty;
            return empty;
        }
    }

    __declspec(dllexport) map <string, vector<double>> GetAllParallelParametrs(void* model) {
        if (model) {
            return static_cast<Network_parallel*>(model)->get_all_parallel_parametrs();
        }
        else {
            map <string, vector<double>> empty;
            return empty;
        }
    }

} // extern "C"