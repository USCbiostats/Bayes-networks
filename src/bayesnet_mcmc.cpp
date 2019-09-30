#include <Rcpp.h>
#include "network.cpp"
using namespace Rcpp;

//' Main network playing function
//'
//' @param X numeric matrix
//' @param Npar Integer vector. Number of parents to each node.
//' @param nodetype Integer vector. Type of the nodes. 1 = source: 2 = sink;
//'     0 = neither.
//' @param par Integer Matrix. Parents of node p in fitted graph.
//' @param nodes A vector of integers. Denoting nodes in a network.
//' @param egdes A vector of integers. Denoting egdes in a network.
//' @param MaxPar Integer. Maximum number of parents allowed for a node.
//'     Default to 50.
//' @param phi Numeric. prior on distance from prior network. Defaults to 1.
//' @param omega Numeric. prior on network size. Defaults to 6.9.
//' @param InitialNetwork Integer. 0 = simulated; 1 = randon; 2 = null.
//' @param drop Integer. number to drop for burn-in. Defaults to 0.
//' @param N Interger, determines the number of MCMC steps.
//' @param output Integer. output every nth iteration. Defaults to 100.
//'
//' @export
// [[Rcpp::export]]
DataFrame main_fun(NumericMatrix X,
                   IntegerVector Npar,
                   IntegerVector nodetype,
                   IntegerMatrix par,
                   IntegerVector nodes, // Not final parameters
                   IntegerVector egdes, // Not final parameters
                   int MaxPar = 50,
                   const double phi = 1,
                   const double omega = 6.9,
                   const int InitialNetwork = 2,
                   const int drop = 0,
                   int N = 1000,
                   int output = 10) {

  network my_network(X, par, Npar, nodetype, InitialNetwork, MaxPar,
                     nodes, egdes);

  for (int i {0}; i < N;  i++) {
    my_network.save_graph();

    if (R::runif(0, 1) > 0.2) {
      my_network.propose_addition();
    } else {
      my_network.propose_deletion();
    }

    if (my_network.checker()) {
      my_network.restore_graph();
    }

    if (i % output == 0) {
      my_network.logger(i);
    }
  }

  DataFrame df = DataFrame::create( Named("V1") = 1 , _["V2"] = 1 );

  return my_network.result();
}
