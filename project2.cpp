#include <iostream>
using namespace std;
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <climits>
#include <iomanip>

vector<double> loadData;
float overallBestForward = 0;
float overallBestBackward = 0;


struct node {
    int classification;
    vector<double> features;
    
    node() {};
    node(int classType, int location) {
        classification = classType;
        location += 1;

        for (int i = location; i < loadData.size(); i++) {
            if (loadData.at(i) == 1 || loadData.at(i) == 2) {
                break;
            } else {
                features.push_back(loadData.at(i));
            }
        }
    }
};

void normalize(vector<node>& data) {
    vector<double> featureMin;
    vector<double> featureMax;

    cout << "Normalizing data... ";
    
    // Initializing vector to first value of each feature
    for (int i = 0; i < data.at(0).features.size(); i++) {
        featureMin.push_back(data.at(0).features.at(i));
        featureMax.push_back(data.at(0).features.at(i));
    }

    for (int i = 0; i < data.size(); i++) {
        for (int j = 0; j < data.at(0).features.size(); j++) {
            if (data.at(i).features.at(j) < featureMin.at(j)) {
                featureMin.at(j) = data.at(i).features.at(j);
            }
            if (data.at(i).features.at(j) > featureMax.at(j)) {
                featureMax.at(j) = data.at(i).features.at(j);
            }
        }
    }

    for (int i = 0; i < data.size(); i++) {
        for (int j = 0; j < data.at(0).features.size(); j++) {
            data.at(i).features.at(j) = (data.at(i).features.at(j) - featureMin.at(j)) / (featureMax.at(j) - featureMin.at(j));
        }
    }

    cout << "Done!\n\n";

}

float leave_one_out(vector<node> data, vector<int> featureSet, int featureToAdd) {
    int numCorrect = 0;
    float accuracy = 0;

    for (int i = 0; i < data.size(); i++) {
        double bestSoFar = INT_MAX;
        int bestSoFarLoc = 0;

        for (int j = 0; j < data.size(); j++) {
            double distance = 0;
            if (i != j) {

                    for (int k = 0; k < featureSet.size(); k++) {
                        distance += pow( data.at(i).features.at(featureSet.at(k)) - data.at(j).features.at(featureSet.at(k)), 2);
                    }

                    distance += pow( data.at(i).features.at(featureToAdd) - data.at(j).features.at(featureToAdd), 2);

                distance = sqrt(distance);

                if (distance < bestSoFar) {
                    bestSoFar = distance;
                    bestSoFarLoc = j;
                }
            }
        }

        if (data.at(i).classification == data.at(bestSoFarLoc).classification) {
            numCorrect++;
        } 
    }
    accuracy = float(numCorrect) / data.size();
    return accuracy;
}

float leave_one_out_backward(vector<node> data, vector<int> featureSet, int featureToRemove) {
    int numCorrect = 0;
    float accuracy = 0;
    vector<int> newFeatureSet;

    for (int i = 0; i < featureSet.size(); i++) {
        if (featureSet.at(i) != featureToRemove) {
            newFeatureSet.push_back(featureSet.at(i));
        }
    }

    for (int i = 0; i < data.size(); i++) {
        double bestSoFar = INT_MAX;
        int bestSoFarLoc = 0;

        for (int j = 0; j < data.size(); j++) {
            double distance = 0;
            if (i != j) {

                for (int k = 0; k < newFeatureSet.size(); k++) {
                        distance += pow( data.at(i).features.at(newFeatureSet.at(k)) - data.at(j).features.at(newFeatureSet.at(k)), 2);
                }

                distance = sqrt(distance);

                if (distance < bestSoFar) {
                    bestSoFar = distance;
                    bestSoFarLoc = j;
                }
            }
        }

        if (data.at(i).classification == data.at(bestSoFarLoc).classification) {
            numCorrect++;
        } 
    }
    accuracy = float(numCorrect) / data.size();
    return accuracy;
}

void forward_selection(vector<node> data) {
    vector<int> featureSet;
    vector<int> bestFeatures;
    float overallBest = 0;
    bool decrease = 0;
    cout << "Beginning search.\n\n";

    for (int i = 0; i < data.at(0).features.size(); i++) {
        int featureToAdd = 0;
        float bestSoFar = 0;

        for (int j = 0; j < data.at(0).features.size(); j++) {
            if (find(featureSet.begin(), featureSet.end(), j) == featureSet.end()) {
                float accuracy = leave_one_out(data, featureSet, j);

                if (accuracy > bestSoFar) {
                    bestSoFar = accuracy;
                    featureToAdd = j;
                }
            }
        }

        cout << "\n";
        featureSet.push_back(featureToAdd);

        if (bestSoFar > overallBest) {
            overallBest = bestSoFar;
            bestFeatures = featureSet;
        } else if (bestSoFar < overallBest && !decrease) {
            cout << "WARNING: Accuracy has decreased. Continuing search in case of local maxima.\n\n";
            decrease = 1;
        }

        cout << "Feature set {";
        for (int x = 0; x < featureSet.size() - 1; x++) {
            cout << featureSet.at(x) + 1 << ", ";
        }
        cout << featureSet.at(featureSet.size() - 1) + 1;
        cout << "} was best, accuracy is " << setprecision(3) << bestSoFar * 100 << "%\n\n";
    }

    cout << "Finished search. The best feature subset is {";
    for (int i = 0; i < bestFeatures.size() - 1; i++) {
        cout << bestFeatures.at(i) + 1 << ", ";
    }
    cout << bestFeatures.at(bestFeatures.size() - 1) + 1 << "}, which has an accuracy of " << setprecision(3) << overallBest * 100 << "%\n\n";
}

void backward_elimination(vector<node> data) {
    vector<int> featureSet;
    vector<int> bestFeatures;
    float overallBest = 0;
    bool decrease = 0;
    bool outputCheck = 0;
    cout << "Beginning search.\n\n";

    for (int i = 0; i < data.at(0).features.size(); i++) {
        featureSet.push_back(i);
    }

    cout << "Starting feature set: ";
    for (int i = 0; i < featureSet.size(); i++) {
        cout << featureSet.at(i) + 1 << " ";
    }
    cout << "\n\n";

    for (int i = 0; i < data.at(0).features.size(); i++) {
        int featureToRemove = 0;
        float bestSoFar = 0;

        for (int j = 0; j < data.at(0).features.size(); j++) {
            if (find(featureSet.begin(), featureSet.end(), j) != featureSet.end() && featureSet.size() > 1) {
                float accuracy = leave_one_out_backward(data, featureSet, j);
                cout << "   Using feature(s) {";

                for (int x = 0; x < featureSet.size() - 2; x++) {
                    if (featureSet.at(x) != j) {
                        cout << featureSet.at(x) + 1 << ", ";
                    }
                }
                if (featureSet.at(featureSet.size() - 2) == j) {
                    cout << featureSet.at(featureSet.size() - 1) + 1;
                } else if (featureSet.at(featureSet.size() - 1) == j) {
                    cout << featureSet.at(featureSet.size() - 2) + 1;
                } else {
                    cout << featureSet.at(featureSet.size() - 2) + 1 << ", ";
                    cout << featureSet.at(featureSet.size() - 1) + 1;
                }
                cout << "} accuracy is " << setprecision(3) << accuracy * 100 << "%\n";

                if (accuracy > bestSoFar) {
                    bestSoFar = accuracy;
                    featureToRemove = j;
                }
            }
        }

        cout << "\n";

        for (int i = 0; i < featureSet.size(); i++) {
            if (featureSet.at(i) == featureToRemove) {
                featureSet.erase(featureSet.begin() + i);
                break;
            }
        }

        if (bestSoFar > overallBest) {
            overallBest = bestSoFar;
            bestFeatures = featureSet;
        } else if (bestSoFar < overallBest && !decrease) {
            cout << "WARNING: Accuracy has decreased. Continuing search in case of local maxima.\n\n";
            decrease = 1;
        }

        if (featureSet.size() != 1 || (featureSet.size() == 1 && !outputCheck)) {
            if (featureSet.size() == 1) {
                outputCheck = 1;
            }
            cout << "Feature set {";
            for (int x = 0; x < featureSet.size() - 1; x++) {
                cout << featureSet.at(x) + 1 << ", ";
            }
            cout << featureSet.at(featureSet.size() - 1) + 1;
            cout << "} was best, accuracy is " << setprecision(10) << bestSoFar * 100 << "%\n\n";
        }

    }

    cout << "Finished search. The best feature subset is {";
    for (int i = 0; i < bestFeatures.size() - 1; i++) {
        cout << bestFeatures.at(i) + 1 << ", ";
    }
    cout << bestFeatures.at(bestFeatures.size() - 1) + 1 << "} which has an accuracy of " << overallBest * 100 << "%\n\n";

}

vector<int> custom_algorithm(vector<node> data) {
    vector<int> featureSetForward;
    vector<int> featureSetBackward;
    vector<int> bestFeaturesForward;
    vector<int> bestFeaturesBackward;
    vector<vector<int> > allBackward;
    vector<vector<int> > allForward;
    bool outputCheck = 0;

    for (int i = 0; i < data.at(0).features.size(); i++) {
        featureSetBackward.push_back(i);
    }

    for (int i = 0; i < data.at(0).features.size(); i++) {
        int featureToAdd = 0;
        int featureToRemove = 0;
        float bestSoFarForward = 0;
        float bestSoFarBackward = 0;

        for (int j = 0; j < data.at(0).features.size(); j++) {
            // doing forward selection
            if (find(featureSetForward.begin(), featureSetForward.end(), j) == featureSetForward.end()) {
                float accuracy = leave_one_out(data, featureSetForward, j);
                if (accuracy > bestSoFarForward) {
                    bestSoFarForward = accuracy;
                    featureToAdd = j;
                }
            }

            // doing backward elimination
            if (find(featureSetBackward.begin(), featureSetBackward.end(), j) != featureSetBackward.end() && featureSetBackward.size() > 1) {
                float accuracy = leave_one_out_backward(data, featureSetBackward, j);
                if (accuracy > bestSoFarBackward) {
                    bestSoFarBackward = accuracy;
                    featureToRemove = j;
                }
            }

        }

        cout << "\n";
        featureSetForward.push_back(featureToAdd);
        allForward.push_back(featureSetForward);
        if (bestSoFarForward > overallBestForward) {
            overallBestForward = bestSoFarForward;
            bestFeaturesForward = featureSetForward;
        } 
        for (int x = 0; x < featureSetBackward.size(); x++) {
            if (featureSetBackward.at(x) == featureToRemove) {
                featureSetBackward.erase(featureSetBackward.begin() + x);
                break;
            }
        }
        allBackward.push_back(featureSetBackward);
        for (int x = 0; x < allForward.size(); x++) {
            for (int y = 0; y < allBackward.size(); y++) {
                sort(allForward.at(x).begin(), allForward.at(x).end());
                sort(allBackward.at(y).begin(), allBackward.at(y).end());
                if (allForward.at(x) == allBackward.at(y)) {
                    return allForward.at(x);
                }
            }
        }

        if (bestSoFarBackward > overallBestBackward) {
            overallBestBackward = bestSoFarBackward;
            bestFeaturesBackward = featureSetBackward;
        } 

        cout << "Forward feature set {";
        for (int x = 0; x < featureSetForward.size() - 1; x++) {
            cout << featureSetForward.at(x) + 1 << ", ";
        }
        cout << featureSetForward.at(featureSetForward.size() - 1) + 1;
        cout << "} was best, accuracy is " << setprecision(3) << bestSoFarForward * 100 << "%\n\n";


        if (featureSetBackward.size() != 1) {
            if (featureSetBackward.size() == 1) {
                outputCheck = 1;
            }
            cout << "Backward feature set {";
            for (int x = 0; x < featureSetBackward.size() - 1; x++) {
                cout << featureSetBackward.at(x) + 1 << ", ";
            }
            cout << featureSetBackward.at(featureSetBackward.size() - 1) + 1;
            cout << "} was best, accuracy is " << setprecision(10) << bestSoFarBackward * 100 << "%\n\n";
        } else {
            cout << "Backward feature set {" << featureSetBackward.at(0) << "} was best, accuracy is " << setprecision(10) << bestSoFarBackward * 100 << "\n\n";
        }
    }
    if (overallBestForward > overallBestBackward) {
        return bestFeaturesForward;
    } else {
        return bestFeaturesBackward;
    }

}

int main() {
    vector<node> data;
    string fileName;
    int algChoice;
    ifstream inFile;
    double temp;

    cout << "Type in the name of the file you would like to test: " << endl;
    cin >> fileName;

    cout << "Type the number of the algorithm you want to run: " << endl;
    cout << "   1) Forward Selection" << endl;
    cout << "   2) Backward Elimination" << endl;
    cout << "   3) Custom Algorithm" << endl;
    cin >> algChoice;

    inFile.open(fileName);

    if (!inFile) {
        cout << "Unable to open file." << endl;
        exit(1);
    }

    while (inFile >> temp) {
        loadData.push_back(temp);
    }

    for (int i = 0; i < loadData.size(); i++) {
        if (loadData.at(i) == 1) {
            node newNode(1, i);
            data.push_back(newNode);
        } else if (loadData.at(i) == 2) {
            node newNode(2, i);
            data.push_back(newNode);
        }
    }

    normalize(data);
    
    if (algChoice == 1) {
        forward_selection(data);
    } else if (algChoice == 2) {
        backward_elimination(data);
    } else {
        vector<int> match = custom_algorithm(data);
        cout << "Finished search. The best feature subset is {";
        for (int i = 0; i < match.size() - 1; i++) {
            cout << match.at(i) + 1 << ", ";
        }
        cout << match.at(match.size() - 1) + 1 << "} which has an accuracy of "; 
        if (overallBestForward > overallBestBackward) {
            cout << overallBestForward * 100 << "%\n\n";
        } else {
            cout << overallBestBackward * 100 << "\n\n";
        }
    }

    inFile.close();
    return 0;
}