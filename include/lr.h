#ifndef PSLR_LR_H_
#define PSLR_LR_H_

#include "metric.h"
#include "data_iter.h"


namespace pslr {

class LR {
    public:
        explicit LR(uint64_t num_feature_dim, int sync_mode, float C_=1, int random_state=0);
  
        virtual ~LR() {
            if (kv) {
                delete kv;
            }
        }
        
        void InitServer();
        void SetKVWorker(ps::KVWorker<float>* kv_);

        void Train(pslr::DataIter& iter, int batch_size);
        void Test(pslr::DataIter& iter);
        void Predict(pslr::DataIter& iter);

        std::vector<float> GetWeight();
        ps::KVWorker<float>* GetKVWorker();

        void update_epoch();
        void update_batch();
        int get_epoch();
        int get_batch();

        bool SaveModel(std::string& filename);
        std::string DebugInfo();

    private:
        int Predict_(std::vector<float> feature);
        float Sigmoid_(std::vector<float> feature);

        void PullWeight_();
        void PushGradient_(const std::vector<float>& grad);

        uint64_t num_feature_dim_;
        int sync_mode_;
        float C_;

        int random_state_;

        std::vector<ps::Key> keys;
        std::vector<float> weight;
        ps::KVWorker<float>* kv;

        int epoch;
        int batch;

        pslr::Metric metric;

}; // class LR

}  // namespace pslr

#endif  // PSLR_LR_H_


