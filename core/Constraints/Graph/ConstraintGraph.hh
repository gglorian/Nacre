/*
 * @CreateTime: Nov 3, 2020 9:50 AM 
 * @Author: Gaël Glorian 
 * @Contact: gael.glorian@gmail.com 
* @Last Modified By: Gaël Glorian
* @Last Modified Time: Mar 15, 2021 3:56 PM
 * @Description: Modify Here, Please  
 */

#ifndef CONSTRAINTGRAPH_H_
#define CONSTRAINTGRAPH_H_

#include "Constraint.hh"
#include "Variable.hh"

#include <string>
#include <vector>

class ConstraintGraph : public Constraint {
protected:
    std::vector<std::vector<Variable*>> corridors;
    std::vector<Variable*> entries;
    std::vector<Variable*> exits;
    std::vector<Variable*> actives;
    std::vector<Variable*> finals;

    int numberOfRooms;

    std::vector<bool> seen;
    std::vector<bool> possible;
    std::vector<bool> kept;

    std::vector<int> roomStack;

    void addAllNext(int curRoom);
    bool connected(int level, std::vector<Variable*>& touched);

    void addAllNextOriented(int curRoom);
    bool checkPaths(int level, std::vector<Variable*>& touched);

    bool filterNodes(int level, std::vector<Variable*>& touched);

    bool handleNode(int roomID);

public:
    ConstraintGraph(std::string n, std::vector<Variable*> vars,
        std::vector<std::vector<Variable*>> matrix,
        std::vector<Variable*> entriesArg,
        std::vector<Variable*> exitsArg,
        std::vector<Variable*> activesArg,
        std::vector<Variable*> finalsArg)
        : Constraint(n, vars)
        , entries(entriesArg)
        , exits(exitsArg)
        , actives(activesArg)
        , finals(finalsArg)
    {
        numberOfRooms = entries.size();
        seen.resize(numberOfRooms);
        possible.resize(numberOfRooms);
        kept.resize(numberOfRooms);

        for (size_t i = 0; i < matrix.size(); i++) {
            corridors.push_back(std::vector<Variable*>(matrix[i]));
        }
    };

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};

#endif // CONSTRAINTGRAPH_H_