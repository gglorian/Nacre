/*
 * @CreateTime: Nov 3, 2020 10:44 AM 
 * @Author: Gaël Glorian 
 * @Contact: gael.glorian@gmail.com 
 * @Last Modified By: Gaël Glorian 
 * @Last Modified Time: Nov 3, 2020 10:44 AM 
 * @Description: Modify Here, Please  
 */

#include "ConstraintGraph.hh"

using namespace std;

bool ConstraintGraph::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    return connected(level, touched);
}

void ConstraintGraph::addAllNext(int curRoom)
{
    for (int i = 0; i < numberOfRooms; i++) {
        if (!corridors[curRoom][i]->isAssigned() || corridors[curRoom][i]->isAssignedTo(1)) {
            roomStack.push_back(i);
            continue;
        }
        if (!corridors[i][curRoom]->isAssigned() || corridors[i][curRoom]->isAssignedTo(1))
            roomStack.push_back(i);
    }
}

bool ConstraintGraph::connected(int level, vector<Variable*>& touched)
{
    fill(seen.begin(), seen.end(), false);
    roomStack.clear();

    // Put the first active room in the stack
    for (int i = 0; i < numberOfRooms; ++i) {
        if (!actives[i]->isAssignedTo(0)) {
            roomStack.push_back(i);
            break;
        }
    }
    if (roomStack.empty())
        return true;

    while (!roomStack.empty()) {
        int curRoom = roomStack.back();
        roomStack.pop_back();

        if (seen[curRoom])
            continue;
        seen[curRoom] = true;

        addAllNext(curRoom);
    }

    for (int i = 0; i < numberOfRooms; ++i) {
        if (seen[i])
            continue;

        if (actives[i]->isAssignedTo(1))
            return true; // UNSAT because an active room as not been seen

        if (!actives[i]->isAssigned()) {
            if (!fixToValue(actives[i], 0, level))
                return true;
            touched.push_back(actives[i]);
        }
    }

    return false;
}
