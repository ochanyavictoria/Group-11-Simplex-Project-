/** Group 11
 * 
 * Matric No.	Fullname
 * U22CS1095	Nwuba Richard Chukwudalu
 * U22CS1096	Odiase Brian Ehimare
 * U22CS1097	Okewu Victoria Ochanya
 * U22CS1098	Oloruntoba Favour Oluwatosin
 * U22CS1099	Olukayode Divine Oladotun
 * U22CS1100	Omale Ojoajogwu Joseph
 * U22CS1103	Oluwaseyi Adeniyi Oyekunle
 * U22CS1093	Nanpon Manfa Elias
 * U22CS1119	Yakubu Zaniab Ohunene
 * 
 * Title: C++ Implementation of the Simplex Algorithm for solving LPPs**/



#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>

using namespace std;

class Simplex {
private:
    int numVariables;          // Number of decision variables
    int numConstraints;        // Number of constraints
    vector<vector<double>> tableau; // Simplex tableau
    vector<int> basicVariables; // Indices of basic variables
    bool isMaximization;       // True if maximization, false if minimization

public:
    // Constructor to initialize the Simplex object
    Simplex(int numVars, int numCons, const vector<vector<double>>& constraints, 
            const vector<double>& rhs, const vector<double>& objFunc, bool isMax)
        : numVariables(numVars), numConstraints(numCons), isMaximization(isMax) {
        
        // Initialize the tableau
        initializeTableau(constraints, rhs, objFunc);
    }

    // Initialize the simplex tableau
    void initializeTableau(const vector<vector<double>>& constraints, 
                           const vector<double>& rhs, const vector<double>& objFunc) {
        // Tableau has (numConstraints + 1) rows and (numVariables + numConstraints + 1) columns
        tableau.resize(numConstraints + 1, vector<double>(numVariables + numConstraints + 1, 0.0));

        // Fill the constraint coefficients and slack variables
        for (int i = 0; i < numConstraints; ++i) {
            for (int j = 0; j < numVariables; ++j) {
                tableau[i][j] = constraints[i][j];
            }
            // Add slack variables
            tableau[i][numVariables + i] = 1.0;
            // Set the right-hand side values
            tableau[i][numVariables + numConstraints] = rhs[i];
        }

        // Fill the objective function row
        for (int j = 0; j < numVariables; ++j) {
            tableau[numConstraints][j] = isMaximization ? -objFunc[j] : objFunc[j];
        }

        // Initialize basic variables (slack variables)
        basicVariables.resize(numConstraints);
        for (int i = 0; i < numConstraints; ++i) {
            basicVariables[i] = numVariables + i;
        }
    }

    // Print the tableau in a formatted manner
    void printTableau() const {
        cout << "\nCurrent Tableau:\n";
        cout << setw(10) << "Basic";
        for (int j = 0; j < numVariables + numConstraints; ++j) {
            cout << setw(10) << "x" << j + 1;
        }
        cout << setw(10) << "RHS" << endl;

        for (int i = 0; i < numConstraints; ++i) {
            cout << setw(10) << "x" << basicVariables[i] + 1;
            for (int j = 0; j < numVariables + numConstraints + 1; ++j) {
                cout << setw(10) << fixed << setprecision(2) << tableau[i][j];
            }
            cout << endl;
        }

        cout << setw(10) << "Z";
        for (int j = 0; j < numVariables + numConstraints + 1; ++j) {
            cout << setw(10) << fixed << setprecision(2) << tableau[numConstraints][j];
        }
        cout << endl;
    }

    // Compute Cj - Zj values
    void computeCjZj() {
        for (int j = 0; j < numVariables + numConstraints; ++j) {
            double sum = 0.0;
            for (int i = 0; i < numConstraints; ++i) {
                sum += tableau[i][j] * tableau[numConstraints][basicVariables[i]];
            }
            tableau[numConstraints][j] = (isMaximization ? -tableau[numConstraints][j] : tableau[numConstraints][j]) - sum;
        }
    }

    // Find the pivot column (most negative Cj - Zj for maximization, most positive for minimization)
    int findPivotColumn() const {
        int pivotCol = -1;
        double minMaxVal = isMaximization ? -numeric_limits<double>::infinity() : numeric_limits<double>::infinity();

        for (int j = 0; j < numVariables + numConstraints; ++j) {
            if (isMaximization ? tableau[numConstraints][j] < 0 : tableau[numConstraints][j] > 0) {
                if (isMaximization ? tableau[numConstraints][j] < minMaxVal : tableau[numConstraints][j] > minMaxVal) {
                    minMaxVal = tableau[numConstraints][j];
                    pivotCol = j;
                }
            }
        }
        return pivotCol;
    }

    // Find the pivot row (minimum ratio test)
    int findPivotRow(int pivotCol) const {
        int pivotRow = -1;
        double minRatio = numeric_limits<double>::infinity();

        for (int i = 0; i < numConstraints; ++i) {
            if (tableau[i][pivotCol] > 0) {
                double ratio = tableau[i][numVariables + numConstraints] / tableau[i][pivotCol];
                if (ratio < minRatio) {
                    minRatio = ratio;
                    pivotRow = i;
                }
            }
        }
        return pivotRow;
    }

    // Perform the pivot operation
    void pivot(int pivotRow, int pivotCol) {
        double pivotElement = tableau[pivotRow][pivotCol];

        // Normalize the pivot row
        for (int j = 0; j < numVariables + numConstraints + 1; ++j) {
            tableau[pivotRow][j] /= pivotElement;
        }

        // Update other rows
        for (int i = 0; i < numConstraints + 1; ++i) {
            if (i != pivotRow) {
                double factor = tableau[i][pivotCol];
                for (int j = 0; j < numVariables + numConstraints + 1; ++j) {
                    tableau[i][j] -= factor * tableau[pivotRow][j];
                }
            }
        }

        // Update basic variables
        basicVariables[pivotRow] = pivotCol;
    }

    // Solve the LPP using the Simplex algorithm
    void solve() {
        int iteration = 0;
        while (true) {
            cout << "\nIteration " << ++iteration << ":\n";
            printTableau();

            computeCjZj();
            int pivotCol = findPivotColumn();
            if (pivotCol == -1) {
                cout << "\nOptimal solution found!\n";
                break;
            }

            int pivotRow = findPivotRow(pivotCol);
            if (pivotRow == -1) {
                cout << "\nProblem is unbounded!\n";
                return;
            }

            pivot(pivotRow, pivotCol);
        }

        // Print the optimal solution
        cout << "\nOptimal Solution:\n";
        for (int i = 0; i < numVariables; ++i) {
            bool isBasic = false;
            int basicRow = -1;
            for (int j = 0; j < numConstraints; ++j) {
                if (basicVariables[j] == i) {
                    isBasic = true;
                    basicRow = j;
                    break;
                }
            }
            if (isBasic) {
                cout << "x" << i + 1 << " = " << tableau[basicRow][numVariables + numConstraints] << endl;
            } else {
                cout << "x" << i + 1 << " = 0\n";
            }
        }
        cout << "Z = " << (isMaximization ? -tableau[numConstraints][numVariables + numConstraints] : tableau[numConstraints][numVariables + numConstraints]) << endl;
    }
};

int main() {
    int numVariables, numConstraints;
    bool isMaximization;

    // Input the number of variables and constraints
    cout << "Enter the number of decision variables: ";
    cin >> numVariables;
    cout << "Enter the number of constraints: ";
    cin >> numConstraints;

    // Input the constraint coefficients
    vector<vector<double>> constraints(numConstraints, vector<double>(numVariables));
    cout << "Enter the constraint coefficients (row by row):\n";
    for (int i = 0; i < numConstraints; ++i) {
        for (int j = 0; j < numVariables; ++j) {
            cin >> constraints[i][j];
        }
    }

    // Input the right-hand side values
    vector<double> rhs(numConstraints);
    cout << "Enter the right-hand side values:\n";
    for (int i = 0; i < numConstraints; ++i) {
        cin >> rhs[i];
    }

    // Input the objective function coefficients
    vector<double> objFunc(numVariables);
    cout << "Enter the objective function coefficients:\n";
    for (int j = 0; j < numVariables; ++j) {
        cin >> objFunc[j];
    }

    // Input whether it's a maximization or minimization problem
    cout << "Is it a maximization problem? (1 for Yes, 0 for No): ";
    cin >> isMaximization;

    // Create an instance of the Simplex class and solve the problem
    Simplex simplex(numVariables, numConstraints, constraints, rhs, objFunc, isMaximization);
    simplex.solve();

    return 0;
}
