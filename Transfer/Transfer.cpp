
#include "pch.h"
#define Transfer_EXPORTS
#define _USE_MATH_DEFINES
#include "Transfer.h"
#include "../Average_number_transfered_messages/Average_number_transfered_messages.h"
#include "../Average_number_transfered_messages_for_one_time/Average_number_transfered_messages_for_one_time.h"
#include <numeric>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <math.h>
#pragma comment(lib, "../x64/Debug/Metric.lib")
#pragma comment(lib, "../x64/Debug/Model.lib")
#pragma comment(lib, "../x64/Debug/Average_number_transfered_messages.lib")
#pragma comment(lib, "../x64/Debug/Average_number_transfered_messages_for_one_time.lib")
using namespace std;

// Конструкторы
Transfer::Transfer()
    : Model("Transfer_1", "Transfer_model") {
}

Transfer::Transfer(const std::string& model_name, const std::string& model_class)
    : Model(model_name, model_class) {
}

// Деструктор
Transfer::~Transfer() {

}

double Transfer::collecting_metrics() {
    double value = (*get_metric(1)).get_value();
    (*get_metric(1)).set_value(value + int(get_parametr("time_max") - get_parametr("time_start")));
    value = (*get_metric(0)).get_value();
    (*get_metric(0)).set_value(value + 1);
    value = (*get_metric(1)).get_parametr("number_transfer");
    (*get_metric(1)).add_parametr("number_transfer", value + 1);
    return 0;
}

double Transfer::modeling_way() {
    double delx = get_parametr("posx1") - get_parametr("posx2");
    double dely = get_parametr("posy1") - get_parametr("posy2");
    double u = get_parametr("speed1") * cos(get_parametr("angle1") * M_PI / 180.0) - get_parametr("speed2") * cos(get_parametr("angle2") * M_PI / 180.0);
    double v = get_parametr("speed1") * sin(get_parametr("angle1") * M_PI / 180.0) - get_parametr("speed2") * sin(get_parametr("angle2") * M_PI / 180.0);
    double a = u * u + v * v;
    double b = 2 * (delx * u + dely * v);
    double c = delx * delx + dely * dely - get_parametr("radius1") * get_parametr("radius1");
    if (a == 0)
    {
        if (c <= 0)
        {
            set_parametr("time_max", get_parametr("time_m"));
            set_parametr("time_start", get_parametr("start_time"));
            collecting_metrics();
        }
    }
    if (a > 0)
    {

        double d = b * b - 4.0 * a * c;
        if (d >= 0)
        {
            double t1 = (-b - sqrt(d)) / (2 * a);
            double t2 = (-b + sqrt(d)) / (2 * a);
            if (t1 < get_parametr("start_time"))
            {
                if (t2 > get_parametr("time_m"))
                {
                    set_parametr("time_max", get_parametr("time_m"));
                    set_parametr("time_start", get_parametr("start_time"));
                    collecting_metrics();
                }
                if (t2 > get_parametr("start_time") && t2 < get_parametr("time_m"))
                {
                    set_parametr("time_max", t2);
                    set_parametr("time_start", get_parametr("start_time"));
                    collecting_metrics();
                }
            }
            else
            {
                if (t1 < get_parametr("time_m"))
                {
                    if (t2 > get_parametr("time_m"))
                    {
                        set_parametr("time_max", get_parametr("time_m"));
                        set_parametr("time_start", t1);
                        collecting_metrics();

                    }
                    if (t2 > get_parametr("start_time") && t2 < get_parametr("time_m"))
                    {
                        set_parametr("time_max", t2);
                        set_parametr("time_start", t1);
                        collecting_metrics();
                    }
                }
            }
        }
    }
    return 0;
}

double Transfer::demonstrate_model() {

    cout << "\n=== Демонстрация работы с моделью передачи данных ===\n";

    // Печатаем имя
    cout << "Имя модели (print_name): ";
    print_name();
    cout << "Класс модели (print_type): ";
    print_class();

    double posx1_demo, posy1_demo, posx2_demo, posy2_demo, speed1_demo, speed2_demo, angle1_demo, angle2_demo, radius_demo, time_m_demo, start_time_demo;
    
    cout << "Введите значение параметра положения первого узла по оси x:";
    cin >> posx1_demo ;
    set_parametr("posx1", posx1_demo);

    cout << "Введите значение параметра положения первого узла по оси y:";
    cin >> posy1_demo;
    set_parametr("posy1", posy1_demo);

    cout << "Введите значение параметра положения второго узла по оси x:"; 
    cin >> posx2_demo;
    set_parametr("posx2", posx2_demo);

    cout << "Введите значение параметра положения второго узла по оси y:";
    cin >> posy2_demo;
    set_parametr("posy2", posy2_demo);
    
    cout << "Введите значение параметра скорость движения первого узла:";
    cin >> speed1_demo;
    set_parametr("speed1", speed1_demo);

    cout << "Введите значение параметра угол поворота первого узла:";
    cin >> angle1_demo;
    set_parametr("angle1", angle1_demo);
    
    cout << "Введите значение параметра скорость движения второго узла:";
    cin >> speed2_demo;
    set_parametr("speed2", speed2_demo);

    cout << "Введите значение параметра угол поворота второго узла:";
    cin >> angle2_demo;
    set_parametr("angle2", angle2_demo);

    cout << "Введите значение параметра радиуса приёма/передачи узла:";
    cin >> radius_demo;
    set_parametr("radius1", radius_demo);
    set_parametr("radius2", radius_demo);

    cout << "Введите значение параметра времени начала движения:";
    cin >> start_time_demo;
    set_parametr("start_time", start_time_demo);

    cout << "Введите значение параметра времени окончания движения:";
    cin >> time_m_demo;
    set_parametr("time_m", time_m_demo);

    Average_number_transfered_messages* metric1 = new Average_number_transfered_messages("average_number_transfered_messages", "average_number_transfered_messages");
    Average_number_transfered_messages_for_one_time* metric2 = new Average_number_transfered_messages_for_one_time("average_number_transfered_messages_for_one_time", "average_number_transfered_messages_for_one_time");
    (*metric1).add_main_parametr("первый параметр", 1);
    (*metric1).add_main_parametr("второй параметр", 2);
    (*metric1).add_parametr("number_nodes", 1);
    (*metric2).add_main_parametr("первый параметр", 1);
    (*metric2).add_main_parametr("второй параметр", 2);
    (*metric2).add_parametr("number_transfer", 0);
    (*metric1).set_value(0);
    (*metric2).set_value(0);
    add_metric(metric1);
    add_metric(metric2);


    modeling_way();
    metrics_output();
    return 0;
}

extern "C" {

    __declspec(dllexport) void* CreateModel(const char* model_name, const char* model_class) {
        try {
            if (model_name && strlen(model_name) > 0 && model_class && strlen(model_class) > 0) {
                return new Transfer(model_name, model_class);
            }
            else {
                return new Transfer();
            }
        }
        catch (...) {
            return nullptr;
        }
    }

    __declspec(dllexport) void DestroyModel(void* model) {
        if (model) {
            delete static_cast<Transfer*>(model);
        }
    }

    __declspec(dllexport) double ModelingWay(void* model) {
        if (model) {
            return static_cast<Transfer*>(model)->modeling_way();
        }
        return 0.0;
    }

    __declspec(dllexport) double CollectingMetric(void* model) {
        if (model) {
            return static_cast<Transfer*>(model)->collecting_metrics();
        }
        return 0.0;
    }

    __declspec(dllexport) string GetNextModelName(void* model) {
        if (model) {
            return static_cast<Transfer*>(model)->get_next_model_name();
        }
        else
            return "";
    }

    __declspec(dllexport) void AddComponent(void* model, const char* model_name, Model* current_model) {
        if (model) {
            static_cast<Transfer*>(model)->add_component(model_name, current_model);
        }
    }

    __declspec(dllexport) Model* GetComponent(void* model, const char* model_name) {
        if (model) {
            return static_cast<Transfer*>(model)->get_component(model_name);
        }
        else
            return nullptr;
    }

    __declspec(dllexport) int ComponentsSize(void* model) {
        if (model) {
            return static_cast<Transfer*>(model)->components_size();
        }
        return 0;
    }

    __declspec(dllexport) void AddMetric(void* model, Metric* metric) {
        if (model) {
            static_cast<Transfer*>(model)->add_metric(metric);
        }
    }

    __declspec(dllexport) Metric* GetMetric(void* model, int metric_nomer) {
        if (model) {
            return static_cast<Transfer*>(model)->get_metric(metric_nomer);
        }
        else
            return nullptr;
    }

    __declspec(dllexport) void AddComponentModelAdress(void* model, const char* model_name, string model_adress) {
        if (model) {
            static_cast<Transfer*>(model)->add_component_model_adress(model_name, model_adress);
        }
    }

    __declspec(dllexport) string GetComponentModelAdress(void* model, const char* model_name) {
        if (model) {
            return static_cast<Transfer*>(model)->get_component_model_adress(model_name);
        }
        else
            return "";
    }

    __declspec(dllexport) int MetricsSize(void* model) {
        if (model) {
            return static_cast<Transfer*>(model)->metrics_size();
        }
        return 0;
    }

    __declspec(dllexport) int AllComponentsMetricsSize(void* model) {
        if (model) {
            return static_cast<Transfer*>(model)->all_components_metrics_size();
        }
        return 0;
    }

    __declspec(dllexport) void SetParametr(void* model, const char* param_name, double param_value) {
        if (model && param_name && param_value) {
            static_cast<Transfer*>(model)->set_parametr(param_name, param_value);
        }
    }

    __declspec(dllexport) double GetParametr(void* model, const char* param_name) {
        if (model && param_name) {
            return static_cast<Transfer*>(model)->get_parametr(param_name);
        }
        return 0.0;
    }

    __declspec(dllexport) void SetParallelParametr(void* model, const char* param_name, vector<double> param_value) {
        if (model && param_name && param_value.size() == 3) {
            static_cast<Transfer*>(model)->set_parallel_parametr(param_name, param_value);
        }
    }

    __declspec(dllexport) vector<double>* GetParallelParametr(void* model, const char* param_name) {
        if (model && param_name) {
            return static_cast<Transfer*>(model)->get_parallel_parametr(param_name);
        }
        else {
            vector<double>* empty=nullptr;
            return empty;
        }
    }

    __declspec(dllexport) int ParallelParametrsSize(void* model) {
        if (model) {
            return  static_cast<Transfer*>(model)->parallel_parametrs_size();
        }
        return 0;
    }

    __declspec(dllexport) void PrintName(void* model) {
        if (model) {
            static_cast<Transfer*>(model)->print_name();
        }
    }

    __declspec(dllexport) void PrintClass(void* model) {
        if (model) {
            static_cast<Transfer*>(model)->print_class();
        }
    }

    __declspec(dllexport) const char* GetName(void* model) {
        if (model) {
            static std::string name;
            name = static_cast<Transfer*>(model)->get_name();
            return name.c_str();
        }
        return "";
    }

    __declspec(dllexport) void MetricsOutput(void* model) {
        if (model) {
            static_cast<Transfer*>(model)->metrics_output();
        }
    }

    __declspec(dllexport) void DemonstrateModel(void* model) {
        if (model) {
            static_cast<Transfer*>(model)->demonstrate_model();
        }
    }

    __declspec(dllexport) vector <Metric*> GetAllComponentsMetrics(void* model) {
        if (model) {
            return static_cast<Transfer*>(model)->get_all_components_metrics();
        }
        else {
            vector <Metric*> empty;
            empty.resize(0);
            return empty;
        }
    }

    __declspec(dllexport) map <string, vector<double>> GetAllParallelParametrs(void* model) {
        if (model) {
            return static_cast<Transfer*>(model)->get_all_parallel_parametrs();
        }
        else {
            map <string, vector<double>> empty;
            return empty;
        }
    }

} // extern "C"