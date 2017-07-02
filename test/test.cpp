#include "ps/ps.h"
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;
using namespace ps;


int ToInt(const char* str) {
    int flag = 1, ret = 0;
    const char* p = str;

    if (*p == '-') {
        ++p;
        flag = -1;
    } else if (*p == '+') {
        ++p;
    }

    while (*p) {
        ret = ret * 10 + (*p - '0');
        ++p;
    }
    return flag * ret;
}

int ToInt(const std::string& str) {
    return ToInt(str.c_str());
}

inline uint64_t ReverseBytes(uint64_t x) {
  	x = x << 32 | x >> 32;
  	x = (x & 0x0000FFFF0000FFFFULL) << 16 | (x & 0xFFFF0000FFFF0000ULL) >> 16;
  	x = (x & 0x00FF00FF00FF00FFULL) << 8  | (x & 0xFF00FF00FF00FF00ULL) >> 8;
  	x = (x & 0x0F0F0F0F0F0F0F0FULL) << 4  | (x & 0xF0F0F0F0F0F0F0F0ULL) >> 4;
  	return x;
}


template <typename Val>
struct KVServerDefaultHandle1 {
    void operator()(const KVMeta& req_meta, const KVPairs<Val>& req_data, KVServer<Val>* server) 
    {
        size_t n = req_data.keys.size();
        KVPairs<Val> res;
        if (req_meta.push){
            CHECK_EQ(n, req_data.vals.size());
        }
        else{
            res.keys = req_data.keys; res.vals.resize(n);
        }

        for (size_t i = 0; i < n; ++i)
        {
            Key key = req_data.keys[i];
            if (req_meta.push){
                store[key] += req_data.vals[i];
            }
            else{
                //cout<<"pull Key: " <<key <<endl;
                res.vals[i] = store[key];
            }
        }
        if (!req_meta.push){
            auto kr = ps::Postoffice::Get()->GetServerKeyRanges()[ps::MyRank()];
            cout<<"rank:" <<ps::MyRank() <<" para:" <<n <<" range:" <<kr.size() <<endl;
        }
        server->Response(req_meta, res);
    }
    std::unordered_map<Key, Val> store;
};

void StartServer() {
    if (!IsServer())
        return;
    
    cout<<"启动Server\n";
    auto server = new KVServer<float>(0);
    server->set_request_handle(KVServerDefaultHandle1<float>());
    RegisterExitCallback([server](){ delete server; });
}

void RunWorker() {
    if (!IsWorker()) 
        return;

    cout<<"启动Worker rank = " << MyRank() << "\n";
    KVWorker<float> kv(0);

    // init
    uint64_t num = ToInt(ps::Environment::Get()->find("NUM_FEATURE_DIM"));;
    std::vector<Key> keys(num);
    std::vector<float> vals(num);

    for (uint64_t i= 0; i < num; ++i) {
        keys[i] = ReverseBytes(i+1);
        vals[i] = i;
    }
    sort(keys.begin(), keys.end());

    // push
    kv.Wait( kv.Push(keys, vals) );
    
    // pull
    std::vector<float> rets;
    kv.Wait(kv.Pull(keys, &rets));
    for(size_t i = 0; i < rets.size(); i++)
    {
        //cout<< " rets[" << i << "]" << rets[i];
    }

    cout<<"结束"<<MyRank()<<"\n";
}

int main(int argc, char *argv[]) {
    // setup server nodes
    StartServer();
    // start system
    Start();
    // run worker nodes
    RunWorker();
    // stop system

    Finalize();
    return 0;
}


