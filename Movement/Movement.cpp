
#include "pch.h"
#define MOVEMENT_EXPORTS
#define _USE_MATH_DEFINES
#include "Movement.h"
#include <numeric>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <math.h>
#pragma comment(lib, "../x64/Debug/Model.lib")
using namespace std;

// Конструкторы
Movement::Movement()
    : Model("Movement_1", "Movement_model") {
}

Movement::Movement(const std::string& model_name, const std::string& model_class)
    : Model(model_name, model_class) {
}

// Деструктор
Movement::~Movement() {

}

double Movement::demonstrate_model() {
    cout << "\n=== Демонстрация работы с моделью ===\n";

    // Печатаем имя
    cout << "Имя модели (print_name): ";
    print_name();
    cout << "Класс модели (print_type): ";
    print_class();


    // Устанавливаем значение через set_value
    double speed_demo, angle_demo, dif_time_demo;
        cout << "Введите значение параметра скорости:";
        cin >> speed_demo;

        cout << "Введите значение параметра угла поворота:";
        cin >> angle_demo;

        cout << "Введите значение параметра времени движения:";
        cin >> dif_time_demo;

    set_parametr("speed", speed_demo);
    set_parametr("angle", angle_demo);
    set_parametr("dif_time", dif_time_demo);

    // получаем значения параметров
    cout << "Значения параметров:" << endl;
    cout << "скорость(speed)=" << get_parametr("speed") << endl;
    cout << "угод поворота(angle)=" << get_parametr("angle") << endl;
    cout << "время движения(dif_time)=" << get_parametr("dif_time") << endl;

    // Моделируем
    modeling_way();
    //Выводим новые параметры
    cout << "Изменения положения узла:" << endl;
    cout << "изменение по оси x=" << get_parametr("plusx") << endl;
    cout << "изменение по оси y=" << get_parametr("plusy") << endl;
    // Вычисляем метрику
    
   
    // Меняем параметры и пересчитываем
    cout << "\nМеняем параметры на 20, 30 и 5:" << endl;
    set_parametr("speed", 20.0);
    set_parametr("angle", 30.0);
    set_parametr("dif_time", 5.0);
    // Моделируем
    modeling_way();
    //Выводим новые изменения положения
    cout << "Новые изменения положения узла:" << endl;
    cout << "изменение по оси x=" << get_parametr("plusx") << endl;
    cout << "изменение по оси y=" << get_parametr("plusy") << endl;

    return 0;
}

double Movement::modeling_way() {
    double value_x = get_parametr("speed") * cos(get_parametr("angle") * M_PI / 180.0) * get_parametr("dif_time");
    double value_y = get_parametr("speed") * sin(get_parametr("angle") * M_PI / 180.0) * get_parametr("dif_time");
    set_parametr("plusx", value_x);
    set_parametr("plusy", value_y);
    return 0;
}

double Movement::collecting_metrics() {
    return 0;
}

extern "C" {

    __declspec(dllexport) void* CreateModel(const char* model_name, const char* model_class) {
        try {
            if (model_name && strlen(model_name) > 0 && model_class && strlen(model_class) > 0) {
                return new Movement(model_name, model_class);
            }
            else {
                return new Movement();
            }
        }
        catch (...) {
            return nullptr;
        }
    }

    __declspec(dllexport) void DestroyModel(void* model) {
        if (model) {
            delete static_cast<Movement*>(model);
        }
    }

    __declspec(dllexport) double ModelingWay(void* model) {
        if (model) {
            return static_cast<Movement*>(model)->modeling_way();
        }
        return 0.0;
    }

    __declspec(dllexport) double CollectingMetric(void* model) {
        if (model) {
            return static_cast<Movement*>(model)->collecting_metrics();
        }
        return 0.0;
    }

    __declspec(dllexport) string GetNextModelName(void* model) {
        if (model) {
            return static_cast<Movement*>(model)->get_next_model_name();
        }
        else
            return "";
    }

    __declspec(dllexport) void AddComponent(void* model, const char* model_name, Model* current_model) {
        if (model) {
            static_cast<Movement*>(model)->add_component(model_name, current_model);
        }
    }

    __declspec(dllexport) Model* GetComponent(void* model, const char* model_name) {
        if (model) {
            return static_cast<Movement*>(model)->get_component(model_name);
        }
        else
            return nullptr;
    }

    __declspec(dllexport) int ComponentsSize(void* model) {
        if (model) {
            return  static_cast<Movement*>(model)->components_size();
        }
        return 0;
    }

    __declspec(dllexport) void AddMetric(void* model, Metric* metric) {
        if (model) {
            static_cast<Movement*>(model)->add_metric(metric);
        }
    }

    __declspec(dllexport) Metric* GetMetric(void* model, int metric_nomer) {
        if (model) {
            return static_cast<Movement*>(model)->get_metric(metric_nomer);
        }
        else
            return nullptr;
    }

    __declspec(dllexport) void AddComponentModelAdress(void* model, const char* model_name, string model_adress) {
        if (model) {
            static_cast<Movement*>(model)->add_component_model_adress(model_name, model_adress);
        }
    }

    __declspec(dllexport) string GetComponentModelAdress(void* model, const char* model_name) {
        if (model) {
            return static_cast<Movement*>(model)->get_component_model_adress(model_name);
        }
        else
            return "";
    }

    __declspec(dllexport) int MetricsSize(void* model) {
        if (model) {
            return  static_cast<Movement*>(model)->metrics_size();
        }
            return 0;
    }

    __declspec(dllexport) int ParallelParametrsSize(void* model) {
        if (model) {
            return  static_cast<Movement*>(model)->parallel_parametrs_size();
        }
        return 0;
    }

    __declspec(dllexport) int AllComponentsMetricsSize(void* model){
        if (model) {
            return  static_cast<Movement*>(model)->all_components_metrics_size();
        }
            return 0;
    }

    __declspec(dllexport) void SetParametr(void* model, const char* param_name, double param_value) {
        if (model && param_name && param_value) {
            static_cast<Movement*>(model)->set_parametr(param_name, param_value);
        }
    }

    __declspec(dllexport) double GetParametr(void* model, const char* param_name) {
        if (model && param_name) {
            return static_cast<Movement*>(model)->get_parametr(param_name);
        }
        return 0.0;
    }

    __declspec(dllexport) void SetParallelParametr(void* model, const char* param_name, vector<double> param_value) {
        if (model && param_name && param_value.size()==3) {
            static_cast<Movement*>(model)->set_parallel_parametr(param_name, param_value);
        }
    }

    __declspec(dllexport) vector<double>* GetParallelParametr(void* model, const char* param_name) {
        if (model && param_name) {
            return static_cast<Movement*>(model)->get_parallel_parametr(param_name);
        }
        else {
            vector<double>* empty=nullptr;
            empty->resize(0);
            return empty;
        }
        
    }

    __declspec(dllexport) map <string, vector<double>> GetAllParallelParametrs(void* model) {
        if (model) {
            return static_cast<Movement*>(model)->get_all_parallel_parametrs();
        }
        else {
            map <string, vector<double>> empty;
            return empty;
        }
    }

    __declspec(dllexport) void PrintName(void* model) {
        if (model) {
            static_cast<Movement*>(model)->print_name();
        }
    }

    __declspec(dllexport) void PrintClass(void* model) {
        if (model) {
            static_cast<Movement*>(model)->print_class();
        }
    }

    __declspec(dllexport) const char* GetName(void* model) {
        if (model) {
            static std::string name;
            name = static_cast<Movement*>(model)->get_name();
            return name.c_str();
        }
        return "";
    }

    __declspec(dllexport) void MetricsOutput(void* model) {
        if (model) {
            static_cast<Movement*>(model)->metrics_output();
        }
    }

    __declspec(dllexport) void DemonstrateModel(void* model) {
        if (model) {
            static_cast<Movement*>(model)->demonstrate_model();
        }
    }

    __declspec(dllexport) vector <Metric*> GetAllComponentsMetrics(void* model) {
        if (model) {
            return static_cast<Movement*>(model)->get_all_components_metrics();
        }
        else {
            vector <Metric*> empty;
            empty.resize(0);
            return empty;
        }
            
    }

} // extern "C"