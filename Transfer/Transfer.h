#pragma once

#ifdef TRANSFER_EXPORTS
#define TRANSFER_API __declspec(dllexport)
#else
#define TRANSFER_API __declspec(dllimport)
#endif

#include "../Model/Model.h"
#include "../Metric/Metric.h"
#include <string>
#include <vector>
#include <map>

class TRANSFER_API Transfer : public Model {


public:

    Transfer();
    explicit Transfer(const std::string& model_name, const std::string& model_class);
    ~Transfer() override;

    virtual double modeling_way() override;

    virtual double collecting_metrics() override;

    virtual double demonstrate_model() override;

};

// C-интерфейс для динамической загрузки
#ifdef __cplusplus
extern "C" {
#endif

    // Основные функции
    __declspec(dllexport) void* CreateModel(const char* name, const char* clas);
    __declspec(dllexport) void DestroyModel(void* model);

    // Методы из Model
    __declspec(dllexport) double ModelingWay(void* model);
    __declspec(dllexport) double CollectingMetric(void* model);
    __declspec(dllexport) void SetParametr(void* model, const char* param_name, double param_value);
    __declspec(dllexport) double GetParametr(void* model, const char* param_name);
    __declspec(dllexport) int ParallelParametrsSize(void* model);
    __declspec(dllexport) void SetParallelParametr(void* model, const char* param_name, vector<double> param_value);
    __declspec(dllexport) vector<double>* GetParallelParametr(void* model, const char* param_name);
    __declspec(dllexport) void AddComponent(void* model, const char* model_name, Model* current_model);
    __declspec(dllexport) Model* GetComponent(void* model, const char* model_name);
    __declspec(dllexport) void AddMetric(void* model, Metric* metric);
    __declspec(dllexport) Metric* GetMetric(void* model, int nomer);
    __declspec(dllexport) void AddComponentModelAdress(void* model, const char* model_name, string model_adress);
    __declspec(dllexport) string GetComponentModelAdress(void* model, const char* model_name);
    __declspec(dllexport) int MetricsSize(void* model);
    __declspec(dllexport) int AllComponentsMetricsSize(void* model);
    __declspec(dllexport) void PrintName(void* model);
    __declspec(dllexport) void PrintClass(void* model);
    __declspec(dllexport) const char* GetName(void* model);
    __declspec(dllexport) void MetricsOutput(void* model);
    __declspec(dllexport) void DemonstrateModel(void* model);
    __declspec(dllexport) vector <Metric*> GetAllComponentsMetrics(void* model);
    __declspec(dllexport) string GetNextModelName(void* model);
    __declspec(dllexport) map <string, vector<double>> GetAllParallelParametrs(void* model);

#ifdef __cplusplus
}
#endif
