#define _USE_MATH_DEFINES
#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <ctime>
#include <random>
#include <fstream>
#include <memory>
#include <exception>
#include <chrono>
#include <Aspose.Cells.h>
#include <math.h>
#include <functional>
#include <chrono>
#include <cmath>
#include <stdexcept>
#include <cstddef>
#include <omp.h>
#include "Metric/Metric.h"
#include "Model/Model.h"
#pragma comment(lib, "./x64/Debug/Metric.lib")
#pragma comment(lib, "./x64/Debug/Model.lib")
//#include "Graph.h"

using namespace std;
using namespace Aspose::Cells;
using namespace Aspose::Cells::Charts;

// Функции метрик
typedef void* (*CreateMetricFunc)(const char*, const char*);
typedef void (*DestroyMetricFunc)(void*);
typedef double (*CalculateMetricFunc)(void*);
typedef void (*SetValueFunc)(void*, double);
typedef double (*GetValueFunc)(void*);
typedef void (*AddParametrFunc)(void*, const char*, double);
typedef double (*GetParametrFunc)(void*, const char*);
typedef void (*AddMainParametrFunc)(void*, double);
typedef double (*GetMainParametrFunc)(void*, int);
typedef void (*PrintNameFunc)(void*);
typedef void (*PrintTypeFunc)(void*);
typedef const char* (*GetNameFunc)(void*);
typedef double (*DemonstrateMetricFunc)(void*);

// Функции моделей
typedef void* (*CreateModelFunc)(const char*, const char*);
typedef void (*DestroyModelFunc)(void*);
typedef double (*ModelingWayFunc)(void*);
typedef double (*CollectingMetricFunc)(void*);
typedef void (*SetParametrFunc)(void*, const char*, double);
typedef double (*GetParametrModelFunc)(void*, const char*);
typedef void (*SetParallelParametrFunc)(void*, const char*, double);
typedef double (*GetParallelParametrModelFunc)(void*, const char*);
typedef void (*AddComponentFunc)(void*, const char*, Model*);
typedef Model* (*GetComponentFunc)(void*, const char*);
typedef int (*ComponentsSizeFunc)(void*);
typedef void (*AddMetricFunc)(void*, Metric*);
typedef Metric* (*GetMetricFunc)(void*, int);
typedef int (*MetricsSizeFunc)(void*); 
typedef int (*AllComponentsMetricsSizeFunc)(void*);
typedef void (*PrintNameModelFunc)(void*);
typedef void (*PrintClassFunc)(void*);
typedef const char* (*GetNameModelFunc)(void*);
typedef void (*MetricsOutputFunc)(void*);
typedef double (*DemonstrateModelFunc)(void*);
typedef vector <Metric*>(*GetAllComponentsMetricsFunc)(void*);
typedef int (*ParallelParametrsSizeFunc)(void*);
typedef void (*AddComponentModelAdressFunc)(void*, const char*, string);
typedef string (*GetComponentModelAdressFunc)(void*, const char*);
typedef string(*GetNextModelNameFunc)(void*);
typedef map<string, double>(*GetAllParametrsFunc)(void*);
typedef map<string, vector<double>>(*GetAllParallelParametrsFunc)(void*);


const double EPS = 1e-12;

// Структура для хранения параметров одного цикла (после извлечения из map)
struct LoopParams {
    std::string name;
    double start;
    double step;
    double end;
    size_t count; // количество итераций
};

// Вычисление количества итераций для одного цикла (double)
size_t compute_count(double start, double step, double end) {
    if (std::fabs(step) < EPS)
        throw std::runtime_error("Step is too close to zero");
    if (step > 0) {
        if (start > end + EPS) return 0;
        double n = (end - start) / step;
        return static_cast<size_t>(floor(n + EPS)) + 1;
    }
    else { // step < 0
        if (start < end - EPS) return 0;
        double n = (start - end) / (-step);
        return static_cast<size_t>(floor(n + EPS)) + 1;
    }
}

// Извлечение параметров из map в вектор (с сохранением порядка)
std::vector<LoopParams> extract_loops(const std::map<std::string, std::vector<double>>& loops_map) {
    std::vector<LoopParams> loops;
    for (const auto& pair : loops_map) {
        const auto& vec = pair.second;
        if (vec.size() != 3)
            throw std::runtime_error("Each map entry must have exactly 3 values: start, step, end");
        LoopParams lp;
        lp.name = pair.first;
        lp.start = vec[0];
        lp.step = vec[1];
        lp.end = vec[2];
        lp.count = compute_count(lp.start, lp.step, lp.end);
        loops.push_back(lp);
    }
    return loops;
}

// Класс-обёртка для работы с метрикой
class DynamicMetric {
private:
    void* instance;
    HMODULE dllHandle;

    // Указатели на функции
    CreateMetricFunc createMetric;
    DestroyMetricFunc destroyMetric;
    CalculateMetricFunc calculateMetric;
    SetValueFunc setValue;
    GetValueFunc getValue;
    AddParametrFunc addParametr;
    GetParametrFunc getParametr;
    AddMainParametrFunc addMainParametr;
    GetMainParametrFunc getMainParametr;
    PrintNameFunc printName;
    PrintTypeFunc printType;
    GetNameFunc getName;
    DemonstrateMetricFunc demonstrateMetric;

public:
    DynamicMetric() : instance(nullptr), dllHandle(nullptr) {
        createMetric = nullptr;
        destroyMetric = nullptr;
        calculateMetric = nullptr;
        setValue = nullptr;
        getValue = nullptr;
        addParametr = nullptr;
        getParametr = nullptr;
        addMainParametr = nullptr;
        getMainParametr = nullptr;
        printName = nullptr;
        printType = nullptr;
        getName = nullptr;
        demonstrateMetric = nullptr; 
    }

    ~DynamicMetric() {
        cleanup();
    }

    bool load(const string& dllPath, const string& metricName, const string& metricType) {
        // Выгружаем предыдущую DLL
        cleanup();

        // Загружаем DLL
        dllHandle = LoadLibraryA(dllPath.c_str());
        if (!dllHandle) {
            cerr << "Ошибка загрузки DLL: " << dllPath << endl;
            cerr << "Код ошибки: " << GetLastError() << endl;
            return false;
        }
        else
        {
            cout << "Загружена DLL: " << dllPath << endl;
        }

        // Получаем указатели на функции
        createMetric = (CreateMetricFunc)GetProcAddress(dllHandle, "CreateMetric");
        destroyMetric = (DestroyMetricFunc)GetProcAddress(dllHandle, "DestroyMetric");
        calculateMetric = (CalculateMetricFunc)GetProcAddress(dllHandle, "CalculateMetric");
        setValue = (SetValueFunc)GetProcAddress(dllHandle, "SetValue");
        getValue = (GetValueFunc)GetProcAddress(dllHandle, "GetValue");
        addParametr = (AddParametrFunc)GetProcAddress(dllHandle, "AddParametr");
        getParametr = (GetParametrFunc)GetProcAddress(dllHandle, "GetParametr");
        addMainParametr = (AddMainParametrFunc)GetProcAddress(dllHandle, "AddMainParametr");
        getMainParametr = (GetMainParametrFunc)GetProcAddress(dllHandle, "GetMainCurrentParametr");
        printName = (PrintNameFunc)GetProcAddress(dllHandle, "PrintName");
        printType = (PrintTypeFunc)GetProcAddress(dllHandle, "PrintType");
        getName = (GetNameFunc)GetProcAddress(dllHandle, "GetName");
        demonstrateMetric = (DemonstrateMetricFunc)GetProcAddress(dllHandle, "DemonstrateMetric");

        if (!createMetric)
            cout << "not createMetric" << endl;
        if (!destroyMetric)
            cout << "not destroyMetric" << endl;
        if (!calculateMetric)
            cout << "not calculateMetric" << endl;
        if (!setValue)
            cout << "not setValue" << endl;
        if (!getValue)
            cout << "not getValue" << endl;
        if (!addParametr)
            cout << "not addParametr" << endl;
        if (!getParametr)
            cout << "not getParametr" << endl;
        if (!addMainParametr)
            cout << "not addMainParametr" << endl;
        if (!getMainParametr)
            cout << "not getMainParametr" << endl;
        if (!printName)
            cout << "not printName" << endl;
        if (!printType)
            cout << "not printType" << endl;
        if (!getName)
            cout << "not getName" << endl;
        if (!demonstrateMetric)
            cout << "not demonstrateMetric" << endl;
        // Проверяем, что все функции загружены
        if (!createMetric || !destroyMetric || !calculateMetric || !setValue ||
            !getValue || !addParametr || !getParametr || !addMainParametr ||
            !getMainParametr || !printName || !printType || !getName || !demonstrateMetric) {
            cerr << "Ошибка: не все функции найдены в DLL" << endl;
            FreeLibrary(dllHandle);
            dllHandle = nullptr;
            return false;
        }

        // Создаём экземпляр метрики
        instance = createMetric(metricName.c_str(), metricType.c_str());
        if (!instance) {
            cerr << "Ошибка создания экземпляра метрики" << endl;
            FreeLibrary(dllHandle);
            dllHandle = nullptr;
            return false;
        }

        return true;
    }

    // Методы для работы с метрикой
    double calculateMetricF() {
        if (instance && calculateMetric) {
            return calculateMetric(instance);
        }
        return 0.0;
    }

    void setValueF(double val) {
        if (instance && setValue) {
            setValue(instance, val);
        }
    }

    double getValueF() {
        if (instance && getValue) {
            return getValue(instance);
        }
        return 0.0;
    }

    void addParametrF(const string& param_name, double param_value) {
        if (instance && addParametr) {
            addParametr(instance, param_name.c_str(), param_value);
        }
    }

    double getParametrF(const string& param_name) {
        if (instance && getParametr) {
            return getParametr(instance, param_name.c_str());
        }
        return 0.0;
    }

    void addMainParametrF(double main_param_value) {
        if (instance && addMainParametr) {
            addMainParametr(instance, main_param_value);
        }
    }

    double getMainParametrF(int nomer) {
        if (instance && getMainParametr) {
            return getMainParametr(instance, nomer);
        }
        return 0.0;
    }

    void printNameF() {
        if (instance && printName) {
            printName(instance);
        }
    }

    void printTypeF() {
        if (instance && printType) {
            printType(instance);
        }
    }

    string getNameF() {
        if (instance && getName) {
            const char* name = getName(instance);
            return name ? name : "";
        }
        return "";
    }

    double demonstrateMetricF() {
        if (instance && demonstrateMetric) {
            return demonstrateMetric(instance);
        }
        return 0.0;
    }

    bool isLoaded() const {
        return instance != nullptr && dllHandle != nullptr;
    }

    void cleanup() {
        if (instance && destroyMetric) {
            destroyMetric(instance);
            instance = nullptr;
        }

        if (dllHandle) {
            FreeLibrary(dllHandle);
            dllHandle = nullptr;
        }
    }
};

class DynamicModel {
private:
    void* instance;
    HMODULE dllHandle;
    vector <DynamicModel> components_model;

    // Указатели на функции
    CreateModelFunc createModel;
    DestroyModelFunc destroyModel;
    ModelingWayFunc modelingWay;
    CollectingMetricFunc collectingMetric;
    SetParametrFunc setParametr;
    GetParametrModelFunc getParametr;
    AddComponentFunc addComponent;
    GetComponentFunc getComponent;
    ComponentsSizeFunc componentsSize;
    AddMetricFunc addMetric;
    GetMetricFunc getMetric;
    MetricsSizeFunc metricsSize;
    AllComponentsMetricsSizeFunc allComponentsMetricsSize;
    PrintNameModelFunc printNameModel;
    PrintClassFunc printClass;
    GetNameModelFunc getNameModel;
    MetricsOutputFunc metricsOutput;
    DemonstrateModelFunc demonstrateModel;
    GetAllComponentsMetricsFunc getAllComponentsMetrics;
    SetParallelParametrFunc setParallelParametr;
    GetParallelParametrModelFunc getParallelParametrModel;
    ParallelParametrsSizeFunc parallelParametrsSize;
    AddComponentModelAdressFunc addComponentModelAdress;
    GetComponentModelAdressFunc getComponentModelAdress;
    GetNextModelNameFunc getNextModelName;
    GetAllParametrsFunc getAllParametrs;
    GetAllParallelParametrsFunc getAllParallelParametrs;



public:
    DynamicModel() : instance(nullptr), dllHandle(nullptr) {
        createModel = nullptr;
        destroyModel = nullptr;
        modelingWay = nullptr;
        collectingMetric = nullptr;
        setParametr = nullptr;
        getParametr = nullptr;
        addComponent = nullptr;
        getComponent = nullptr;
        componentsSize = nullptr;
        addMetric = nullptr;
        getMetric = nullptr;
        metricsSize = nullptr;
        allComponentsMetricsSize = nullptr;
        printNameModel = nullptr;
        printClass = nullptr;
        getNameModel = nullptr;
        metricsOutput = nullptr;
        demonstrateModel = nullptr;
        getAllComponentsMetrics = nullptr;
        setParallelParametr = nullptr;
        getParallelParametrModel = nullptr;
        parallelParametrsSize = nullptr;
        addComponentModelAdress = nullptr;
        getComponentModelAdress = nullptr;
        getNextModelName = nullptr;
        getAllParametrs = nullptr;
        getAllParallelParametrs = nullptr;
    }

    ~DynamicModel() {
        cleanup();
    }

    bool load(const string& dllPath, const string& modelName, const string& modelClass) {
        // Выгружаем предыдущую DLL
        cleanup();

        // Загружаем DLL
        dllHandle = LoadLibraryA(dllPath.c_str());
        if (!dllHandle) {
            cerr << "Ошибка загрузки DLL: " << dllPath << endl;
            cerr << "Код ошибки: " << GetLastError() << endl;
            return false;
        }
        

        // Получаем указатели на функции

        createModel = (CreateModelFunc)GetProcAddress(dllHandle, "CreateModel");
        destroyModel = (DestroyModelFunc)GetProcAddress(dllHandle, "DestroyModel");
        modelingWay = (ModelingWayFunc)GetProcAddress(dllHandle, "ModelingWay");
        collectingMetric = (CollectingMetricFunc)GetProcAddress(dllHandle, "CollectingMetric");
        setParametr = (SetParametrFunc)GetProcAddress(dllHandle, "SetParametr");
        getParametr = (GetParametrModelFunc)GetProcAddress(dllHandle, "GetParametr");
        addComponent = (AddComponentFunc)GetProcAddress(dllHandle, "AddComponent");
        getComponent = (GetComponentFunc)GetProcAddress(dllHandle, "GetComponent");
        componentsSize = (ComponentsSizeFunc)GetProcAddress(dllHandle, "ComponentsSize");
        addMetric = (AddMetricFunc)GetProcAddress(dllHandle, "AddMetric");
        getMetric = (GetMetricFunc)GetProcAddress(dllHandle, "GetMetric");
        printNameModel = (PrintNameModelFunc)GetProcAddress(dllHandle, "PrintName");
        printClass = (PrintClassFunc)GetProcAddress(dllHandle, "PrintClass");
        getNameModel = (GetNameModelFunc)GetProcAddress(dllHandle, "GetName");
        metricsOutput = (MetricsOutputFunc)GetProcAddress(dllHandle, "MetricsOutput");
        demonstrateModel = (DemonstrateModelFunc)GetProcAddress(dllHandle, "DemonstrateModel");
        metricsSize = (MetricsSizeFunc)GetProcAddress(dllHandle, "MetricsSize");
        allComponentsMetricsSize = (AllComponentsMetricsSizeFunc)GetProcAddress(dllHandle, "AllComponentsMetricsSize");
        getAllComponentsMetrics = (GetAllComponentsMetricsFunc)GetProcAddress(dllHandle, "GetAllComponentsMetrics");
        setParallelParametr = (SetParallelParametrFunc)GetProcAddress(dllHandle, "SetParallelParametr");
        getParallelParametrModel = (GetParallelParametrModelFunc)GetProcAddress(dllHandle, "GetParallelParametr");
        parallelParametrsSize = (ParallelParametrsSizeFunc)GetProcAddress(dllHandle, "ParallelParametrsSize");
        addComponentModelAdress = (AddComponentModelAdressFunc)GetProcAddress(dllHandle, "AddComponentModelAdress");
        getComponentModelAdress = (GetComponentModelAdressFunc)GetProcAddress(dllHandle, "GetComponentModelAdress");
        getNextModelName = (GetNextModelNameFunc)GetProcAddress(dllHandle, "GetNextModelName");
        getAllParametrs = (GetAllParametrsFunc)GetProcAddress(dllHandle, "GetAllParametrs");
        getAllParallelParametrs = (GetAllParallelParametrsFunc)GetProcAddress(dllHandle, "GetAllParallelParametrs");
        if (!createModel)
            cout << "not createModel" << endl;
        if (!destroyModel)
            cout << "not destroyModel" << endl;
        if (!modelingWay)
            cout << "not modelingWay" << endl;
        if (!collectingMetric)
            cout << "not collectingMetric" << endl;
        if (!setParametr)
            cout << "not setParametr" << endl;
        if (!getParametr)
            cout << "not getParametr" << endl;
        if (!addComponent)
            cout << "not addComponent" << endl;
        if (!getComponent)
            cout << "not getComponent" << endl;
        if (!addMetric)
            cout << "not addMetric" << endl;
        if (!getMetric)
            cout << "not getMetric" << endl;
        if (!printNameModel)
            cout << "not printNameModel" << endl;
        if (!printClass)
            cout << "not printClass" << endl;
        if (!getNameModel)
            cout << "not getNameModel" << endl;
        if (!metricsOutput)
            cout << "not metricsOutput" << endl;
        if (!demonstrateModel)
            cout << "not demonstrateModel" << endl;
        if (!metricsSize)
            cout << "not metricsSize" << endl;
        if (!allComponentsMetricsSize)
            cout << "not allComponentsMetricsSize" << endl;
        if (!componentsSize)
            cout << "not componentsSize" << endl;
        if (!getAllComponentsMetrics)
            cout << "not getAllComponentsMetrics" << endl;
        if (!setParallelParametr)
            cout << "not setParallelParametr" << endl;
        if (!getParallelParametrModel)
            cout << "not getParallelParametr" << endl;
        if (!parallelParametrsSize)
            cout << "not ParallelParametrsSize" << endl;
        if (!addComponentModelAdress)
            cout << "not addComponentModelAdress" << endl;
        if (!getComponentModelAdress)
            cout << "not getComponentModelAdress" << endl;
        if (!getNextModelName)
            cout << "not getNextModelName" << endl;
        if (!getAllParametrs)
            cout << "not getAllParametrs" << endl;
        if (!getAllParallelParametrs)
            cout << "not getAllParallelParametrs" << endl;
        // Проверяем, что все функции загружены
        if (!createModel || !destroyModel || !modelingWay || !collectingMetric ||
            !setParametr || !getParametr || !addComponent || !getComponent ||
            !addMetric || !getMetric || !printNameModel || !printClass || 
            !getNameModel || !metricsOutput || !demonstrateModel || !metricsSize || 
            !allComponentsMetricsSize || !componentsSize || !getAllComponentsMetrics || 
            !setParallelParametr || !getParallelParametrModel || !parallelParametrsSize ||
            !addComponentModelAdress || !getComponentModelAdress || !getNextModelName || 
            !getAllParametrs || !getAllParallelParametrs) {
            cerr << "Ошибка: не все функции найдены в DLL" << endl;
            FreeLibrary(dllHandle);
            dllHandle = nullptr;
            return false;
        }

        // Создаём экземпляр модели
        instance = createModel(modelName.c_str(), modelClass.c_str());
        if (!instance) {
            cerr << "Ошибка создания экземпляра модели" << endl;
            FreeLibrary(dllHandle);
            dllHandle = nullptr;
            return false;
        }
        return true;
    }

    // Методы для работы с метрикой
    double modelingWayF() {
        if (instance && modelingWay) {
            double result = modelingWay(instance);
            while (result != 0) {
                auto start_seq = std::chrono::steady_clock::now();
                if (result == 1) //последовательный проход компонента
                {
                    DynamicModel component_model;
                    if (componentsSizeF() > 0)
                    {
                        if (getNextModelNameF() != "") {
                            if (component_model.load(getComponentModelAdressF(getNextModelNameF()), getNextModelNameF(), "")) {
                                Model* component = getComponentF(getNextModelNameF());
                                map <string, double> component_parametrs = component->get_all_parametrs();
                                for (std::map<string, double>::iterator param = component_parametrs.begin(); param != component_parametrs.end(); ++param) {
                                    component_model.setParametrF(param->first, param->second);
                                }
                                result = component_model.modelingWayF();
                                vector<Metric*>component_metrics = component_model.getAllComponentsMetricsF();
                                for (int ind = 0; ind < component_metrics.size(); ind++)
                                {
                                    component->add_metric(component_metrics[ind]);
                                }
                                addComponentF(getNextModelNameF(), component);
                            }
                        }
                    }
                }
                if (result == 2) //параллельный проход компонента
                {
                    if (componentsSizeF() > 0)
                    {
                        if (getNextModelNameF() != "") {
                            if (mainParametrsSizeF() > 0) {
                                Model* component = getComponentF(getNextModelNameF());
                                vector <vector<Metric*>> local_metrics;
                                map <string, double> component_parametrs = component->get_all_parametrs();
                                map <string, vector<double>> parallel_parametrs = getAllParallelParametrsF();
                                std::vector<LoopParams> loops = extract_loops(parallel_parametrs);
                                if (!loops.empty()) {
                                    const size_t loops_size = loops.size();

                                    size_t total = 1;
                                    for (const auto& lp : loops) {
                                        if (lp.count == 0) {
                                            throw std::runtime_error("Total iterations = 0");
                                        }
                                        if (total > SIZE_MAX / lp.count)
                                            throw std::runtime_error("Total iterations overflow");
                                        total *= lp.count;
                                    }

                                    std::vector<size_t> strides(loops_size);
                                    strides.back() = 1;
                                    for (size_t i = loops_size - 1; i > 0; --i) {
                                        strides[i - 1] = strides[i] * loops[i].count;
                                    }
                                    local_metrics.resize(total);
                                    cout << "total loop size=" << total << endl;
                                    // Параллельный цикл
                                    #pragma omp parallel for
                                    for (int idx = 0; idx < total; ++idx) {
                                     // начало цикла
                                        int thread_num = omp_get_thread_num();
                                        cout << "Поток " << thread_num << " начал работу" << endl;
                                        std::map<std::string, double> values;
                                        size_t remainder = idx;
                                        for (size_t i = 0; i < loops_size; ++i) {
                                            size_t local_idx = remainder / strides[i];
                                            remainder %= strides[i];
                                            double val = loops[i].start + static_cast<double>(local_idx) * loops[i].step;
                                            values[loops[i].name] = val;
                                        }
                                        
                                        DynamicModel component_model;
                                        component_model.load(getComponentModelAdressF(getNextModelNameF()), getNextModelNameF(), "");

                                        for (std::map<string, double>::iterator param = component_parametrs.begin(); param != component_parametrs.end(); ++param) {
                                            component_model.setParametrF(param->first, param->second);
                                        }
                                        for (std::map<string, double>::iterator par_param = values.begin(); par_param != values.end(); ++par_param) {
                                            component_model.setParametrF(par_param->first, par_param->second);
                                        }
                                        result = component_model.modelingWayF();
                                        vector<Metric*>component_metrics = component_model.getAllComponentsMetricsF();
                                        
                                        local_metrics[thread_num].insert(local_metrics[thread_num].end(), component_metrics.begin(), component_metrics.end());
                                    }
                                    //конец цикла
                                }
                                for (int ind1 = 0; ind1 < local_metrics.size(); ind1++)
                                {
                                    for (int ind2 = 0; ind2 < local_metrics[ind1].size(); ind2++)
                                    {
                                        component->add_metric(local_metrics[ind1][ind2]);
                                    }
                                }
                                addComponentF(getNextModelNameF(), component);
                            }    
                        }
                        else
                        {
                            DynamicModel component_model;
                            if (component_model.load(getComponentModelAdressF(getNextModelNameF()), getNextModelNameF(), "")) {
                                Model* component = getComponentF(getNextModelNameF());
                                map <string, double> component_parametrs = component->get_all_parametrs();
                                for (std::map<string, double>::iterator param = component_parametrs.begin(); param != component_parametrs.end(); ++param) {
                                    cout << param->first << " " << param->second << endl;
                                    component_model.setParametrF(param->first, param->second);
                                }
                                result = component_model.modelingWayF();
                                vector<Metric*>component_metrics = component_model.getAllComponentsMetricsF();
                                for (int ind = 0; ind < component_metrics.size(); ind++)
                                {
                                    component->add_metric(component_metrics[ind]);
                                }
                                addComponentF(getNextModelNameF(), component);
                            }
                        }
                        //result = component_model.modelingWayF();        
                    }
                }
                auto end_seq = std::chrono::steady_clock::now();
                double time_seq = std::chrono::duration<double>(end_seq - start_seq).count();
                std::cout << "Time completion: " << time_seq << " s\n";
                }
            return result;
        }
        return 0.0;
    }

    double collectingMetricF() {
        if (instance && collectingMetric) {
            return collectingMetric(instance);
        }
        return 0.0;
    }

    map <string, double> getAllParametrsF() {
        if (instance && getAllParametrs) {
            return getAllParametrs(instance);
        }
        map <string, double> empty;
        return empty;
    }

    void addComponentF(const string& model_name, Model* model) {
        if (instance && addComponent) {
            addComponent(instance, model_name.c_str(), model);
        }
    }
    Model* getComponentF(const string& model_name) {
        if (instance && getComponent) {
            return getComponent(instance, model_name.c_str());
        }
        return nullptr;
    }

    map <string, vector<double>> getAllParallelParametrsF() {
        if (instance && getAllParallelParametrs) {
            return getAllParallelParametrs(instance);
        }
        map <string, vector<double>> empty;
        return empty;
    }

    int componentsSizeF() {
        if (instance && componentsSize) {
            return componentsSize(instance);
        }
    }

    void setParametrF(const string& param_name, double param_value) {
        if (instance && setParametr) {
            setParametr(instance, param_name.c_str(), param_value);
        }
    }

    double getParametrF(const string& param_name) {
        if (instance && getParametr) {
            return getParametr(instance, param_name.c_str());
        }
        return 0.0;
    }

    string getNextModelNameF() {
        if (instance && getNextModelName) {
            return getNextModelName(instance);
        }
        else
            return "";
    }

    void setMainParametrF(const string& param_name, double param_value) {
        if (instance && setParallelParametr) {
            setParallelParametr(instance, param_name.c_str(), param_value);
        }
    }

    double getMainParametrF(const string& param_name) {
        if (instance && getParallelParametrModel) {
            return getParallelParametrModel(instance, param_name.c_str());
        }
        return 0.0;
    }

    void addMetricF(Metric* metric) {
        if (instance && addMetric) {
            addMetric(instance, metric);
        }
    }

    Metric* getMetricF(int nomer) {
        if (instance && getMetric) {
            return getMetric(instance, nomer);
        }
        return nullptr;
    }

    void addComponentModelAdressF(const string& model_name, string model_adress) {
        if (instance && addComponentModelAdress) {
            addComponentModelAdress(instance, model_name.c_str(), model_adress);
        }
    }

    string getComponentModelAdressF(const string& model_name) {
        if (instance && getComponentModelAdress) {
            return getComponentModelAdress(instance, model_name.c_str());
        }
        else
            return "";
    }

    int metricsSizeF() {
        if (instance && metricsSize) {
            return metricsSize(instance);
        }
        else
            return 0;
    }

    int mainParametrsSizeF() {
        if (instance && parallelParametrsSize) {
            return parallelParametrsSize(instance);
        }
        else
            return 0;
    }

    int allComponentsMetricsSizeF() {
        if (instance) {
            return allComponentsMetricsSize(instance);
        }
    }

    void printNameF() {
        if (instance && printNameModel) {
            printNameModel(instance);
        }
    }

    void printClassF() {
        if (instance && printClass) {
            printClass(instance);
        }
    }

    string getNameF() {
        if (instance && getNameModel) {
            const char* name = getNameModel(instance);
            return name ? name : "";
        }
        return "";
    }

    void metricsOutputF() {
        if (instance && metricsOutput) {
            metricsOutput(instance);
        }
    }

    double demonstrateModelF() {
        if (instance && demonstrateModel) {
           return demonstrateModel(instance);
        }
        return 0.0;
    }

    vector <Metric*> getAllComponentsMetricsF() {
        if (instance && getAllComponentsMetrics)
        {
            return getAllComponentsMetrics(instance);
        }
        vector <Metric*> empty;
        empty.resize(0);
        return empty;
    }

    bool isLoaded() const {
        return instance != nullptr && dllHandle != nullptr;
    }

    void cleanup() {
        if (instance && destroyModel) {
            destroyModel(instance);
            instance = nullptr;
        }

        if (dllHandle) {
            FreeLibrary(dllHandle);
            dllHandle = nullptr;
        }
    }
};

class Graphic {

public:
    map <string, double> fixed_graphic_parametrs;
    string parametr1_name;
    string parametr2_name;
    vector<double> parametr1_values;
    vector<double> parametr2_values;
    vector<vector<double>> values;
};

class Exception_all : public std::exception {
private:
    std::string message;
public:
    Exception_all(const std::string& msg) : message(msg) {}

    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};

class Simulation_system {

private:
    DynamicModel model;
    string dllPathModel;
    string dllPathMetric;
    string variant = "";
    string name_model = "";
    string class_model;
    DynamicMetric metric;
    string name_metric = "";
    string type_metric = "";
    U16String excel_file_name = "";
    string file_name;
    
public:

    void start_modeling() {
        
        while (variant != "exit") {
            cout << "Введите metric - для моделирования метрики или model - для моделирования модели, exit - для выхода из программы:";
            cin >> variant;
            if (variant == "model") {
                cout << "Введите адрес dll-файла модели: ";
                cin >> dllPathModel;
               
                cout << "\nЗагружаем DLL: " << dllPathModel << endl;

                while (name_model == "") {
                    cout << "Введите наименование модели:";
                    cin >> name_model;
                }
                while (class_model == "") {
                    cout << "Введите класс модели:";
                    cin >> class_model;
                }

                bool loadedModel = model.load(dllPathModel, name_model, class_model);

                if (loadedModel) {

                    cout << "Имя модели (print_name): ";
                    model.printNameF();
                    cout << "Класс модели (print_type): ";
                    model.printClassF();
                    string choise = "";
                    while (choise != "exit") {
                        cout << "Введите demonstrate - чтобы вызвать демонстрационную функцию модели, modeling - чтобы вызвать функцию моделирования или exit для завершения работы программы:";

                        cin >> choise;
                        if (choise == "demonstrate")
                        {
                            model.demonstrateModelF();
                        }
                        else {
                            if (choise == "modeling")
                            {
                                cout << "Введите имя файла Excel для сохранения результатов и построения графиков:";
                                cin >> file_name;
                                model.modelingWayF();
                                model.metricsOutputF();
                                try {
                                    save_instance(file_name);
                                }
                                catch (const Exception_all& e) { // Перехват
                                    std::cerr << e.what() << std::endl;
                                }
                            }
                        }
                    }
                }
                else {
                    cerr << "\nНе удалось загрузить DLL. Проверьте:\n";
                    cerr << "1. Существует ли файл DLL\n";
                    cerr << "2. Находится ли DLL в той же папке, что и исполняемый файл\n";
                    cerr << "3. Скомпилирована ли DLL с правильными настройками\n";
                }
            }
            else
            {
                if (variant == "metric") {
                    cout << "Введите адрес dll-файла метрики: ";
                    cin >> dllPathMetric;
                    

                    cout << "\nЗагружаем DLL: " << dllPathMetric << endl;
                    
                    cout << "Введите наименование метрики:";
                    cin >> name_metric;
                    cout << "Введите тип метрики:";
                    cin >> type_metric;

                    bool loadedMetric = metric.load(dllPathMetric, name_metric, type_metric);

                    if (loadedMetric) {

                        cout << "Имя модели (print_name): ";
                        metric.printNameF();
                        cout << "Класс модели (print_type): ";
                        metric.printTypeF();
                        string choise = "";
                        //cout << "Введите demonstrate - чтобы вызвать демонстрационную функцию модели, modeling - чтобы вызвать функцию моделирования или exit для завершения работы программы:";
                        while (choise != "exit") {
                            cout << "Введите demonstrate - чтобы вызвать демонстрационную функцию метрики или exit для завершения работы программы:";
                            cin >> choise;
                            if (choise == "demonstrate")
                            {
                                metric.demonstrateMetricF();
                            }
                        }
                    }
                    else {
                        cerr << "\nНе удалось загрузить DLL. Проверьте:\n";
                        cerr << "1. Существует ли файл DLL\n";
                        cerr << "2. Находится ли DLL в той же папке, что и исполняемый файл\n";
                        cerr << "3. Скомпилирована ли DLL с правильными настройками\n";
                    }
                }
            }
        }
    }


    void save_instance(string file)
    {
        unique_ptr<Workbook> wkb;
        excel_file_name = file_name.c_str();
        ifstream f(file_name);
        if (f.good()) {
            f.close();
            wkb = std::make_unique<Workbook>(excel_file_name);
        }
        else {
            wkb = std::make_unique<Workbook>();
        }
        if (name_model != "")
        {
            if (model.allComponentsMetricsSizeF() > 0) {
                WorksheetCollection worksheetc = wkb->GetWorksheets();
                vector <Metric*> metrics_to_excel=model.getAllComponentsMetricsF();
                unordered_set <string> metrics_types;
                map <string, vector <Metric*>> metrics_devided_by_types;
                for (int i = 0; i < metrics_to_excel.size(); i++)
                {
                    if ((metrics_to_excel[i]->main_parametrs_size()>1)&&(metrics_to_excel[i]->main_parametrs_size() > metrics_to_excel[i]->fixed_main_parametrs_size()))
                    {
                        throw Exception_all("Ошибка: Количество фиксированных параметров меньше общего количества параметров!"); 
                    }
                    metrics_types.insert(metrics_to_excel[i]->get_type());
                    if (metrics_devided_by_types.find(metrics_to_excel[i]->get_type()) == metrics_devided_by_types.end()) 
                    {
                        vector <Metric*> metrics_type{ metrics_to_excel[i] };
                        metrics_devided_by_types[metrics_to_excel[i]->get_type()] = metrics_type;
                    }
                    else {
                        metrics_devided_by_types[metrics_to_excel[i]->get_type()].push_back(metrics_to_excel[i]);
                    }
                }

                for (std::map<std::string, vector<Metric*>>::iterator metr = metrics_devided_by_types.begin(); metr != metrics_devided_by_types.end(); ++metr)
                {
                    int sheet_number=worksheetc.Add();
                    Worksheet worksheet = worksheetc.Get(sheet_number);
                    //U16String worksheet_name = (metr->first).c_str();
                    //worksheet.SetName(worksheet_name);
                    cout << " Тип =" << metr->first << endl;
                    int excel_cell_row = 0;
                    int excel_cell_column = 0;
                    int max_column_to_graphic = 0;
                    U16String names;
                    map <string, double> metrics_main_parametrs = metrics_devided_by_types[metr->first][0]->get_all_main_parametrs();
                    int main_params_size = metrics_devided_by_types[metr->first][0]->main_parametrs_size();
                    int fixed_main_params_size = metrics_devided_by_types[metr->first][0]->fixed_main_parametrs_size();
                    for (std::map<std::string, double>::iterator it = metrics_main_parametrs.begin(); it != metrics_main_parametrs.end(); ++it)
                    {
                        names = it->first.c_str();
                        worksheet.GetCells().Get(excel_cell_row, excel_cell_column).PutValue(names);
                        excel_cell_column++;
                    }
                    names = metr->first.c_str();
                    worksheet.GetCells().Get(excel_cell_row, excel_cell_column).PutValue(names);
                    excel_cell_row++;
                    max_column_to_graphic = excel_cell_column + 2;
                    map<string, vector<vector<double>>> values_for_graphics;
                    for (int i = 0; i < metrics_devided_by_types[metr->first].size(); i++)
                    {
                        excel_cell_column = 0;
                        metrics_main_parametrs = metrics_devided_by_types[metr->first][i]->get_all_main_parametrs();
                        
                        if (main_params_size > 1){
                            if (fixed_main_params_size > 0) {

                                for (std::map<std::string, double>::iterator param = metrics_main_parametrs.begin(); param != metrics_main_parametrs.end(); ++param)
                                {
                                    bool flag_param = true;
                                    for (std::map<std::string, double>::iterator param2 = metrics_main_parametrs.begin(); param2 != metrics_main_parametrs.end(); ++param2)
                                    {
                                        if (param->first != param2->first)
                                        {
                                            if (metrics_main_parametrs[param2->first] != metrics_devided_by_types[metr->first][i]->get_fixed_main_parametr(param2->first))
                                            {
                                                flag_param = false;
                                                break;
                                            }
                                        }
                                    }
                                    if (flag_param)
                                    {
                                        vector<double> temp_vector{ metrics_main_parametrs[param->first], metrics_devided_by_types[metr->first][i]->calculate_metric() };
                                        values_for_graphics[param->first].push_back(temp_vector);
                                    }
                                    worksheet.GetCells().Get(excel_cell_row, excel_cell_column).PutValue(metrics_main_parametrs[param->first]);
                                    excel_cell_column++;
                                }
                            }
                            if (main_params_size == 2) {
                                std::map<std::string, double>::iterator param1 = metrics_main_parametrs.begin();
                                string graphic_parametr1_name = param1->first;
                                double graphic_parametr1_value = param1->second;
                                ++param1;
                                string graphic_name = graphic_parametr1_name+"|"+param1->first;
                                vector<double> temp_vector{ graphic_parametr1_value, param1->second, metrics_devided_by_types[metr->first][i]->calculate_metric() };
                                values_for_graphics[graphic_name].push_back(temp_vector);
                            }
                            else 
                            {
                                unordered_set<string> passed_parametrs;
                                
                                for (std::map<std::string, double>::iterator param1 = metrics_main_parametrs.begin(); param1 != metrics_main_parametrs.end(); ++param1)
                                {   
                                    bool flag_param = true;
                                    passed_parametrs.insert(param1->first);
                                    for (std::map<std::string, double>::iterator param2 = metrics_main_parametrs.begin(); param2 != metrics_main_parametrs.end(); ++param2)
                                    {
                                        if (passed_parametrs.count(param2->first)==0)
                                        {
                                            for (std::map<std::string, double>::iterator param3 = metrics_main_parametrs.begin(); param3 != metrics_main_parametrs.end(); ++param3)
                                            {
                                                if (param1->first != param3->first && param2->first != param3->first)
                                                {
                                                    if (metrics_main_parametrs[param3->first] != metrics_devided_by_types[metr->first][i]->get_fixed_main_parametr(param3->first))
                                                    {
                                                        flag_param = false;
                                                        break;
                                                    }
                                                }
                                            }

                                            if (flag_param)
                                            {
                                                string graphic_name = param1->first + "|" + param2->first;
                                                vector<double> temp_vector{ param1->second, param2->second, metrics_devided_by_types[metr->first][i]->calculate_metric() };
                                                values_for_graphics[graphic_name].push_back(temp_vector);
                                            }
                                        }
                                    }
                                    worksheet.GetCells().Get(excel_cell_row, excel_cell_column).PutValue(metrics_main_parametrs[param1->first]);
                                    excel_cell_column++;   
                                }
                            }
                        }

                        if (main_params_size==1){
                            std::map<std::string, double>::iterator param = metrics_main_parametrs.begin();
                            vector<double> temp_vector{ metrics_main_parametrs[param->first], metrics_devided_by_types[metr->first][i]->calculate_metric() };
                            values_for_graphics[param->first].push_back(temp_vector);
                            worksheet.GetCells().Get(excel_cell_row, excel_cell_column).PutValue(metrics_main_parametrs[param->first]);
                            excel_cell_column++;
                        }
                        
                        worksheet.GetCells().Get(excel_cell_row, excel_cell_column).PutValue(metrics_devided_by_types[metr->first][i]->calculate_metric());
                        excel_cell_row++;
                    }
                    
                    int graphic_position = 10;
                    for (std::map<std::string, vector<vector<double>>>::iterator val = values_for_graphics.begin(); val != values_for_graphics.end(); ++val)
                    {
                        if (values_for_graphics[val->first][0].size() == 2)
                        {
                            excel_cell_row = 0;
                            excel_cell_column = max_column_to_graphic;
                            names = val->first.c_str();
                            worksheet.GetCells().Get(excel_cell_row, excel_cell_column).PutValue(names);
                            names = metr->first.c_str();
                            worksheet.GetCells().Get(excel_cell_row, excel_cell_column + 1).PutValue(names);
                            excel_cell_row++;
                            for (int val_ind = 0; val_ind < values_for_graphics[val->first].size(); val_ind++)
                            {
                                excel_cell_column = max_column_to_graphic;
                                for (int val_val_ind = 0; val_val_ind < values_for_graphics[val->first][val_ind].size(); val_val_ind++)
                                {
                                    worksheet.GetCells().Get(excel_cell_row, excel_cell_column).PutValue(values_for_graphics[val->first][val_ind][val_val_ind]);
                                    excel_cell_column++;
                                }
                                excel_cell_row++;
                            }

                            int chartIndex = worksheet.GetCharts().Add(Aspose::Cells::Charts::ChartType::Line, graphic_position, 10, graphic_position + 20, 30);

                            Chart chart = worksheet.GetCharts().Get(chartIndex);

                            U16String startCell = CellsHelper::CellIndexToName(1, max_column_to_graphic + 1);
                            U16String endCell = CellsHelper::CellIndexToName(excel_cell_row - 1, max_column_to_graphic + 1);
                            U16String range = startCell + u":" + endCell;
                            chart.GetNSeries().Add(range, true);
                            startCell = CellsHelper::CellIndexToName(1, max_column_to_graphic);
                            endCell = CellsHelper::CellIndexToName(excel_cell_row - 1, max_column_to_graphic);
                            range = startCell + u":" + endCell;
                            chart.GetNSeries().SetCategoryData(range);
                            string text_name = "График зависимости метрики '" + metr->first + "' от параметра '" + val->first + "'";
                            if (metrics_devided_by_types[metr->first][0]->fixed_main_parametrs_size() > 0) {
                                text_name += " при";
                                for (std::map<std::string, double>::iterator param1 = metrics_main_parametrs.begin(); param1 != metrics_main_parametrs.end(); ++param1) {
                                    if (param1->first != val->first)
                                    {
                                        text_name += " параметре '" + param1->first + "' = " + to_string(metrics_devided_by_types[metr->first][0]->get_fixed_main_parametr(param1->first));
                                    }
                                }
                            }
                            U16String text_text = text_name.c_str();
                            chart.GetTitle().SetText(text_text);

                            Axis xAxis = chart.GetCategoryAxis();
                            U16String axis_name = val->first.c_str();
                            xAxis.GetTitle().SetText(axis_name);

                            Axis yAxis = chart.GetValueAxis();
                            axis_name = metr->first.c_str();
                            yAxis.GetTitle().SetText(axis_name);

                            text_text = metr->first.c_str();
                            chart.GetNSeries().Get(0).SetType(Aspose::Cells::Charts::ChartType::Line);
                            chart.GetNSeries().Get(0).SetName(text_text);
                            chart.GetChartArea().GetArea().SetFormatting(FormattingType::Custom);
                            chart.GetChartArea().GetArea().SetForegroundColor(Color::White());


                            chart.GetPlotArea().GetArea().SetFormatting(FormattingType::Custom);
                            chart.GetPlotArea().GetArea().SetForegroundColor(Color::White());
                            max_column_to_graphic += 3;
                            graphic_position += 25;
                        }
                        if (values_for_graphics[val->first][0].size() > 2)
                        {
                            bool first_time_val_all = true;
                            bool first_time_val = true;
                            int move = 0;
                            int val_first_value = values_for_graphics[val->first][0][0];
                            int val_ind = 0;
                            string name = val->first;
                            string name1;
                            string name2;
                            vector<string> parts;
                            size_t pos = 0;
                            string token;
                            excel_cell_row = 0;
                            excel_cell_column = max_column_to_graphic;
                            if ((pos = name.find("|")) != std::string::npos)
                            {
                                token = name.substr(0, pos);
                                parts.push_back(token);
                                name.erase(0, pos + 1);
                            }
                            parts.push_back(name);
                            if (parts.size() > 1) {
                                name1 = parts[0];
                                name2 = parts[1];
                            }
                            names = name2.c_str();
                            worksheet.GetCells().Get(excel_cell_row, excel_cell_column).PutValue(names);
                            for (int val_ind = 0; val_ind < values_for_graphics[val->first].size(); val_ind++)
                            {
                                excel_cell_row = 0;
                                excel_cell_column = max_column_to_graphic+move;
                                while ((val_ind< values_for_graphics[val->first].size()) && (values_for_graphics[val->first][val_ind][0] == val_first_value))
                                {
                                    if (first_time_val)
                                    {
                                        names = (name1+"=" + to_string(val_first_value)).c_str();
                                        worksheet.GetCells().Get(excel_cell_row, excel_cell_column + 1).PutValue(names);
                                        first_time_val = false;
                                    }
                                    excel_cell_row++;
                                    if (first_time_val_all)
                                    {
                                        worksheet.GetCells().Get(excel_cell_row, excel_cell_column).PutValue(values_for_graphics[val->first][val_ind][1]);
                                    }
                                    worksheet.GetCells().Get(excel_cell_row, excel_cell_column + 1).PutValue(values_for_graphics[val->first][val_ind][2]);
                                    val_ind++;
                                }
                                if (val_ind < values_for_graphics[val->first].size())
                                {
                                    val_first_value = values_for_graphics[val->first][val_ind][0];
                                    first_time_val_all = false;
                                    first_time_val = true;
                                    move++;
                                }
                                val_ind--;
                            }
                            
                            int chartIndex = worksheet.GetCharts().Add(Aspose::Cells::Charts::ChartType::Line, graphic_position, 10, graphic_position + 20, 30);

                            Chart chart = worksheet.GetCharts().Get(chartIndex);
                            int nom = 0;
                            U16String startCell;
                            U16String endCell;
                            U16String range;
                            for (int col = max_column_to_graphic + 1; col < max_column_to_graphic + 2 + move; col++)
                            {
                                startCell = CellsHelper::CellIndexToName(1, col);
                                endCell = CellsHelper::CellIndexToName(excel_cell_row, col);
                                range = startCell + u":" + endCell;
                                chart.GetNSeries().Add(range, true);
                                chart.GetNSeries().Get(nom).SetType(Aspose::Cells::Charts::ChartType::Line);
                                startCell = CellsHelper::CellIndexToName(0, col);
                                //"=Sheet1!$B$1"
                                range = u"=" + startCell;
                                chart.GetNSeries().Get(nom).SetName(range);
                                nom++;
                            }
                            
                            startCell = CellsHelper::CellIndexToName(1, max_column_to_graphic);
                            endCell = CellsHelper::CellIndexToName(excel_cell_row, max_column_to_graphic);
                            range = startCell + u":" + endCell;
                            chart.GetNSeries().SetCategoryData(range);
                            string text_name = "График зависимости метрики '" + metr->first + "' от параметров '" +  name1 + "' и '"+name2+"'";
                            if (metrics_devided_by_types[metr->first][0]->fixed_main_parametrs_size() > 0) {
                                for (std::map<std::string, double>::iterator param1 = metrics_main_parametrs.begin(); param1 != metrics_main_parametrs.end(); ++param1) {
                                    if ((param1->first != name1) && (param1->first != name2))
                                    {
                                        text_name += " при параметре '" + param1->first + "' = " + to_string(metrics_devided_by_types[metr->first][0]->get_fixed_main_parametr(param1->first));
                                    }
                                }
                            }
                            U16String text_text = text_name.c_str();
                            chart.GetTitle().SetText(text_text);

                            Axis xAxis = chart.GetCategoryAxis();
                            U16String axis_name = name2.c_str();
                            xAxis.GetTitle().SetText(axis_name);

                            Axis yAxis = chart.GetValueAxis();
                            axis_name = metr->first.c_str();
                            yAxis.GetTitle().SetText(axis_name);
                            chart.GetChartArea().GetArea().SetFormatting(FormattingType::Custom);
                            chart.GetChartArea().GetArea().SetForegroundColor(Color::White());


                            chart.GetPlotArea().GetArea().SetFormatting(FormattingType::Custom);
                            chart.GetPlotArea().GetArea().SetForegroundColor(Color::White());
                            
                            max_column_to_graphic += (3+move);
                            graphic_position += 25;
                        }
                    }
                    

                    sheet_number++;
                }
            }
        }
        try {
            wkb->Save(excel_file_name);
            // Освобождаем объект Aspose
        }
        catch (CellsException& ex) {
            // Ловим исключение по ссылке

            // 1. Получить сообщение об ошибке
            
            U16String message = ex.GetErrorMessage();
            wstring wideStr(reinterpret_cast<const wchar_t*>(message.GetData()));
            std::wcout << wideStr << std::endl;
        }
        catch (...) {
            // Ловим все остальные типы ошибок
            std::wcout << L"Произошла неизвестная ошибка." << std::endl;
        }
        wkb = nullptr;
    }
};

int main()
{
    // Устанавливаем кодировку для русского языка
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    
    Startup();
    
    Simulation_system sys;
    sys.start_modeling();

    Cleanup();
    system("pause");
    return 0;
}