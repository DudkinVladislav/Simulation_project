#pragma once

#ifdef AVERAGE_MESSAGES_EXPORTS
#define AVERAGE_MESSAGES_API __declspec(dllexport)
#else
#define AVERAGE_MESSAGES_API __declspec(dllimport)
#endif

#include "../Metric/Metric.h"
#include <string>
#include <vector>
#include <map>

class AVERAGE_MESSAGES_API Average_number_transfered_messages_for_one_time : public Metric {


public:

    Average_number_transfered_messages_for_one_time();
    explicit Average_number_transfered_messages_for_one_time(const std::string& metric_name, const std::string& metric_type);
    ~Average_number_transfered_messages_for_one_time() override;

    virtual double calculate_metric() override;

    virtual void set_value(double val) override;

    virtual double demonstrate_metric() override;

};

// C-интерфейс для динамической загрузки
#ifdef __cplusplus
extern "C" {
#endif

    // Основные функции
    __declspec(dllexport) void* CreateMetric(const char* name, const char* type);
    __declspec(dllexport) void DestroyMetric(void* metric);

    // Методы из Metric
    __declspec(dllexport) double CalculateMetric(void* metric);
    __declspec(dllexport) void SetValue(void* metric, double val);
    __declspec(dllexport) double GetValue(void* metric);
    __declspec(dllexport) void AddParametr(void* metric, const char* param_name, double param_value);
    __declspec(dllexport) double GetParametr(void* metric, const char* param_name);
    __declspec(dllexport) void AddMainParametr(void* metric, string main_param_name, double main_param_value);
    __declspec(dllexport) double GetMainCurrentParametr(void* metric, string name);
    __declspec(dllexport) void PrintName(void* metric);
    __declspec(dllexport) void PrintType(void* metric);
    __declspec(dllexport) const char* GetName(void* metric);
    __declspec(dllexport) void DemonstrateMetric(void* metric);

#ifdef __cplusplus
}
#endif
#pragma once
