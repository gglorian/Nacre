/*
 * @CreateTime: Dec 18, 2020 11:37 AM 
 * @Author: Gaël Glorian 
 * @Contact: gael.glorian@gmail.com 
* @Last Modified By: Gaël Glorian
* @Last Modified Time: Dec 18, 2020 12:04 PM
 * @Description: Modify Here, Please  
 */

#include "PreprocSolver.hh"
#include "Constraint.hh"
#include "InfoSystem.hh"
#include "Options.hh"
#include "Solver.hh"
#include "Variable.hh"

#include <cassert>
#include <cstring>
#include <iostream>
#include <math.h>
#include <vector>

using namespace std;

/**
   Allow to print the final message.
 */
int PreprocSolver::showFinal(int res)
{
    int ret = 0;

    cout << "c" << endl
         << "c # time = " << InfoSystem::timeElapsed() << endl
         << "c # Dec = " << Stats::nbDecisions << endl
         << "c # Propagate Calls = " << Stats::nbPropCalls << endl
         << "c # assignAt Calls = " << Stats::assignCall << endl
         << "c # removeAt Calls = " << Stats::rmCall << endl
         << "c # Run = " << Stats::run << endl
         << "c # Conflicts = " << Stats::conflict << endl;

    uint64_t mem_used = InfoSystem::getMemUsed();
    if (mem_used != 0)
        cout << "c # Memory used = " << (double)mem_used / 1000 << " Mb" << endl;

    if (Options::countSols == countTypes::all) {
        cout << "c # Sols = " << Stats::nbSols << endl;
        exit(40);
    }

    switch (res) {
    case 0:
        cout << "s SATISFIABLE" << endl
             << "c" << endl;
        cout << "v <instantiation>" << endl
             << "v     <list> ";
        for (auto var : problem->getVariables())
            cout << var->getName() << " ";
        cout << "</list>" << endl
             << "v     <values> ";

        for (auto var : problem->getVariables()) {
            if (var->domainInitSize == var->domainCurSize) {
                cout << "* ";
            } else if (var->domainCurSize == 1) {
                cout << var->getVarPropFromLocalDomInd(0).val << " ";
            } else {
                cout << "{ ";
                for (int i = 0; i < var->domainCurSize; i++) {
                    cout << var->getVarPropFromLocalDomInd(i).val << " ";
                }
                cout << "} ";
            }
        }

        cout << "</values>" << endl
             << "v </instantiation>" << endl;
        ret = 10;
        break;
    case 1:
        cout << "s UNSATISFIABLE" << endl;
        ret = 20;
        break;
    case 2:
        cout << "s UNKNOWN" << endl;
        ret = 30;
        break;
    }

    return ret;
} // showFinal
