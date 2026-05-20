
#include "pch.h"
#define AREA_PARALLEL_EXPORTS
#define _USE_MATH_DEFINES
#include "Area_parallel.h"
#include "../Average_number_transfered_messages/Average_number_transfered_messages.h"
#include "../Average_number_transfered_messages_for_one_time/Average_number_transfered_messages_for_one_time.h"
#include <numeric>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <random>
#include <math.h>

#pragma comment(lib, "../x64/Debug/Metric.lib")
#pragma comment(lib, "../x64/Debug/Model.lib")
#pragma comment(lib, "../x64/Debug/Transfer.lib")
#pragma comment(lib, "../x64/Debug/Average_number_transfered_messages.lib")
#pragma comment(lib, "../x64/Debug/Average_number_transfered_messages_for_one_time.lib")
#pragma comment(lib, "../x64/Debug/Node.lib")
using namespace std;

//  онструкторы
Area_parallel::Area_parallel()
    : Model("Area", "Area_parallel") {
}

Area_parallel::Area_parallel(const std::string& model_name, const std::string& model_class)
    : Model(model_name, model_class) {
}

// ƒеструктор
Area_parallel::~Area_parallel() {

}

int Area_parallel::partition(vector<Node>& arr, int low, int high) {
    double pivot = arr[high].get_parametr("time");
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j].get_parametr("time") <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void Area_parallel::quickSort(vector<Node>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void Area_parallel::modeling_transfer(Transfer& transf, Node node1, Node node2, double simulation_time)
{
    transf.set_parametr("start_time", simulation_time);
    transf.set_parametr("time_m", node1.get_parametr("time"));
    transf.set_parametr("posx1", node1.get_parametr("posx"));
    transf.set_parametr("posx2", node2.get_parametr("posx"));
    transf.set_parametr("posy1", node1.get_parametr("posy"));
    transf.set_parametr("posy2", node2.get_parametr("posy"));
    transf.set_parametr("radius1", node1.get_parametr("radius"));
    transf.set_parametr("radius2", node2.get_parametr("radius"));
    transf.set_parametr("speed1", node1.get_parametr("speed"));
    transf.set_parametr("speed2", node2.get_parametr("speed"));
    transf.set_parametr("angle1", node1.get_parametr("angle"));
    transf.set_parametr("angle2", node2.get_parametr("angle"));

    transf.modeling_way();

    double temp = transf.get_parametr("posx1");
    transf.set_parametr("posx1", transf.get_parametr("posx2"));
    transf.set_parametr("posx2", temp);

    temp = transf.get_parametr("posy1");
    transf.set_parametr("posy1", transf.get_parametr("posy2"));
    transf.set_parametr("posy2", temp);

    temp = transf.get_parametr("radius1");
    transf.set_parametr("radius1", transf.get_parametr("radius2"));
    transf.set_parametr("radius2", temp);

    temp = transf.get_parametr("speed1");
    transf.set_parametr("speed1", transf.get_parametr("speed2"));
    transf.set_parametr("speed2", temp);

    temp = transf.get_parametr("angle1");
    transf.set_parametr("angle1", transf.get_parametr("angle2"));
    transf.set_parametr("angle2", temp);

    transf.modeling_way();
}

double Area_parallel::collecting_metrics() {
    return 0;
}

double Area_parallel::modeling_way() {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dist1(0, get_parametr("length"));
    uniform_real_distribution<double> dist2(0, get_parametr("width"));
    uniform_real_distribution<double> angle(0, get_parametr("max_angle"));
    uniform_real_distribution<double> speed(0, get_parametr("max_speed"));
    uniform_real_distribution<double> time_move(0, get_parametr("max_time_move") - 0.1);
    vector<vector<vector<vector<Node>>>> all_nodes;
    //vector<Dynamic_Geom_Graph*> graphs;
    //for (double i = get_parametr("start_number"); i <= get_parametr("max_number"); i += get_parametr("step_number"))
    //{
        //vector<vector<vector<Node>>> all_nodes_radius;
       // for (double j = get_parametr("start_radius"); j <= get_parametr("max_radius"); j += get_parametr("step_radius"))
        //{
            double score = get_parametr("simulation_number");
            int number_transfered_messages_radius = 0;
            int number_attempt_radius = 0;
            Transfer* trans = new Transfer("transfer_" + to_string(get_parametr("number")) + "_" + to_string(get_parametr("radius")), "moving_model");
            Average_number_transfered_messages* metric1 = new Average_number_transfered_messages("average_number_transfered_messages_" + to_string(get_parametr("number")) + "_" + to_string(get_parametr("radius")), "average_number_transfered_messages");
            Average_number_transfered_messages_for_one_time* metric2 = new Average_number_transfered_messages_for_one_time("average_number_transfered_messages_for_one_time_" + to_string(get_parametr("number")) + "_" + to_string(get_parametr("radius")), "average_number_transfered_messages_for_one_time");
            (*metric1).add_main_parametr("количество узлов", get_parametr("number"));
            (*metric1).add_main_parametr("радиус", get_parametr("radius"));
            (*metric1).add_parametr("number_nodes", get_parametr("number"));
            (*metric1).add_fixed_main_parametr("количество узлов", get_parametr("fixed_number"));
            (*metric1).add_fixed_main_parametr("радиус", get_parametr("fixed_radius"));

            (*metric2).add_main_parametr("количество узлов", get_parametr("number"));
            (*metric2).add_main_parametr("радиус", get_parametr("radius"));
            (*metric2).add_parametr("number_transfer", 0);
            (*metric2).add_fixed_main_parametr("количество узлов", get_parametr("fixed_number"));
            (*metric2).add_fixed_main_parametr("радиус", get_parametr("fixed_radius"));

            (*metric1).set_value(0);
            (*metric2).set_value(0);

            (*trans).add_metric(metric1);
            (*trans).add_metric(metric2);
            while (score > 0)
            {
                double simulation_time = 0;
                //graphs.resize(0);
                int kolvo_svyaznih_component = 0;
                vector <Node> nodes;
                for (int k = 1; k <= get_parametr("number"); k++)
                {
                    Node node("node_" + nodes.size(), "model_node");
                    node.set_parametr("connected", 0);
                    node.set_parametr("radius", get_parametr("radius"));
                    node.set_parametr("nomer", nodes.size());

                    node.set_parametr("time", 0);
                    bool flag_stop = false;
                    while (flag_stop == false) {
                        flag_stop = true;
                        node.set_parametr("posx", dist1(gen));
                        node.set_parametr("posy", dist2(gen));

                        for (int l = 0; l < nodes.size(); l++)
                        {
                            if (nodes[l].get_parametr("posx") == node.get_parametr("posx") && nodes[l].get_parametr("posy") == node.get_parametr("posy"))
                            {
                                flag_stop = false;
                                break;
                            }
                        }
                    }
                    nodes.push_back(node);
                }
                vector<int>checked_nodes;
                //Dynamic_Geom_Graph graphh = Dynamic_Geom_Graph("dynamic_geom_graph");
                /*while (checked_nodes.size() < nodes.size())
                {
                    for (int start = 0; start < nodes.size(); start++)
                    {
                        if (nodes[start].get_parametr("connected") == false)
                        {
                            vector<int> graph;
                            graph.resize(0);


                           // graphh.create_graph(nodes[start], &nodes, &checked_nodes, &graph);

                        }
                    }
                }*/
                double time_now = simulation_time;
                vector<int> min_nomer;
                min_nomer.resize(0);
                for (int start = 0; start < nodes.size(); start++)
                {
                    nodes[start].set_parametr("angle", int(angle(gen)));
                    nodes[start].set_parametr("speed", speed(gen));
                    double time_next_stop = time_move(gen) + 0.1;
                    nodes[start].set_parametr("time", time_next_stop);
                    nodes[start].set_parametr("max_time_move", get_parametr("max_time_move"));
                    nodes[start].set_parametr("max_speed", get_parametr("max_speed"));
                    nodes[start].set_parametr("max_angle", get_parametr("max_angle"));
                }
                quickSort(nodes, 0, nodes.size() - 1);
                for (int ind1 = 0; ind1 < nodes.size() - 1; ind1++)
                {
                    for (int ind2 = ind1 + 1; ind2 < nodes.size(); ind2++)
                    {
                        modeling_transfer((*trans), nodes[ind1], nodes[ind2], simulation_time);
                    }
                }
                while (simulation_time < get_parametr("time")) {
                    simulation_time = nodes[0].get_parametr("time");
                    int ind = 0;
                    double dif_time = nodes[0].get_parametr("time") - nodes[0].get_parametr("last_time");
                    vector<Node> nodes_to_transfer;
                    while (ind < nodes.size() && nodes[ind].get_parametr("time") == simulation_time)
                    {
                        nodes[ind].set_parametr("dif_time", dif_time);
                        nodes[ind].modeling_way();
                        nodes_to_transfer.push_back(nodes[ind]);
                        ind++;
                    }
                    quickSort(nodes, 0, nodes.size() - 1);
                    for (int ind1 = 0; ind1 < nodes_to_transfer.size(); ind1++)
                    {
                        for (int ind2 = 0; ind2 < nodes.size(); ind2++)
                        {
                            if (nodes[ind2].get_parametr("nomer") != nodes_to_transfer[ind1].get_parametr("nomer")) {
                                modeling_transfer((*trans), nodes_to_transfer[ind1], nodes[ind2], simulation_time);
                            }
                        }
                    }
                }
                score--;
            }
            add_component(trans->get_name(), trans);
       // }
    //}
    return 0;
}

double Area_parallel::demonstrate_model() {

    cout << "\n=== ƒемонстраци€ работы с моделью области ===\n";

    // ѕечатаем им€
    cout << "»м€ модели (print_name): ";
    print_name();
    cout << " ласс модели (print_type): ";
    print_class();

    double length_demo, width_demo, max_angle_demo, max_speed_demo, max_time_move_demo, start_number_demo, number_demo, step_number_demo, start_radius_demo, max_radius_demo, step_radius_demo, simulation_number_demo, time_demo;

    cout << "¬ведите значение параметра длины области:";
    cin >> length_demo;
    set_parametr("length", length_demo);

    cout << "¬ведите значение параметра ширины области:";
    cin >> width_demo;
    set_parametr("width", width_demo);

    cout << "¬ведите значение максимального угла поворота:";
    cin >> max_angle_demo;
    set_parametr("max_angle", max_angle_demo);

    cout << "¬ведите значение максимальной скорости движени€:";
    cin >> max_speed_demo;
    set_parametr("max_speed", max_speed_demo);

    cout << "¬ведите значение максимального времени непрерывного движени€:";
    cin >> max_time_move_demo;
    set_parametr("max_time_move", max_time_move_demo);

    cout << "¬ведите значение начального количества узлов сети:";
    cin >> start_number_demo;
    set_parametr("start_number", start_number_demo);

    cout << "¬ведите значение максимального количества узлов сети:";
    cin >> number_demo;
    set_parametr("number", number_demo);

    cout << "¬ведите значение шага изменени€ количества узлов сети:";
    cin >> step_number_demo;
    set_parametr("step_number", step_number_demo);

    cout << "¬ведите значение начальной длины радиуса приЄма/передачи узлов сети:";
    cin >> start_radius_demo;
    set_parametr("start_radius", start_radius_demo);

    cout << "¬ведите значение максимальной длины радиуса приЄма/передачи узлов сети:";
    cin >> max_radius_demo;
    set_parametr("max_radius", max_radius_demo);

    cout << "¬ведите значение шага изменени€ количества длины радиуса приЄма/передачи узлов сети:";
    cin >> step_radius_demo;
    set_parametr("step_radius", step_radius_demo);

    cout << "¬ведите количество симул€ций на каждой итерации:";
    cin >> simulation_number_demo;
    set_parametr("simulation_number", simulation_number_demo);

    cout << "¬ведите предел модельного времени дл€ одной симул€ции:";
    cin >> time_demo;
    set_parametr("time", time_demo);

    modeling_way();
    metrics_output();

    return 0;

}

extern "C" {

    __declspec(dllexport) void* CreateModel(const char* model_name, const char* model_class) {
        try {
            if (model_name && strlen(model_name) > 0 && model_class && strlen(model_class) > 0) {
                return new Area_parallel(model_name, model_class);
            }
            else {
                return new Area_parallel();
            }
        }
        catch (...) {
            return nullptr;
        }
    }

    __declspec(dllexport) void DestroyModel(void* model) {
        if (model) {
            delete static_cast<Area_parallel*>(model);
        }
    }

    __declspec(dllexport) double ModelingWay(void* model) {
        if (model) {
            return static_cast<Area_parallel*>(model)->modeling_way();
        }
        return 0.0;
    }

    __declspec(dllexport) double CollectingMetric(void* model) {
        if (model) {
            return static_cast<Area_parallel*>(model)->collecting_metrics();
        }
        return 0.0;
    }

    __declspec(dllexport) string GetNextModelName(void* model) {
        if (model) {
            return static_cast<Area_parallel*>(model)->get_next_model_name();
        }
        else
            return "";
    }

    __declspec(dllexport) void AddComponent(void* model, const char* model_name, Model* current_model) {
        if (model) {
            static_cast<Area_parallel*>(model)->add_component(model_name, current_model);
        }
    }

    __declspec(dllexport) Model* GetComponent(void* model, const char* model_name) {
        if (model) {
            return static_cast<Area_parallel*>(model)->get_component(model_name);
        }
        else
            return nullptr;
    }

    __declspec(dllexport) int ComponentsSize(void* model) {
        if (model) {
            return static_cast<Area_parallel*>(model)->components_size();
        }
        return 0;
    }

    __declspec(dllexport) void AddMetric(void* model, Metric* metric) {
        if (model) {
            static_cast<Area_parallel*>(model)->add_metric(metric);
        }
    }

    __declspec(dllexport) Metric* GetMetric(void* model, int metric_nomer) {
        if (model) {
            return static_cast<Area_parallel*>(model)->get_metric(metric_nomer);
        }
        return nullptr;
    }

    __declspec(dllexport) void AddComponentModelAdress(void* model, const char* model_name, string model_adress) {
        if (model) {
            static_cast<Area_parallel*>(model)->add_component_model_adress(model_name, model_adress);
        }
    }

    __declspec(dllexport) string GetComponentModelAdress(void* model, const char* model_name) {
        if (model) {
            return static_cast<Area_parallel*>(model)->get_component_model_adress(model_name);
        }
        else
            return "";
    }

    __declspec(dllexport) int MetricsSize(void* model) {
        if (model) {
            return static_cast<Area_parallel*>(model)->metrics_size();
        }
        return 0;
    }

    __declspec(dllexport) int AllComponentsMetricsSize(void* model) {
        if (model) {
            return static_cast<Area_parallel*>(model)->all_components_metrics_size();
        }
        return 0;
    }

    __declspec(dllexport) void SetParametr(void* model, const char* param_name, double param_value) {
        if (model && param_name && param_value) {
            static_cast<Area_parallel*>(model)->set_parametr(param_name, param_value);
        }
    }

    __declspec(dllexport) double GetParametr(void* model, const char* param_name) {
        if (model && param_name) {
            return static_cast<Area_parallel*>(model)->get_parametr(param_name);
        }
        return 0.0;
    }

    __declspec(dllexport) void SetParallelParametr(void* model, const char* param_name, vector<double> param_value) {
        if (model && param_name && param_value.size() == 3) {
            static_cast<Area_parallel*>(model)->set_parallel_parametr(param_name, param_value);
        }
    }

    __declspec(dllexport) vector<double>* GetParallelParametr(void* model, const char* param_name) {
        if (model && param_name) {
            return static_cast<Area_parallel*>(model)->get_parallel_parametr(param_name);
        }
        else {
            vector<double>* empty=nullptr;
            return empty;
        }
    }

    __declspec(dllexport) int ParallelParametrsSize(void* model) {
        if (model) {
            return  static_cast<Area_parallel*>(model)->parallel_parametrs_size();
        }
        return 0;
    }

    __declspec(dllexport) void PrintName(void* model) {
        if (model) {
            static_cast<Area_parallel*>(model)->print_name();
        }
    }

    __declspec(dllexport) void PrintClass(void* model) {
        if (model) {
            static_cast<Area_parallel*>(model)->print_class();
        }
    }

    __declspec(dllexport) const char* GetName(void* model) {
        if (model) {
            static std::string name;
            name = static_cast<Area_parallel*>(model)->get_name();
            return name.c_str();
        }
        return "";
    }

    __declspec(dllexport) void MetricsOutput(void* model) {
        if (model) {
            static_cast<Area_parallel*>(model)->metrics_output();
        }
    }

    __declspec(dllexport) void DemonstrateModel(void* model) {
        if (model) {
            static_cast<Area_parallel*>(model)->demonstrate_model();
        }
    }

    __declspec(dllexport) vector <Metric*> GetAllComponentsMetrics(void* model) {
        if (model) {
            return static_cast<Area_parallel*>(model)->get_all_components_metrics();
        }
        else {
            vector <Metric*> empty;
            empty.resize(0);
            return empty;
        }
    }

    __declspec(dllexport) map <string, double> GetAllParametrs(void* model) {
        if (model) {
            return static_cast<Area_parallel*>(model)->get_all_parametrs();
        }
        else {
            map <string, double> empty;
            return empty;
        }
    }

    __declspec(dllexport) map <string, vector<double>> GetAllParallelParametrs(void* model) {
        if (model) {
            return static_cast<Area_parallel*>(model)->get_all_parallel_parametrs();
        }
        else {
            map <string, vector<double>> empty;
            return empty;
        }
    }

} // extern "C"