#include <cstdlib>
#include <vector>
#include <ctime>
#include <algorithm>
#include "../include/dfa.h"

//template <class vector>
DFA::DFA (int initial, std::vector<int> Q, std::vector<int> F, int* trans, int sigma) {
  q0 = initial;
  states = Q;
  finals = F;
  transitions = trans;
  alphabet_size = sigma;
}

DFA::DFA (int num_states, int alphabet_size) {
  std::srand(std::time(0));
  std::vector<int> Q, F;
  for(int i = 0; i < num_states; ++i){
    Q.insert(Q.begin() + i, i);
    if(std::rand() % 2) F.insert(F.begin() + i, i);
  }
  states = Q;
  finals = F;
  q0 = std::rand() % num_states;
  int transition_size = num_states * alphabet_size;
  int* trans = (int*)std::calloc(transition_size, sizeof(int));
  for(int state = 0; state < num_states; state++)
  for(int tran_num = 0; tran_num < alphabet_size; tran_num++){
    int r = std::rand() % num_states;
    trans[state * alphabet_size + tran_num] = r;
  }
  transitions = trans;
  alphabet_size = alphabet_size;
}

std::vector<int> DFA::getStates() {
  return states;
}

int DFA::num_states() {
  return states.size();
}

int DFA::num_finals() {
  return finals.size();
}

std::vector< std::vector<int> > DFA::hopcroft(){

  //P := {F, Q \ F};
  std::vector<int> F = finals;
  std::vector<int> Fc;
  std::sort (states.begin(), states.end());
  std::sort (finals.begin(), finals.end());
  std::set_difference(states.begin(), states.end(), finals.begin(),  finals.end(),
  std::inserter(Fc, Fc.begin()));
  std::vector< std::vector<int> >  P;
  P.insert(P.begin(), F); P.insert(P.begin() + 1, Fc);

  //  Ws := {F};
  std::vector< std::vector<int> > Ws;
  Ws.push_back(F);
  // std::vector<int> Wa;
  // std::vector<int> smaller = F.size() <= Fc.size() ? F : Fc;
  // for(int i = 0; i < alphabet_size; i++){
  //   Ws.insert(Ws.begin()+i, smaller);
  //   Wa.insert(Wa.begin()+i, i);
  // }

  //while (Ws is not empty) do
  while (Ws.size() > 0) {
    //choose and remove a set A from W
    std::vector<int> A = Ws[0];
    Ws.erase(Ws.begin());
    //for each c in Σ do
    for (int c = 0; c < alphabet_size; ++c){
      //let X be the set of states for which a transition on c leads to a state in A
      std::vector<int> X;
      std::vector<int>::iterator iter = X.begin();
      for (int q = 0; q < num_states(); q++) {
        for (int qa = 0; qa < A.size(); qa++) {
          if (transitions[num_states() * q + c] == A[qa])
            X.push_back(q);
        }
      }

      //for each set Y in P for which X ∩ Y is nonempty and Y \ X is nonempty do
      for (int y = P.size()-1; y >= 0; --y) {
        std::vector<int> Y = P[y];
        std::vector<int> XintY(std::min(X.size(),Y.size()));
        std::vector<int> YsubX(Y.size());
        std::vector<int>::iterator iter1;
        std::vector<int>::iterator iter2;
        std::sort (X.begin(),X.end());
        std::sort (Y.begin(),Y.end());
        iter1=std::set_intersection (X.begin(), X.end(), Y.begin(), Y.end(),
        XintY.begin());
        iter2=std::set_difference (X.begin(), X.end(), Y.begin(), Y.end(),
        XintY.begin());
        XintY.resize(iter1 - XintY.begin());
        YsubX.resize(iter2 - YsubX.begin());
        if (!XintY.empty() && !YsubX.empty()){
          //replace Y in P by the two sets X ∩ Y and Y \ X
          P.erase(P.begin()+y);
          P.push_back(XintY); P.push_back(YsubX);
          //if Y is in W
          int i = 0;
          for ( ; i < Ws.size(); i++)
          if (Ws[i] == Y) {
            break;
          }
          if (i != Ws.size()) {
            //replace Y in W by the same two sets
            Ws.erase(Ws.begin() + i);
            Ws.push_back(XintY);
            Ws.push_back(YsubX);
          } else {
            //if |X ∩ Y| <= |Y \ X|
            if (XintY.size() <= YsubX.size()) {
              //add X ∩ Y to W
              Ws.push_back(XintY);
            }
            else {
              //add Y \ X to W
              Ws.push_back(YsubX);
            }
          }
        }
      }
    }
  }
  return Ws;
}


DFA DFA::hopPartition2DFA(std::vector< std::vector<int> > Ws) {
  int initial;
  std::vector<int> fin;
  std::vector<int> Q;
  int transition_size = alphabet_size * Ws.size();
  int* trans = (int*)std::calloc(transition_size, sizeof(int));
  for (int i = 0; i < Ws.size(); ++i) {
    Q.push_back(i);
    for (int j = 0; j < finals.size(); ++j) {
      if (Ws[i][0] == finals[j]) {
        fin.push_back(i);
        break;
      }
    }
    for (int j = 0; j < Ws[i].size(); ++j) {
      if (Ws[i][j] == q0) initial = i;
      // else std::cout<<"Hopcroft failed"<<std::endl;
    }
    for (int j = 0; j < alphabet_size; ++j) {
      int dest = transitions[alphabet_size * Ws[i][0] + j];
      bool done = false;
      for (int k = 0; k < Ws.size(); ++k) {
        if (done) break;
        for (int l = 0; l < Ws[k].size(); ++l) {
          if (Ws[k][l] == dest) {
            trans[alphabet_size * i + j] = k;
            done = true;
            break;
          }
        }
      }
    }
  }

  return DFA(initial, Q, fin, trans, alphabet_size);
}
