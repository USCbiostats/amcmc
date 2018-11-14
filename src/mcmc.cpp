#include <Rcpp.h>
using namespace Rcpp;

void normal_prop_void(
    NumericVector* ans,
    const NumericVector & x,
    const NumericVector & lb,
    const NumericVector & ub,
    const NumericVector & scale,
    const IntegerVector & fixed
) {
  
  int K = x.size();
  
  // Proposal
  GetRNGstate();
  (*ans) = x + rnorm(x.size())*scale;
  PutRNGstate();
  
  for (int k=0; k<K; k++) {
    
    // Is it fixed?
    if (fixed.at(k)==1) {
      (*ans).at(k) = x.at(k);
      continue;
    }
    
    // Reflection adjustment
    while( ((*ans)[k] > ub[k]) | ((*ans)[k] < lb[k]) ) {
      
      if ((*ans)[k] > ub[k]) {
        (*ans)[k] = 2.0*ub[k] - (*ans)[k];
      } else {
        (*ans)[k] = 2.0*lb[k] - (*ans)[k];
      }  
      
    }
    
  }
  
  return;
}

// [[Rcpp::export(rng=false)]]
NumericVector normal_prop(
    const NumericVector & x,
    const NumericVector & lb,
    const NumericVector & ub,
    const NumericVector & scale,
    const IntegerVector & fixed
) {
  
  // Proposal
  NumericVector ans(x.length());
  normal_prop_void(&ans, x, lb, ub, scale, fixed);
  
  return ans;
}

// [[Rcpp::export(name = ".MCMC", rng=false)]]
NumericMatrix MCMC(
    Function fun,
    const NumericVector & theta,
    int nsteps,
    const NumericVector & lb,
    const NumericVector & ub,
    const NumericVector & scale,
    const IntegerVector & fixed
) {
  
  int K = lb.size();
  
  NumericMatrix ans(nsteps, K);
  NumericVector theta0 = clone(theta);
  NumericVector theta1 = clone(theta);
  NumericVector f0 = fun(theta0), f1(1);
  
  // Checking values
  if (is_na(f0)[0u] || is_nan(f0)[0u])
    stop("fun(par) is undefined. Check either -fun- or the -lb- and -ub- parameters.");
  
  // Using sugar to generate the random values for the hastings ratio.
  GetRNGstate();
  NumericVector R = runif(nsteps);
  PutRNGstate();
  
  int k;
  for (int i = 0; i < nsteps; i++) {
  
    // Generating proposal
    normal_prop_void(&theta1, theta0, lb, ub, scale, fixed);
    // Take a look at https://github.com/cran/mcmc/blob/c0644b84416a75293e1d31b87d4f2af47c0784f5/src/metrop.c#L230-L250
    f1 = fun(theta1);
  
    // Checking values
    if (is_na(f1)[0u] || is_nan(f1)[0u])
      stop("fun(par) is undefined. Check either -fun- or the -lb- and -ub- parameters.");
    
    // Metropolis-Hastings ratio
    if (R[i] < exp( f1[0] - f0[0] )) {
      theta0 = clone(theta1);
      f0[0] = f1[0];
      
    }
    
    // Storing the current state
    ans(i, _) = theta0;
    
  }
  
  return ans;
}

void update_equal(
  NumericVector & par,
  const std::vector< std::vector< unsigned int > > & ids
) {
  
  for (int i = 0; i < (int) ids.size(); i++)
    for (int j = 1; j < (int) ids.at(i).size(); j++)
      par.at(ids.at(i).at(j)) = par.at(ids.at(i).at(0));
  
  return;
}

// [[Rcpp::export]]
NumericVector update_equal(
  const NumericVector & par,
  const std::vector< std::vector< unsigned int > > & ids
) {
  
  NumericVector ans = clone(par);
  update_equal(ans, ids);
  
  return ans;
  
}
