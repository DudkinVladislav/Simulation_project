
#include "pch.h"
#define AVERAGE_MESSAGES_EXPORTS
#include "Average_number_transfered_messages_for_one_time.h"
#include <numeric>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#pragma comment(lib, "../x64/Debug/Metric.lib")
using namespace std;

// Конструкторы
Average_number_transfered_messages_for_one_time::Average_number_transfered_messages_for_one_time()
    : Metric() {
    name = "Average number transfered messages for one time";
    type = "Average number transfered messages for one time";
    add_parametr("number_transfer", 0.0);  // Значение по умолчанию
}

Average_number_transfered_messages_for_one_time::Average_number_transfered_messages_for_one_time(const std::string& metric_name, const std::string& metric_type)
    : Metric() {
    name = metric_name;
    type = metric_type;
    add_parametr("number_transfer", 0.0);  // Значение по умолчанию
}

double Average_number_transfered_messages_for_one_time::demonstrate_metric() {

    cout << "Демонстрация работы метрики Average_number_transfered_messages_for_one_time" << endl;

    double number_transfer_demo;
    cout << "Введите значение параметра 'Количество передач (number_transfer)':";
    cin >> number_transfer_demo;
    add_parametr("number_transfer", number_transfer_demo);

    double value_demo;
    cout << "Введите значение метрики:";
    cin >> value_demo;
    set_value(value_demo);

    cout << "Рассчитанное значение метрики=" << calculate_metric() << endl;

    return calculate_metric();
}

// Деструктор
Average_number_transfered_messages_for_one_time::~Average_number_transfered_messages_for_one_time() {

}

void Average_number_transfered_messages_for_one_time::set_value(double val) {
    value = val;
}

double Average_number_transfered_messages_for_one_time::calculate_metric() {
    double number_transfer = get_parametr("number_transfer");
    if (number_transfer == 0) {
        return 0.0;
    }
    return get_value() / number_transfer;
}


extern "C" {

    __declspec(dllexport) void* CreateMetric(const char* name, const char* type) {
        try {
            if (name && strlen(name) > 0 && type && strlen(type) > 0) {
                return new Average_number_transfered_messages_for_one_time(name, type);
            }
            else {
                return new Average_number_transfered_messages_for_one_time();
            }
        }
        catch (...) {
            return nullptr;
        }
    }

    __declspec(dllexport) void DestroyMetric(void* metric) {
        if (metric) {
            delete static_cast<Average_number_transfered_messages_for_one_time*>(metric);
        }
    }

    __declspec(dllexport) double CalculateMetric(void* metric) {
        if (metric) {
            return static_cast<Average_number_transfered_messages_for_one_time*>(metric)->calculate_metric();
        }
        return 0.0;
    }

    __declspec(dllexport) void SetValue(void* metric, double val) {
        if (metric) {
            static_cast<Average_number_transfered_messages_for_one_time*>(metric)->set_value(val);
        }
    }

    __declspec(dllexport) double GetValue(void* metric) {
        if (metric) {
            return static_cast<Average_number_transfered_messages_for_one_time*>(metric)->get_value();
        }
        return 0.0;
    }

    __declspec(dllexport) void AddParametr(void* metric, const char* param_name, double param_value) {
        if (metric && param_name) {
            static_cast<Average_number_transfered_messages_for_one_time*>(metric)->add_parametr(param_name, param_value);
        }
    }

    __declspec(dllexport) double GetParametr(void* metric, const char* param_name) {
        if (metric && param_name) {
            return static_cast<Average_number_transfered_messages_for_one_time*>(metric)->get_parametr(param_name);
        }
        return 0.0;
    }

    __declspec(dllexport) void AddMainParametr(void* metric, string main_param_name, double main_param_value) {
        if (metric) {
            static_cast<Average_number_transfered_messages_for_one_time*>(metric)->add_main_parametr(main_param_name, main_param_value);
        }
    }

    __declspec(dllexport) double GetMainCurrentParametr(void* metric, string name) {
        if (metric) {
            return static_cast<Average_number_transfered_messages_for_one_time*>(metric)->get_main_parametr(name);
        }
        return 0.0;
    }

    __declspec(dllexport) void PrintName(void* metric) {
        if (metric) {
            static_cast<Average_number_transfered_messages_for_one_time*>(metric)->print_name();
        }
    }

    __declspec(dllexport) void PrintType(void* metric) {
        if (metric) {
            static_cast<Average_number_transfered_messages_for_one_time*>(metric)->print_type();
        }
    }

    __declspec(dllexport) const char* GetName(void* metric) {
        if (metric) {
            static std::string name;
            name = static_cast<Average_number_transfered_messages_for_one_time*>(metric)->get_name();
            return name.c_str();
        }
        return "";
    }

    __declspec(dllexport) void DemonstrateMetric(void* metric) {
        if (metric) {
            static_cast<Average_number_transfered_messages_for_one_time*>(metric)->demonstrate_metric();
        }
    }

} // extern "C"