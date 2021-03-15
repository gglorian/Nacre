/*
 * @CreateTime: Nov 3, 2020 10:44 AM 
 * @Author: Gaël Glorian 
 * @Contact: gael.glorian@gmail.com 
* @Last Modified By: Gaël Glorian
* @Last Modified Time: Mar 15, 2021 3:54 PM
 * @Description: Modify Here, Please  
 */

#include "ConstraintGraph.hh"

using namespace std;

bool ConstraintGraph::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    // cerr << "propagate" << endl;
    return connected(level, touched) || checkPaths(level, touched);
}

bool ConstraintGraph::filterNodes(int level, vector<Variable*>& touched)
{
    for (int i = 0; i < numberOfRooms; ++i) {
        if (kept[i])
            continue;

        if (actives[i]->isAssignedTo(1))
            return true; // UNSAT because an active room as not been kept

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
        if (actives[i]->isAssignedTo(0))
            continue;

        if (!corridors[curRoom][i]->isAssignedTo(0) || !corridors[i][curRoom]->isAssignedTo(0))
            roomStack.push_back(i);
    }
}

bool ConstraintGraph::connected(int level, vector<Variable*>& touched)
{
    fill(kept.begin(), kept.end(), false);
    roomStack.clear();

    // Put the first active room in the stack
    bool sat = false;
    for (int i = 0; i < numberOfRooms; ++i) {
        if (actives[i]->isAssignedTo(1)) {
            roomStack.push_back(i);
            break;
        }

        if (!actives[i]->isAssignedTo(0))
            sat = true;
    }
    if (roomStack.empty())
        return !sat;

    while (!roomStack.empty()) {
        int curRoom = roomStack.back();
        roomStack.pop_back();

        if (kept[curRoom])
            continue;
        kept[curRoom] = true;

        addAllNext(curRoom);
    }

    return filterNodes(level, touched);
}

void ConstraintGraph::addAllNextOriented(int curRoom)
{
    for (int i = 0; i < numberOfRooms; i++) {
        if (actives[i]->isAssignedTo(0))
            continue;

        if (!corridors[curRoom][i]->isAssignedTo(0))
            roomStack.push_back(i);
    }
}

bool ConstraintGraph::handleNode(int curRoom)
{
    bool validEntry = false;

    seen[curRoom] = true;
    possible[curRoom] = true;

    // Check if the path is valid
    if (kept[curRoom] || !exits[curRoom]->isAssignedTo(0)) {
        validEntry = true;
        for (size_t j = 0, stop = kept.size(); j < stop; ++j)
            kept[j] = kept[j] || possible[j];
    }

    // Add unseen childs into stack
    for (int j = 0; j < numberOfRooms; j++) {
        if (actives[j]->isAssignedTo(0))
            continue;

        if (!corridors[curRoom][j]->isAssignedTo(0)) {
            // Validate the path when a child is kept
            if (kept[j]) {
                validEntry = true;
                for (size_t k = 0, stop = kept.size(); k < stop; ++k)
                    kept[k] = kept[k] || possible[k];
            }

            else if (!seen[j])
                if (handleNode(j))
                    validEntry = true;
        }
    }

    possible[curRoom] = false;

    return validEntry;
}

bool ConstraintGraph::checkPaths(int level, vector<Variable*>& touched)
{
    fill(seen.begin(), seen.end(), false);
    fill(possible.begin(), possible.end(), false);
    fill(kept.begin(), kept.end(), false);

    int nbValidEntry = 0;

    // For each entry room possible, we check if there is a path to a possible exit room
    for (int i = 0; i < numberOfRooms; ++i) {
        if (entries[i]->isAssignedTo(0) || actives[i]->isAssignedTo(0))
            continue;

        if (handleNode(i)) {
            nbValidEntry++;
        } else {
            // The entry is not connected to an exit, we must remove it
            if (!fixToValue(entries[i], 0, level))
                return true; // UNSAT because the entry was already assigned
            touched.push_back(actives[i]);
        }
        fill(possible.begin(), possible.end(), false);
    }

    if (!nbValidEntry)
        return true; // UNSAT because no valid entry was found

    return filterNodes(level, touched);
}
