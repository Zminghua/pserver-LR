#ifndef PSLR_METRIC_H_
#define PSLR_METRIC_H_

#include <string>
#include <vector>


namespace pslr {

#define SEG 5000

struct pred_label {
    int label;
    int pred;
    double score;
    bool operator < (const pred_label &other) const {
        return other.score<score;
    }
};

typedef struct pred_label ins_pair;

class Metric {
	public:
        void clear();
        void add(double score, int pred, int label);
        long double get_auc();
        std::vector<long double> get_PRF();
        void show();

    private:
        long long total;
        long long pos_num;
        long long neg_num;
        std::vector<ins_pair> pre_list;
        long long q_dist[SEG];
        long long qq_plot[SEG][2];

        long double p_click;
        long double mae;
        long double mse;
        long double copc;
        long double pos_rate;
        long double logloss;
        long double auc;
        long double backgroundLoss;
        long double ne;

}; // Metric

} // namespace pslr

#endif  // PSLR_METRIC_H_


