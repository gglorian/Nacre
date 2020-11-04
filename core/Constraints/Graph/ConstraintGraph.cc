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
    return connected(level, touched) || checkPaths(level, touched);
}

bool ConstraintGraph::filterNodes(int level, vector<Variable*>& touched)
{
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

    return filterNodes(level, touched);
}

void ConstraintGraph::addAllNextOriented(int curRoom)
{
    for (int i = 0; i < numberOfRooms; i++) {
        if (!corridors[curRoom][i]->isAssigned() || corridors[curRoom][i]->isAssignedTo(1))
            roomStack.push_back(i);
    }
}

bool ConstraintGraph::checkPaths(int level, vector<Variable*>& touched)
{
    fill(seen.begin(), seen.end(), false);
    fill(tmpSeen.begin(), tmpSeen.end(), false);

    bool validEntry;
    int nbValidEntry = 0;

    // For each entry room possible, we check if there is a path to a possible exit room
    for (int i = 0; i < numberOfRooms; ++i) {
        if (entries[i]->isAssignedTo(0) || actives[i]->isAssignedTo(0))
            continue;

        roomStack.clear();
        roomStack.push_back(i);
        validEntry = false;

        while (!roomStack.empty()) {
            int curRoom = roomStack.back();
            roomStack.pop_back();

            if (tmpSeen[curRoom])
                continue;
            tmpSeen[curRoom] = true;

            if (!exits[curRoom]->isAssignedTo(0))
                validEntry = true;

            addAllNextOriented(curRoom);
        }

        if (!validEntry) {
            // The entry is not connected to an exit, we must remove it
            if (!fixToValue(entries[i], 0, level))
                return true; // UNSAT because the entry was already assigned
            touched.push_back(actives[i]);
        } else {
            nbValidEntry++;
            for (size_t j = 0, stop = seen.size(); j < stop; ++j)
                seen[j] = seen[j] | tmpSeen[j];
        }

        fill(tmpSeen.begin(), tmpSeen.end(), false);
    }

    if (!nbValidEntry)
        return true; // UNSAT because no valid entry was found

    return filterNodes(level, touched);
}
