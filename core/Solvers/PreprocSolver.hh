/*
 * @CreateTime: Dec 18, 2020 11:38 AM 
 * @Author: Gaël Glorian 
 * @Contact: gael.glorian@gmail.com 
* @Last Modified By: Gaël Glorian
* @Last Modified Time: Dec 18, 2020 11:53 AM
 * @Description: Modify Here, Please  
 */

#ifndef PREPROCSOLVER_H_
#define PREPROCSOLVER_H_

#include "ConsistencyHandler.hh"
#include "Problem.hh"
#include "Solver.hh"

class PreprocSolver : public Solver {
public:
    /// Constructor
    PreprocSolver(Problem* p)
        : Solver(p, new ConsistencyHandler(p)) {};

    PreprocSolver(Problem* p, ConsistencyHandler* ch)
        : Solver(p, ch) {};

    /// Perform AC once, return true if UNSAT.
    virtual bool init() { return cspAC->init(); }

    /// Exhaustive research based on MAC, return 0 if the problem is satisfiable, 1 if it is unsatisfiable.
    virtual int search(int zeroing = 0) { return 0; }

    /// Display a lot of information on the solver and an instantiation if the problem is found SAT
    virtual int showFinal(int res);
};
#endif // PREPROCSOLVER_H_
