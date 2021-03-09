//CS 170 Project 1: The Eight Puzzle
//Due 2/21/2021 11:59pm
//Name: Carson Welty
//netID: cwelt001
//SID: 862012918
//don't copy this losers I'm watching you
//cool code

#include <iostream>
#include <string>
#include <vector>
#include <queue>
using namespace std;

//global declarations
struct node;
struct manhattans;

int algorithm= 0; //the heuristic function selected by the user
int maxQSize = 0; //holds the max number of nodes in the priority queue at any given time 
string depthResult = "";
vector<vector<int>> closedNodes; //list of nodes no longer needed
vector<vector<int>> discoveredNodes; //list of nodes that are discovered but not expanded
vector<vector<int>> expandedNodes; //list of expanded nodes
priority_queue<node, vector<node>, greater<node>> heap;

//helper function declarations
void printPuzzle(node);
void displayResults();
void setupManhattanGoalState(vector<manhattans> &goalState);
void compareQueue();
void chooseAlgorithm();
node defaultPuzzleSetup();
node customPuzzleSetup();
node setupPuzzleGoalState();

class node { 
    public:
        int h_n;
        int depth;
        vector<int> state;

        node() {};
        node(vector<int>);
        void dequeNode();
        void expandNode();    
        void queueingFunction(int algorithm, int hParent);
        void chooseTradePos(int zeroPos);
        void tradePos(int,int);
        void setupCurrState(vector<manhattans>&);
        int zeroLocator();
        int manhattanDistance();
};

class manhattans {
    public:
        int x;
        int y;
        int num;

        manhattans(int,int,int);
        int getNum();
};

//BEGIN FUNCTION DEFINITIONS FOR NODE STRUCT

//parameterized construcor
node::node(vector<int> input) {
        state = input;
}

//finds manhattan distance between goal state and current state
int node::manhattanDistance() {
    vector<manhattans> goalState;
    vector<manhattans> nodePos;
    int dist = 0;

    setupManhattanGoalState(goalState);
    setupCurrState(nodePos);

    //calculates the manhattan distance: abs(x2 - x1) + abs(y2 - y1)
    for (int i = 0; i < nodePos.size(); i++) {
        for (int j = 0; j < goalState.size(); j++) {
            if (nodePos.at(i).num == goalState.at(j).num && nodePos.at(i).num != 0) {
                dist += abs(nodePos.at(i).x - goalState.at(j).x) + abs(nodePos.at(i).y - goalState.at(j).y);
            }
        }
    }
    return dist;
}

//calculates h_n and depth according to choice of algorithm 
void node::queueingFunction(int algorithm, int hParent) {
    depth = hParent + 1;
    
    if (algorithm == 1) { //Uniform Cost Search
        h_n = hParent + 1;
    } 
    else if (algorithm == 2) { //A* with the Misplaced Tile heuristic
        int misplaced = 0;
        for (int i = 0; i < state.size(); i++) {
            if (state.at(i) != i + 1 && state.at(i) != 0) {
                misplaced++;
            }
        }
        h_n = misplaced + hParent + 1;
    } 
    else if (algorithm == 3) { //A* with the Manhattan Distance heuristic
        h_n = manhattanDistance() + hParent + 1;
    }
}

//returns position of 0 in the node
int node::zeroLocator() {
    for (int i = 0; i < 9; i++) {
        if (state.at(i) == 0) {
            return i;
        }
    }
    return -1;
}

//calls the heuristic function for new node created when positions are swapped
void node::tradePos(int pos1, int pos2) {
    int temp;
    bool closeMe = 0;
    vector<int> tempVec = state;
    
    temp = tempVec.at(pos1);
    tempVec.at(pos1) = tempVec.at(pos2);
    tempVec.at(pos2) = temp;
    node newNode(tempVec);
    newNode.queueingFunction(algorithm, depth);
    
    for(int i = 0; i < discoveredNodes.size(); i++) { //
        if(discoveredNodes.at(i) == newNode.state) {
            closeMe = 1;
        }
    }
    for(int i = 0; i < closedNodes.size(); i++) {
        if(closedNodes.at(i) == newNode.state) {
            closeMe = 1;    
        }
    }
    
    if(closeMe) {
        closedNodes.push_back(newNode.state);
    }
    else {
        heap.push(newNode);
        compareQueue();
        discoveredNodes.push_back(newNode.state);
    }
}

//expands the given node, deciding which positions to swap
void node::expandNode() {
    cout << "The best state to expand with a g(n) = " << this->depth;
    cout << " and a h(n) = " << this->h_n - this->depth << " is..." << endl;
    printPuzzle(*this);

    int zeroPos = zeroLocator();
    chooseTradePos(zeroPos);

    for(int i = 0; i < discoveredNodes.size(); i++) { 
        if(discoveredNodes.at(i) == state) { //check if current node is an existing state
            closedNodes.push_back(state); //add current node to list of closedNodes states
            discoveredNodes.erase(discoveredNodes.begin() + i); //remove current node from list of discovered states
        }
    }
    expandedNodes.push_back(state); //add current node to list of expanded states
}

void node::chooseTradePos(int zeroPos) {
    if(zeroPos >= 0 && zeroPos <= 5) { //excluding bottom row positions
        tradePos(zeroPos, zeroPos + 3); //move zero down,
    } 
    if(zeroPos >= 3 && zeroPos <= 8) { //excluding top row positions
        tradePos(zeroPos, zeroPos - 3); //move zero up
    } 
    if(zeroPos == 0 || zeroPos == 1 || zeroPos == 3 || zeroPos == 4 || zeroPos == 6 || zeroPos == 7) { //excluding rightmost column positions
        tradePos(zeroPos, zeroPos + 1); //move zero right
    } 
    if(zeroPos == 1 || zeroPos == 2 || zeroPos == 4 || zeroPos == 5 || zeroPos == 7 || zeroPos == 8) { //excluding leftmost column positions
        tradePos(zeroPos, zeroPos - 1); //move zero left
    }
}

void node::setupCurrState(vector<manhattans> &nodePos) {
    nodePos.push_back(manhattans(state.at(0), 0, 0));
    nodePos.push_back(manhattans(state.at(1), 0, 1));
    nodePos.push_back(manhattans(state.at(2), 0, 2));
    nodePos.push_back(manhattans(state.at(3), 1, 0));
    nodePos.push_back(manhattans(state.at(4), 1, 1));
    nodePos.push_back(manhattans(state.at(5), 1, 2));
    nodePos.push_back(manhattans(state.at(6), 2, 0));
    nodePos.push_back(manhattans(state.at(7), 2, 1));
    nodePos.push_back(manhattans(state.at(8), 2, 2));
}

//compares current node to goal, deques node if not found
void node::dequeNode() {
    node goalNode = setupPuzzleGoalState();
    
    while(!heap.empty()) { //makes sure we expand all the nodes in the priority queue before we clear it
        node expandMe = heap.top();
        heap.pop();
        if (goalNode.state == expandMe.state) {
            cout << "Goal!!\n";
            printPuzzle(goalNode);
            depthResult = "The depth of the goal node was " + to_string(expandMe.depth - 1) + ".\n";
            break;
        }
        expandMe.expandNode();
    }
}

//BEGIN FUNCTION DEFINITIONS FOR MANHATTANS STRUCT

manhattans::manhattans(int num, int x, int y) {
        this->num = num;
        this->x = x;
        this->y = y;
}

int manhattans::getNum() {
        return this->num;
}

//takes in user input, then creates the first node and calls dequeNode() on that node
void setup() {
    int puzzleType;
    node starting;

    cout << "Welcome to the 8-puzzle solver!" << endl;
    cout << "Type '1' to use a default puzzle or '2' to enter your own: ";

    do {
        cin >> puzzleType;
        if (puzzleType == 1) {
            starting = defaultPuzzleSetup();
            break;
        }
        else if (puzzleType == 2) {
            starting = customPuzzleSetup();
            break;
        }
        cout << puzzleType << " is invalid. Enter '1' or '2': ";
    } while(puzzleType != 1 && puzzleType != 2);
    
    chooseAlgorithm();
    starting.queueingFunction(algorithm, 0);
    heap.push(starting);
    compareQueue();
    
    discoveredNodes.push_back(starting.state);
    starting.dequeNode();
}

int main() {
    setup();
    displayResults();
    return 0;
}

//BEGIN HELPER FUNCTION DEFINITIONS

//used in priority queue declaration
bool operator>(const node& lhs, const node& rhs) {
  return lhs.h_n > rhs.h_n;
}

//used in setup()
void chooseAlgorithm() {
    do {
        if(algorithm) {
            cout << "You entered an invalid number. Please select one of the options below.\n";
        }
        cout << "Enter your choice of algorithm:" << endl;
        cout << "1. Uniform Cost Search\n2. A* with the Misplaced Tile Heuristic\n3. A* with the Manhattan Distance Heuristic\n";
        cin >> algorithm;
        cout << endl;
    } while(algorithm < 1 || algorithm > 3);
}

//used in expandNode(), dequeNode(), defaultPuzzleSetup(), and customPuzzleSetup()
void printPuzzle(node currNode) {
    for (int i = 0; i < 9; i++) {
        if (i % 3 == 0) {
            cout << endl;
        }
        cout << currNode.state.at(i) << " ";
    }
    cout << "\n\n";
}

//used in setup()
node defaultPuzzleSetup() {
    vector<int> input;
    input.push_back(1);
    input.push_back(2);
    input.push_back(3);
    input.push_back(4);
    input.push_back(0);
    input.push_back(6);
    input.push_back(7);
    input.push_back(5);
    input.push_back(8);
    
    node initialPuzzle(input);
    printPuzzle(initialPuzzle);
    return initialPuzzle;
}

//used in setup()
node customPuzzleSetup() {
    int pos1, pos2, pos3, pos4, pos5, pos6, pos7, pos8, pos9;
    vector<int> input;
    bool validInput = true;

    do {
        cout << "Enter the first row, use space or tabs between numbers: ";
        cin >> pos1 >> pos2 >> pos3;
        input.push_back(pos1);
        input.push_back(pos2);
        input.push_back(pos3);
        cout << "Enter the second row, use space or tabs between numbers: ";
        cin >> pos4 >> pos5 >> pos6;
        input.push_back(pos4);
        input.push_back(pos5);
        input.push_back(pos6);
        cout << "Enter the third row, use space or tabs between numbers: ";
        cin >> pos7 >> pos8 >> pos9;
        input.push_back(pos7);
        input.push_back(pos8);
        input.push_back(pos9);

        for(int i = 0; i < input.size(); i++){
            if (input.at(i) < 0 || input.at(i) > 8) {
                cout << "You have inputted an invalid number. Please try again using values 0 through 8 only.\n";
                validInput = false;
                node invalidState(input);
                printPuzzle(invalidState);
            }
        }
    } while(!validInput);

    node initialPuzzle(input);
    printPuzzle(initialPuzzle);
    return initialPuzzle;
}

//used in main()
void displayResults() {
    cout << "To solve this problem the search algorithm expanded a total of " << expandedNodes.size() << " nodes.\n";
    cout << "The maximum number of nodes in the queue at any one time was " << maxQSize << ".\n";
    cout << depthResult;
}

//used in node's manhattanDistance()
void setupManhattanGoalState(vector<manhattans> &goalState) {
    goalState.push_back(manhattans(1, 0, 0));
    goalState.push_back(manhattans(2, 0, 1));
    goalState.push_back(manhattans(3, 0, 2));
    goalState.push_back(manhattans(4, 1, 0));
    goalState.push_back(manhattans(5, 1, 1));
    goalState.push_back(manhattans(6, 1, 2));
    goalState.push_back(manhattans(7, 2, 0));
    goalState.push_back(manhattans(8, 2, 1));
    goalState.push_back(manhattans(0, 2, 2));
}

//used in node's dequeNode()
node setupPuzzleGoalState() {
    vector<int> goal;
    goal.push_back(1);
    goal.push_back(2);
    goal.push_back(3);
    goal.push_back(4);
    goal.push_back(5);
    goal.push_back(6);
    goal.push_back(7);
    goal.push_back(8);
    goal.push_back(0);
    node goalNode(goal);
    return goalNode;
}

//used in setup()
void compareQueue() {
    if (heap.size() > maxQSize) {
        maxQSize = heap.size();
    }
}