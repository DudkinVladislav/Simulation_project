
#include "pch.h"
#define NODE_EXPORTS
#define _USE_MATH_DEFINES
#include "Node.h"
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
#pragma comment(lib, "../x64/Debug/Movement.lib")
using namespace std;

// Конструкторы
Node::Node()
    : Model("Node_1", "Node_model") {
}


Node::Node(const std::string& model_name, const std::string& model_class)
    : Model(model_name, model_class) {
}

// Деструктор
Node::~Node() {

}

double Node::modeling_way() {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> angle(0, get_parametr("max_angle"));
    uniform_real_distribution<double> speed(0, get_parametr("max_speed"));
    uniform_real_distribution<double> time_move(0, get_parametr("max_time_move") - 0.1);
    set_parametr("last_time", get_parametr("time"));
    Movement move("Movement", "movement_model");
    if (components_size() == 0)
    {
        move.set_parametr("speed", get_parametr("speed"));
        move.set_parametr("angle", get_parametr("angle"));
        move.set_parametr("dif_time", get_parametr("dif_time"));
        add_component("Movement", &move);
    }
    else
    {
        (*get_component("Movement")).set_parametr("speed", get_parametr("speed"));
        (*get_component("Movement")).set_parametr("angle", get_parametr("angle"));
        (*get_component("Movement")).set_parametr("dif_time", get_parametr("dif_time"));
    }
    (*get_component("Movement")).modeling_way();
    set_parametr("posx", get_parametr("posx") + (*get_component("Movement")).get_parametr("plusx"));
    set_parametr("posy", get_parametr("posy") + (*get_component("Movement")).get_parametr("plusy"));
    set_parametr("angle", int(angle(gen)));
    set_parametr("speed", speed(gen));
    double time_next_stop = time_move(gen) + 0.1;
    double new_time = get_parametr("time") + time_next_stop;
    set_parametr("time", new_time);
    return 0;
}

double Node::collecting_metrics() {
    return 0;
}

double Node::demonstrate_model() {
    cout << "\n=== Демонстрация работы с моделью узла ===\n";

    // Печатаем имя
    cout << "Имя модели (print_name): ";
    print_name();
    cout << "Класс модели (print_type): ";
    print_class();

    // Устанавливаем значение через set_value
    double posx_demo, posy_demo, time_demo, speed_demo, angle_demo, dif_time_demo;
    
    cout << "Введите значение параметра положения узла по оси x:";
    cin >> posx_demo;
    set_parametr("posx", posx_demo);

    cout << "Введите значение параметра положения узла по оси y:";
    cin >> posy_demo;
    set_parametr("posy", posy_demo);

    cout << "Введите значение параметра времени начала движения:";
    cin >> time_demo;
    set_parametr("time", time_demo);

    cout << "Введите значение параметра скорости движения:";
    cin >> speed_demo;
    set_parametr("speed", speed_demo);

    cout << "Введите значение параметра угла поворота:";
    cin >> angle_demo;
    set_parametr("angle", angle_demo);

    cout << "Введите значение параметра времени движения:";
    cin >> dif_time_demo;
    set_parametr("dif_time", dif_time_demo);


    double max_angle_demo, max_speed_demo, max_time_move_demo;
    cout << "Введите значение параметра максимального угла поворота:";
    cin >> max_angle_demo;
    set_parametr("max_angle", max_angle_demo);

    cout << "Введите значение параметра максимальной скорости:";
    cin >> max_speed_demo;
    set_parametr("max_speed", max_speed_demo);
    
    cout << "Введите значение параметра максимального времени непрерывного движения:";
    cin >> max_time_move_demo;
    set_parametr("max_time_move", max_time_move_demo);
    
    
    // Моделируем
    modeling_way();
    //Выводим новые параметры
    cout << "Новые значения параметров:" << endl;
    cout << "положение узла по оси x=" << get_parametr("posx") << endl;
    cout << "положение узла по оси y=" << get_parametr("posy") << endl;
    cout << "время начала движения=" << get_parametr("time") << endl;
    cout << "скорость движения узла=" << get_parametr("speed") << endl;
    cout << "угол поворота=" << get_parametr("angle") << endl;

    return 0;
}

extern "C" {

    __declspec(dllexport) void* CreateModel(const char* model_name, const char* model_class) {
        try {
            if (model_name && strlen(model_name) > 0 && model_class && strlen(model_class) > 0) {
                return new Node(model_name, model_class);
            }
            else {
                return new Node();
            }
        }
        catch (...) {
            return nullptr;
        }
    }

    __declspec(dllexport) void DestroyModel(void* model) {
        if (model) {
            delete static_cast<Node*>(model);
        }
    }

    __declspec(dllexport) double ModelingWay(void* model) {
        if (model) {
            return static_cast<Node*>(model)->modeling_way();
        }
        return 0.0;
    }

    __declspec(dllexport) double CollectingMetric(void* model) {
        if (model) {
            return static_cast<Node*>(model)->collecting_metrics();
        }
        return 0.0;
    }

    __declspec(dllexport) string GetNextModelName(void* model) {
        if (model) {
            return static_cast<Node*>(model)->get_next_model_name();
        }
        else
            return "";
    }

    __declspec(dllexport) void AddComponent(void* model, const char* model_name, Model* current_model) {
        if (model) {
            static_cast<Node*>(model)->add_component(model_name, current_model);
        }
    }

    __declspec(dllexport) Model* GetComponent(void* model, const char* model_name) {
        if (model) {
            return static_cast<Node*>(model)->get_component(model_name);
        }
        else
            return nullptr;
    }

    __declspec(dllexport) int ComponentsSize(void* model) {
        if (model) {
            return static_cast<Node*>(model)->components_size();
        }
        return 0;
    }

    __declspec(dllexport) void AddMetric(void* model, Metric* metric) {
        if (model) {
            static_cast<Node*>(model)->add_metric(metric);
        }
    }

    __declspec(dllexport) Metric* GetMetric(void* model, int metric_nomer) {
        if (model) {
            return static_cast<Node*>(model)->get_metric(metric_nomer);
        }
            return nullptr;
    }

    __declspec(dllexport) void AddComponentModelAdress(void* model, const char* model_name, string model_adress) {
        if (model) {
            static_cast<Node*>(model)->add_component_model_adress(model_name, model_adress);
        }
    }

    __declspec(dllexport) string GetComponentModelAdress(void* model, const char* model_name) {
        if (model) {
            return static_cast<Node*>(model)->get_component_model_adress(model_name);
        }
        else
            return "";
    }

    __declspec(dllexport) int MetricsSize(void* model) {
        if (model) {
            return static_cast<Node*>(model)->metrics_size();
        }
        return 0;
    }

    __declspec(dllexport) int AllComponentsMetricsSize(void* model) {
        if (model) {
            return static_cast<Node*>(model)->all_components_metrics_size();
        }
        return 0;
    }

    __declspec(dllexport) void SetParametr(void* model, const char* param_name, double param_value) {
        if (model && param_name && param_value) {
            static_cast<Node*>(model)->set_parametr(param_name, param_value);
        }
    }

    __declspec(dllexport) double GetParametr(void* model, const char* param_name) {
        if (model && param_name) {
            return static_cast<Node*>(model)->get_parametr(param_name);
        }
        return 0.0;
    }

    __declspec(dllexport) void SetParallelParametr(void* model, const char* param_name, vector<double> param_value) {
        if (model && param_name && param_value.size()==3) {
            static_cast<Node*>(model)->set_parallel_parametr(param_name, param_value);
        }
    }

    __declspec(dllexport) vector<double>* GetParallelParametr(void* model, const char* param_name) {
        if (model && param_name) {
            return static_cast<Node*>(model)->get_parallel_parametr(param_name);
        }
        else {
            vector<double>* empty=nullptr;
            return empty;
        }
    }

    __declspec(dllexport) int ParallelParametrsSize(void* model) {
        if (model) {
            return  static_cast<Node*>(model)->parallel_parametrs_size();
        }
        return 0;
    }

    __declspec(dllexport) map <string, vector<double>> GetAllParallelParametrs(void* model) {
        if (model) {
            return static_cast<Node*>(model)->get_all_parallel_parametrs();
        }
        else {
            map <string, vector<double>> empty;
            return empty;
        }
    }

    __declspec(dllexport) void PrintName(void* model) {
        if (model) {
            static_cast<Node*>(model)->print_name();
        }
    }

    __declspec(dllexport) void PrintClass(void* model) {
        if (model) {
            static_cast<Node*>(model)->print_class();
        }
    }

    __declspec(dllexport) const char* GetName(void* model) {
        if (model) {
            static std::string name;
            name = static_cast<Node*>(model)->get_name();
            return name.c_str();
        }
        return "";
    }

    __declspec(dllexport) const char* GetClass(void* model) {
        if (model) {
            static std::string classs;
            classs = static_cast<Node*>(model)->get_class();
            return classs.c_str();
        }
        return "";
    }

    __declspec(dllexport) void MetricsOutput(void* model) {
        if (model) {
            static_cast<Node*>(model)->metrics_output();
        }
    }

    __declspec(dllexport) void DemonstrateModel(void* model) {
        if (model) {
            static_cast<Node*>(model)->demonstrate_model();
        }
    }

    __declspec(dllexport) vector <Metric*> GetAllComponentsMetrics(void* model) {
        if (model) {
            return static_cast<Node*>(model)->get_all_components_metrics();
        }
        else {
            vector <Metric*> empty;
            empty.resize(0);
            return empty;
        }
    }

} // extern "C"