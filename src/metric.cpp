#include "metric.h"
#include <iostream>
#include <ctime>
#include <cmath>
#include <cstring>
#include <algorithm>


namespace pslr{

using namespace std;

void Metric::clear(){
    pre_list.clear();
    memset(q_dist,0,sizeof(q_dist));
    memset(qq_plot,0,sizeof(qq_plot));
    total = 0;
    pos_num = 0;
    neg_num = 0;
    mae = 0.0;
    mse = 0.0;
    p_click = 0.0;
    logloss = 0.0;
}

void Metric::add(double score, int pred, int label){
    ins_pair ins = {label,pred,score};
    p_click = p_click + ins.score;
    int slot = (int)(ins.score * SEG);
    q_dist[slot] = q_dist[slot] + 1;
    qq_plot[slot][0] = qq_plot[slot][0] + 1;
    if(ins.label != 1)
    {
        ins.label = 0;
        neg_num = neg_num + 1;
        mae = mae + ins.score;
        mse = mse + ins.score * ins.score;
        logloss = logloss + log(1.0 - ins.score);
    }
    else
    {
        pos_num = pos_num + 1;
        mae = mae + 1.0 - ins.score;
        mse = mse + (1.0 - ins.score) * (1.0 - ins.score);
        qq_plot[slot][1] = qq_plot[slot][1] + 1;
        logloss = logloss + log(ins.score);
    }
    pre_list.push_back(ins);
}

long double Metric::get_auc(){
    sort(pre_list.begin(),pre_list.end());
    long long index = 0;
    long double total_score = 0.0;
    
    vector<ins_pair>::iterator it;
    for(it=pre_list.begin();it!=pre_list.end();it++)
    {
        if((*it).label==1)
        {
            total_score = total_score + total - index;
        }
        index += 1;
    }
    total_score = total_score - pos_num * (pos_num + 1.0) / 2.0;
    auc = total_score / (pos_num * neg_num);
    return auc;
}

std::vector<long double> Metric::get_PRF(){
    double a[2][2]={{0.0, 0.0},{0.0, 0.0}};
    
    vector<ins_pair>::iterator it;
    for(it=pre_list.begin();it!=pre_list.end();it++)
    {
        a[(*it).label][(*it).pred] += 1.0;
    }
    
    std::vector<long double> PRF;
    long double P = a[1][1]/(a[1][1]+a[0][1]);
    long double R = a[1][1]/(a[1][1]+a[1][0]);
    long double F = 2*P*R/(P+R);
    PRF.push_back(P);
    PRF.push_back(R);
    PRF.push_back(F);
    
    return PRF;
}

void Metric::show(){

    total = neg_num + pos_num;
    pos_rate = (0.0 + pos_num) / total;
    mae = mae / total;
    mse = mse / total;
    std::vector<long double> PRF = get_PRF();

    cout<<"MAE:" <<mae<<" MSE:" <<mse <<" AUC:" <<get_auc();
    cout<<"  P:" <<PRF[0] <<" R:" <<PRF[1] <<" F:" <<PRF[2];
    cout<<"  pos:" <<pos_num <<" neg:" <<neg_num <<" pos_rate:" <<pos_rate <<endl;
    
    //copc = pos_num / p_click;
    //cout<<"COPC is "<<copc<<endl;
    //logloss = -logloss / total;
    //cout<<"Logloss is "<<logloss<<endl;
    //backgroundLoss = pos_rate*log(pos_rate) + (1-pos_rate)*log(1-pos_rate);
    //ne = -logloss/backgroundLoss;
    //cout<<"NE is "<<ne<<endl;
}

} // namespace pslr

