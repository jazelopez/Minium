/*
This program uses dynamic programing to find the number of
distinct subsets leading up to the target the user inputs from
the given .txt file. As it populates the table we are getting
the number of distinct subsets and number of min subset and
Lexicographically first min-sized subset.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>


struct ssum_elem {
  unsigned int x;
  std::string name;
};
//book keeping struct
struct table_val {
  bool valid;
  //ptr to inclusive predecessor
  table_val* in_prev;
  //ptr to exclusive predecessor
  table_val* ex_prev;
  //holds number of distinct subsets yielding target
  int64_t size = 0;

  //holds index of input string
  int64_t index;
  //holds target sum
  int64_t sum;

  //holds size of minimum sized subset yielding target 
  int64_t min_size;
  int64_t num_of_min;
};


class ssum_instance {
    int64_t target=0;
    //Vector to contain input values along input value names
    std::vector<ssum_elem> elems;
    //table of structs which each contain a bookkeeping struct
    std::vector<std::vector<table_val>> feasible;  
    // feasible[i][x] = TRUE if there is a subset
    //  of a[0..i] adding up to x; FALSE otherwise
    //  (once instacne has been solved)
    //
    int done=false;    // flag indicating if dp has been run or 
                       //   not on this instance

    public:
    //returns minimum subset size for inclusive case
    //Function returns minimum sized subset
    //returns minimum subset size for inclusive case
    int min_subset(int i, int index, int tgt, int &track) {
      int j = index;
      int x = tgt;
      int min = elems.size();
      //if not base case
      if (x > 1) {
          while (j >= 0) {
            //if feasible value is inclusive
            if (feasible[j][x-elems[i].x].in_prev != NULL ) {
              //and feasible vale is new minimum
              if (feasible[j][x-elems[i].x].min_size < min) {
                track = 1;
                min = feasible[j][x-elems[i].x].min_size;
              } 
              //else if min value == feasiblemin value, increment number of minimum sized subsets
              else if (feasible[j][x-elems[i].x].min_size == min) 
                track += feasible[j][x-elems[i].x].num_of_min ;
            }
            //moved down feasible column
            j--;
          } //second base case if no inclusive min is found (singleton)
            if (min == elems.size()) {min = 1; track = 1;}
        }
        //base case when end of table is reached
        else {
          //set min book keeping values to 1
          track = 1; 
          return 1;
        }
        //minimum sized subset, is previous minsized subset + 1 to account for new inclusive element
        return min + 1;
    }

 //Traceback prints out lexicographically first minimum sized subset
  void traceBack(int first,int start, unsigned int target) {
      int cur_tgt = target;
     //set pointers to first instance of minimum inclusive subset in target column
      table_val *cur = &feasible[start][cur_tgt];
      table_val *min = &feasible[start][cur_tgt];
      int runs = 0;
      std::cout << "Lexicographically first minsized subset :\n      " << elems.size() - first << " " << elems.at(first).name << "\n";
      //while remaining target sum is not 0
      while (cur_tgt != 0 && runs <= elems.size()) {
        //within column for cur target, and entries remain valid possible subsets
        while (cur->valid == 1 &&cur->ex_prev != NULL) {
         //if current item contains a smaller min subset and is inclusive, update pointers
          if (cur->min_size < min->min_size && cur->in_prev != NULL)
            min = cur;
          //increment cur pointer
            cur = cur->ex_prev;
        }
         //additional check for last exclusive case
        if (cur->min_size < min->min_size && cur->in_prev != NULL)
            min = cur;
         //print element info of current minimum index (due to reversing, this prints in lexicographical order 
        std::cout << "      " << elems.size() -  min->index << " " << elems.at(min->index).name << "\n";
         //decrement remaining current target by the latest element value
        cur_tgt-= elems.at(min->index).x;
         //update cur pointer to inclusive previous
        cur = min->in_prev;       
        runs ++;
      }
      std::cout << "\n";
    }
    

    //reads in input string and populates elems vector
    void read_elems( std::istream &stream) {
        ssum_elem e;

        elems.clear();
        // while(std::cin >> e.x && std::cin >> e.name) {
        while(stream>> e.x && stream >> e.name) {
            elems.push_back(e);
        }
        done = false;
        //reverses elems vector in order to make maintaing lexographical ordering easier
        std::reverse(elems.begin(), elems.end());
    }

    // Function:  solve
    // Desc:  populates dynamic programming table of
    //    calling object for specified target sum.
    //    Returns true/false depending on whether the
    //    target sum is achievalble or not.

    bool solve(unsigned int tgt) {
        //keep track of size of input
        unsigned int n = elems.size();
        //initialize ints to maintain coordinates within variable
        unsigned int i, x;

        
        if(target == tgt && done) 
            return feasible[n-1][tgt].valid;
        

        target = tgt;
        //initialze booking keeping struct with default values for unsolved feasible table
        table_val initialize;
        initialize.valid = false;
        initialize.in_prev = NULL;
        initialize.ex_prev = NULL;
        initialize.size = 0;
        initialize.min_size = 0;
        initialize.index = 0;
        initialize.sum = 0;
        initialize.num_of_min = 0;
        //initialize feasible to default unsolved values
        feasible = 
            std::vector<std::vector<table_val>>(n, std::vector<table_val>(target+1, initialize));


        //PREPROCESSING
        //Base case of empty set, all valid for target of 0
        for(i=0; i<n; i++) {
          feasible[i][0].valid = true;
          //size of possible subsets is 1 for the empty set-> {}
          feasible[i][0].size = 1;
          feasible[i][0].in_prev = &feasible[i][0];
          feasible[i][0].num_of_min = 1;
        }
          
        //Base for subset of size 1
        for(x=1; x<=target; x++) {
            //singleton case, subset of size 1 is equal to the target x
            if(elems[0].x == x) {
              //set feasible to true
              feasible[0][x].valid = true;
              //increment amount of distinct subsets
              feasible[0][x].size += 1;
              //minimum size for target at index is now 1
              feasible[0][x].min_size = 1;
              feasible[0][x].num_of_min = 1;
              //set inclusive to true, by element pointing it to itself (used later)
              feasible[0][x].in_prev = &feasible[0][x];
            }
        }
 
 
        //Populate Rest of feasible table
        //for subsets of size 2 to entire set
        for(i=1; i<n; i++) {
            //for target of 1 to desired target
            for(x=1; x<=tgt; x++) {
                //Check exclusive case
                if(feasible[i-1][x].valid) {
                  feasible[i][x].valid = true;
                  //get coordinates within feasible
                  feasible[i][x].index = i;
                  feasible[i][x].sum = x;
                  //set exclusive pointer
                  feasible[i][x].ex_prev = &feasible[i-1][x];
                  //set inclusive pointer NULL by default
                  feasible[i][x].in_prev = NULL;
                  //account for exclusive disctint subsets
                  feasible[i][x].size = feasible[i-1][x].size;
                }
                //if exclusive case does not yield subset leading to target, set exclusive pointer to null
                else 
                  feasible[i][x].ex_prev = NULL;
                //Also must check inclusive case
                if(x >= elems[i].x && feasible[i-1][x-elems[i].x].valid) {
                    feasible[i][x].valid = true;
                    //set inclusive pointer to subset below, and to current target minus value at current index
                    feasible[i][x].in_prev = &feasible[i-1][x-elems[i].x];
                    //set coordinates of current feasible book keeping object
                    feasible[i][x].index = i;
                    feasible[i][x].sum = x;
                    //Distinct subsets yielding current target is number of distinct subsets in the inclusive and exclusive case
                    feasible[i][x].size =  feasible[i][x].size + feasible[i-1][x-elems[i].x].size;
  
                    int num_min = 0;
                    //get min possible number of subsets leading up to inclusive case
                    int min = this->min_subset(i, i - 1, x, num_min);
                    feasible[i][x].num_of_min = num_min;
                    feasible[i][x].min_size = min;
                }
            }
        }
        done = true;
        return feasible[n-1][target].valid;
    }

    //print info describing solution
    void printFeasible(unsigned int target) {
      unsigned int n = elems.size();
      unsigned int tgt = target + 1;
      unsigned int i, x;

      //Print number of distinct subsets
      std::cout << "Distinct Subsets: " << feasible[n - 1][tgt - 1].size << "\n";

      //find minimum cardinality subset yielding target by looping through last column
      int j = n - 1;
      int index_of_min = 0;
      int min = feasible[j][tgt - 1].min_size;
      //while subsets yield valid target sums and index remains within bounds
      while (feasible[j][tgt - 1].valid == true && j >= 0) {
        //if current index is inclusive
        if (feasible[j][tgt - 1].in_prev != NULL)
          //get min
          min = (feasible[j][tgt - 1].min_size < min) ? feasible[j][tgt - 1].min_size : min;
        j--;
      }
      //counter to keep track number of minimum cardinality subsets
      int count = 0;
      bool traceBack = false;
      j = n - 1;
      //while subsets yield valid target sums and index remains within bounds
      while (feasible[j][tgt - 1].valid == true && j >= 0) {
        //std::cout << elems.size() - j << "->" << feasible[j][tgt - 1].min_size <<"\n" ;
        //if minimum cardinality increment 
        if (min == feasible[j][tgt - 1].min_size && feasible[j][tgt - 1].in_prev != NULL) {
          
          count += feasible[j][tgt - 1].num_of_min;
          if (!traceBack) {
            this->traceBack(j ,j - 1,target - elems.at(j).x);}
            std::cout << feasible[j][tgt - 1].num_of_min << "*" << feasible[j][tgt - 1].min_size <<":" << elems.size() - j<<"\n";
          traceBack = true;
        }
        j--;
      }
      //output minimum cardinality leading to target
      std::cout << "Minimum size subset leading to target : " << min << "\n";
      //output number minimmum cardinality subsets leading to target
      std::cout << "Number of minsized subsets : " << count << "\n";

    }
  
};  // end class





/**
* usage:  ssum  <target> < <input-file>
*
*
* input file format:
*
*     sequence of non-negative-int, string pairs
*
*     example:

    12 alice
    9  bob
    22 cathy
    12 doug

* such a file specifies a collection of 4 integers: 12, 9, 22, 12
* "named" alice, bob, cathy and doug.
*/
int main(int argc, char *argv[]) {
  unsigned int target;
  ssum_instance ssi;

  if(argc != 2) {
    fprintf(stderr, "one cmd-line arg expected: target sum\n");
    return 0;
  }
  if(sscanf(argv[1], "%u", &target) != 1) {
    fprintf(stderr, "bad argument '%s'\n", argv[1]);
    fprintf(stderr, "   Expected unsigned integer\n");
    return 0;
  }

  ssi.read_elems(std::cin);

  if(ssi.solve(target) ) {
    std::cout << "HOORAY!  Apparently, the target sum of " <<
      target << " is achievable\n";
    std::cout << "  How you ask?  Sorry, we just know it is possible...\n";
    ssi.printFeasible(target);
  }
  else {
    std::cout << "SORRY!  Apparently, the target sum of " <<
      target << " is NOT achievable\n";
  }

}
