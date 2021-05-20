#pragma once
#include <libprecompiled/Common.h>
#include <libnlohmann_json/single_include/nlohmann/json.hpp>
#define OUTPUT 1

// the setting of model
#define n_features 5
#define n_class 2

// the number of committee
#define COMM_COUNT 4
// the number of clients which to be aggregated
#define AGGREGATE_COUNT 6
// the number of needed update
#define NEEDED_UPDATE_COUNT 10
// the number of clients which have been register
#define CLIENT_NUM 20
// learning rate
#define learning_rate 0.001 

using json=nlohmann::json;

// global model or local model
struct Model
{
	/* model data */
	std::vector<std::vector<float>> ser_W;
	std::vector<float> ser_b;

	/* constructor function */
	Model(){
		ser_W = std::vector<std::vector<float>>(n_features,std::vector<float>(n_class,0));
		ser_b = std::vector<float>(n_class,0);
	}

	Model(const json & j){
        ser_W = j["ser_W"].get<std::vector<std::vector<float>>>();
        ser_b = j["ser_b"].get<std::vector<float>>();
    }

    void operator=(const json & j){
        ser_W = j["ser_W"].get<std::vector<std::vector<float>>>();
        ser_b = j["ser_b"].get<std::vector<float>>();
    }

    std::string to_json_string(){
        json j;
        j["ser_W"] = ser_W;
        j["ser_b"] = ser_b;
        return j.dump();
    }
};
// the meta of local model
struct Meta
{
	/* meta data */
	int n_samples;
	float avg_cost;

	/* constructor function */ 
	Meta(int n = 0, float cost = 0) : n_samples(n), avg_cost(cost){}

	Meta(const json & j){
        n_samples = j["n_samples"].get<int>();
        avg_cost = j["avg_cost"].get<float>();
    }

    void operator=(const json & j){
        n_samples = j["n_samples"].get<int>();
        avg_cost = j["avg_cost"].get<float>();
    }

    std::string to_json_string(){
        json j;
        j["n_samples"] = n_samples;
        j["avg_cost"] = avg_cost;
        return j.dump();
    }
};

// the update of local model
struct LocalUpdate
{
	/* update data */
	Model delta_model;
	Meta meta;

	/* constructor function */
	LocalUpdate(){}

	LocalUpdate(const json & j){
        delta_model = j["delta_model"];
        meta = j["meta"];
    }

    void operator=(const json & j){
        delta_model = j["delta_model"];
        meta = j["meta"];
    }

    std::string to_json_string(){
        json j;
        j["delta_model"] = delta_model.to_json_string();
        j["meta"] = meta.to_json_string();
        return j.dump();
    }
};

namespace dev
{
	namespace precompiled
	{

		class CommitteePrecompiled : public dev::precompiled::Precompiled
		{
		public:
			typedef std::shared_ptr<CommitteePrecompiled> Ptr;  //指针
			CommitteePrecompiled();   //构造
			virtual ~CommitteePrecompiled(){};   //析构
			
			PrecompiledExecResult::Ptr call(std::shared_ptr<dev::blockverifier::ExecutiveContext> _context,
					bytesConstRef _param, Address const& _origin = Address(),
					Address const& _sender = Address()) override;     //重载call函数

		private:
			void InitGlobalModel(storage::Table::Ptr table, Address const& _origin, PrecompiledExecResult::Ptr callResult);
			void Aggregate(storage::Table::Ptr table, Address const& _origin, PrecompiledExecResult::Ptr callResult, std::unordered_map<std::string, std::string>& local_scores);
			void InsertVariable(storage::Table::Ptr table, Address const& _origin, PrecompiledExecResult::Ptr callResult, const std::string & Key, std::string & strValue);
			std::string GetVariable(storage::Table::Ptr table, Address const& _origin, PrecompiledExecResult::Ptr callResult, const std::string & Key);
			void UpdateVariable(storage::Table::Ptr table, Address const& _origin, PrecompiledExecResult::Ptr callResult, const std::string & Key, std::string & strValue);
		};
	}  // namespace precompiled
}  // namespace dev