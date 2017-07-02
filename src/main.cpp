#include <iostream>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include "ps/ps.h"

#include "lr.h"
#include "util.h"
#include "data_iter.h"


template <typename Val>
class KVStoreServer {
    public:
        KVStoreServer() {
            using namespace std::placeholders;
            ps_server_ = new ps::KVServer<float>(0);
            ps_server_->set_request_handle(
                        std::bind(&KVStoreServer::DataHandle, this, _1, _2, _3));

            sync_mode = pslr::ToInt(ps::Environment::Get()->find("SYNC_MODE"));
            learning_rate = pslr::ToFloat(ps::Environment::Get()->find("LEARNING_RATE"));
        }

        ~KVStoreServer() {
            if (ps_server_) {
                delete ps_server_;
            }
        }

    private:
        void DataHandle(const ps::KVMeta& req_meta,
                        const ps::KVPairs<Val>& req_data,
                        ps::KVServer<Val>* server) {

            size_t n = req_data.keys.size();
            if (req_meta.push) {
                CHECK_EQ(n, req_data.vals.size());
                
                if (weights.empty()) {
                    auto kr = ps::Postoffice::Get()->GetServerKeyRanges()[ps::MyRank()];
                    pslr::Time();
                    std::cout<<" Server[" <<ps::MyRank() <<"]:init " <<"Mode:" <<(sync_mode==1 ? "sync" : "async") 
                                                                <<" pnum:" <<n <<" IDrange:" <<kr.size() <<std::endl;
                    for (size_t i = 0; i < n; ++i) {
                        ps::Key key = req_data.keys[i];
                        weights[key] = req_data.vals[i];
                    }
                    server->Response(req_meta);
                }
                else{
                    pslr::Time();
                    std::cout<<" Server[" <<ps::MyRank() <<"]:push " <<"Worker:" <<ps::Postoffice::IDtoRank(req_meta.sender) <<std::endl;
                    if (sync_mode==1) {
                        for (size_t i = 0; i < n; ++i) {
                            ps::Key key = req_data.keys[i];
                            merge_buf.vals[key] += req_data.vals[i];
                        }

                        merge_buf.reqs.push_back(req_meta);
                        if (merge_buf.reqs.size() == (size_t)ps::NumWorkers()) {
                            for (size_t i = 0; i < n; ++i) {
                                ps::Key key = req_data.keys[i];
                                weights[key] -= learning_rate * merge_buf.vals[key] / merge_buf.reqs.size();
                            }
                            for (const auto& req : merge_buf.reqs) {
                                server->Response(req);
                            }
                            merge_buf.reqs.clear();
                            merge_buf.vals.clear();
                        }
                    }
                    else {
                        for (size_t i = 0; i < n; ++i) {
                            ps::Key key = req_data.keys[i];
                            weights[key] -= learning_rate * req_data.vals[i];
                        }
                        server->Response(req_meta);
                    }
                }
            }
            else {
                CHECK(!weights.empty()) << "init first" << std::endl;

                pslr::Time();
                std::cout<<" Server[" <<ps::MyRank() <<"]:pull " <<"Worker:" <<ps::Postoffice::IDtoRank(req_meta.sender) <<std::endl;
                ps::KVPairs<Val> response;
                response.keys = req_data.keys;
                response.vals.resize(n);
                for (size_t i = 0; i < n; ++i) {
                    ps::Key key = req_data.keys[i];
                    response.vals[i] = weights[key];
                }
                server->Response(req_meta, response);
            }
        }

        struct MergeBuf {
            std::vector<ps::KVMeta> reqs;
            std::unordered_map<ps::Key, Val> vals;
        };

        int sync_mode;
        float learning_rate;
        ps::KVServer<float>* ps_server_;
        std::unordered_map<ps::Key, Val> weights;
        MergeBuf merge_buf;
};

void StartServer() {
    if (!ps::IsServer()) {
        return;
    }
    auto server = new KVStoreServer<float>();
    ps::RegisterExitCallback([server](){ delete server; });
}

void RunWorker(){
    if (!ps::IsWorker()){
        return;
    }

    std::string root = ps::Environment::Get()->find("DATA_DIR");
    std::string testfile = root + "/test/part-001";
    int num_feature_dim = pslr::ToInt(ps::Environment::Get()->find("NUM_FEATURE_DIM"));
    int sync_mode = pslr::ToInt(ps::Environment::Get()->find("SYNC_MODE"));

    int rank = ps::MyRank();
    ps::KVWorker<float>* kv = new ps::KVWorker<float>(0);
    pslr::LR lr = pslr::LR(static_cast<uint64_t>(num_feature_dim), sync_mode);
    lr.SetKVWorker(kv);

    if (rank == 0) {
        lr.InitServer();
    }
    ps::Postoffice::Get()->Barrier(ps::kWorkerGroup);

    pslr::Time();
    std::cout<<" Worker[" <<rank <<"]:Start" <<std::endl;
    int num_epoch = pslr::ToInt(ps::Environment::Get()->find("NUM_EPOCH"));
    int batch_size = pslr::ToInt(ps::Environment::Get()->find("BATCH_SIZE"));
    int test_freq = pslr::ToInt(ps::Environment::Get()->find("TEST_FREQ"));

    //num_epoch = 0;
    for (int i = 0; i < num_epoch; ++i) {
        std::string filename = root + "/train/part-00" + std::to_string(rank + 1);
        pslr::DataIter iter(filename, num_feature_dim);
        lr.Train(iter, batch_size);

        if (rank == 0 and (i + 1) % test_freq == 0) {
            pslr::DataIter test_iter(testfile, num_feature_dim);
            lr.Test(test_iter);
        }
    }
    
    ps::Postoffice::Get()->Barrier(ps::kWorkerGroup);
    if (rank == 0) {
        pslr::DataIter test_iter(testfile, num_feature_dim);
        lr.Predict(test_iter);
        std::string modelfile = root + "/models/pslr";
        lr.SaveModel(modelfile);
    }

}

int main() {
    StartServer();
    ps::Start();
    RunWorker();
    ps::Finalize();
    return 0;
}


