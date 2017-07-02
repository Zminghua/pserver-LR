#include "ps/ps.h"
#include "cmath"
#include "lr.h"
#include "util.h"
#include "sample.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>


namespace pslr {

LR::LR(uint64_t num_feature_dim, int sync_mode, float C, int random_state)
                    : num_feature_dim_(num_feature_dim), sync_mode_(sync_mode), C_(C), random_state_(random_state) {
   
    keys.resize(num_feature_dim_);
    for (uint64_t i = 0; i < num_feature_dim_; ++i) {
        keys[i] = pslr::ReverseBytes(i+1);
        //keys[i] = i+1;
    }
    sort(keys.begin(), keys.end());

    srand(random_state_);
    weight.resize(num_feature_dim_);
    for (uint64_t i = 0; i < num_feature_dim_; ++i) {
        weight[i] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }

    epoch = 0;
    batch = 0;
}

void LR::InitServer() {
    PushGradient_(weight);
}

void LR::SetKVWorker(ps::KVWorker<float>* kv_) {
    kv = kv_;
}

void LR::Train(DataIter& iter, int batch_size) {
    update_epoch();
    while (iter.HasNext()) {
        std::vector<Sample> batch = iter.NextBatch(batch_size);

        update_batch();
        pslr::Time();
        std::cout<<" Worker[" <<ps::MyRank() <<"]:train " <<"Epoch:" <<get_epoch() 
                                                        <<" Batch:" <<get_batch() <<std::endl;

        PullWeight_();
        std::vector<float> grad(num_feature_dim_);
        for (uint64_t j = 0; j < num_feature_dim_; ++j) {
            grad[j] = 0;
            for (size_t i = 0; i < batch.size(); ++i) {
                auto& sample = batch[i];
                grad[j] += (Sigmoid_(sample.GetFeature()) - sample.GetLabel()) * sample.GetFeature(j);
            }
            grad[j] = 1. * grad[j] / batch.size() + C_ * weight[j] / batch.size();
        }
        PushGradient_(grad);

        if  (sync_mode_ == 1) {
            ps::Postoffice::Get()->Barrier(ps::kWorkerGroup);
        }
    }
}

void LR::Test(DataIter& iter) {
    PullWeight_();
    std::vector<Sample> batch = iter.NextBatch(-1);
    
    metric.clear();
    for (size_t i = 0; i < batch.size(); ++i) {
        auto& sample = batch[i];
        float score = Sigmoid_(sample.GetFeature());
        int label = score>0.5 ? 1 : 0;
        metric.add(score, label, sample.GetLabel());
    }
    
    pslr::Time();
    std::cout<<" Worker[" <<ps::MyRank() <<"]:test " <<"Epoch:" <<get_epoch() <<" ";
    metric.show();
}

void LR::Predict(DataIter& iter) {
    PullWeight_();
    std::vector<Sample> batch = iter.NextBatch(-1);
    
    std::ofstream cout(iter.getFile()+".pre", std::ios::out);
    metric.clear();
    for (size_t i = 0; i < batch.size(); ++i) {
        auto& sample = batch[i];
        float score = Sigmoid_(sample.GetFeature());
        int label = score>0.5 ? 1 : 0;
        cout<<sample.GetLabel() <<"\t" <<label <<"\t" <<score <<"\n";
        metric.add(score, label, sample.GetLabel());
    }
    cout.close();
    
    pslr::Time();
    std::cout<<" Worker[" <<ps::MyRank() <<"]:test " <<"Epoch:" <<get_epoch() <<" ";
    metric.show();
}

std::vector<float> LR::GetWeight() {
    return weight;
}

ps::KVWorker<float>* LR::GetKVWorker() {
    return kv;
}

void LR::update_epoch() {
    epoch = epoch+1;
}

void LR::update_batch() {
    batch = batch+1;
}

int LR::get_epoch() {
    return epoch;
}

int LR::get_batch() {
    return batch;
}

bool LR::SaveModel(std::string& filename) {
    PullWeight_();
    
    std::ofstream fout(filename.c_str());
    fout << num_feature_dim_ << std::endl;
    for (uint64_t i = 0; i < num_feature_dim_; ++i) {
        fout << weight[i] << ' ';
    }
    fout << std::endl;
    fout.close();
    return true;
}

std::string LR::DebugInfo() {
    std::ostringstream out;
    for (size_t i = 0; i < weight.size(); ++i) {
        out << weight[i] << " ";
    }
    return out.str();
}

int LR::Predict_(std::vector<float> feature) {
    float z = 0;
    for (size_t j = 0; j < num_feature_dim_; ++j) {
        z += weight[j] * feature[j];
    }
    return z > 0;
}

float LR::Sigmoid_(std::vector<float> feature) {
    float z = 0;
    for (size_t j = 0; j < num_feature_dim_; ++j) {
        z += weight[j] * feature[j];
    }
    return 1. / (1. + exp(-z));
}

void LR::PullWeight_() {
    //std::vector<float> vals;
    kv->Wait(kv->Pull(keys, &weight));
    //weight = vals;
}

void LR::PushGradient_(const std::vector<float>& grad) {
    kv->Wait(kv->Push(keys, grad));
}

} // namespace pslr


