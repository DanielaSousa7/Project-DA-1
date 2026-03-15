#include "Menu.h"
#include <iostream>
#include <iomanip>
#include <limits>

using namespace std;

// ─── Utilities ────────────────────────────────────────────────────────────────

static void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static void pause() {
    cout << "\nPress Enter to continue...";
    clearInput();
}

static void printHeader(const string& title) {
    const int W = 60;
    cout << "\n" << string(W, '=') << "\n";
    int pad = (W - (int)title.size()) / 2;
    cout << string(max(0, pad), ' ') << title << "\n";
    cout << string(W, '=') << "\n";
}

static void printSep() {
    cout << string(60, '-') << "\n";
}

static int readInt() {
    int c;
    if (!(cin >> c)) { clearInput(); return -1; }
    clearInput();
    return c;
}

// ─── Data display ─────────────────────────────────────────────────────────────

static void showSubmissions(const ProblemInstance& inst) {
    printHeader("Papers (" + to_string(inst.submissions.size()) + ")");
    cout << left << setw(6) << "ID" << setw(36) << "Title"
         << setw(10) << "Primary" << "Secondary\n";
    printSep();
    for (const auto& s : inst.submissions) {
        string t = s.title.size() > 33 ? s.title.substr(0, 30) + "..." : s.title;
        cout << setw(6) << s.id << setw(36) << t << setw(10) << s.primaryTopic;
        if (s.secondaryTopic) cout << s.secondaryTopic;
        cout << "\n";
    }
}

static void showReviewers(const ProblemInstance& inst) {
    printHeader("Reviewers (" + to_string(inst.reviewers.size()) + ")");
    cout << left << setw(6) << "ID" << setw(28) << "Name"
         << setw(10) << "Primary" << "Secondary\n";
    printSep();
    for (const auto& r : inst.reviewers) {
        cout << setw(6) << r.id << setw(28) << r.name << setw(10) << r.primaryExpertise;
        if (r.secondaryExpertise) cout << r.secondaryExpertise;
        cout << "\n";
    }
}

static void showParameters(const ProblemInstance& inst) {
    printHeader("Parameters & Control");
    cout << "  MinReviewsPerSubmission    : " << inst.params.minReviewsPerSubmission    << "\n"
         << "  MaxReviewsPerReviewer      : " << inst.params.maxReviewsPerReviewer      << "\n"
         << "  PrimaryReviewerExpertise   : " << inst.params.primaryReviewerExpertise   << "\n"
         << "  SecondaryReviewerExpertise : " << inst.params.secondaryReviewerExpertise << "\n"
         << "  PrimarySubmissionDomain    : " << inst.params.primarySubmissionDomain    << "\n"
         << "  SecondarySubmissionDomain  : " << inst.params.secondarySubmissionDomain  << "\n";
    printSep();
    cout << "  GenerateAssignments : " << inst.control.generateAssignments << "\n"
         << "  RiskAnalysis        : " << inst.control.riskAnalysis        << "\n"
         << "  OutputFileName      : " << inst.control.outputFileName      << "\n";
}

static void showGraphInfo(const Application& app) {
    const auto& inst = app.getInstance();
    printHeader("Flow Network Structure");

    int mode = (inst.control.generateAssignments == 0) ? 1 : inst.control.generateAssignments;
    FlowNetwork fn(inst, mode);
    const auto& g = fn.getGraph();
    int ns = (int)inst.submissions.size(), nr = (int)inst.reviewers.size(), ne = 0;
    for (auto* v : g.getVertexSet()) ne += (int)v->getAdj().size();

    printSep();
    cout << "  Node layout:\n"
         << "    [0]            Source\n"
         << "    [1 .. " << setw(3) << ns << "]      Papers    (" << ns << " nodes)\n"
         << "    [" << setw(3) << ns+1 << " .. " << setw(3) << ns+nr << "]    Reviewers (" << nr << " nodes)\n"
         << "    [" << setw(3) << ns+nr+1 << "]           Sink\n";
    printSep();
    cout << "  Total vertices         : " << g.getNumVertex() << "\n"
         << "  Total edges (+ reverse): " << ne               << "\n"
         << "  Active mode            : " << mode             << "\n";
    printSep();
    cout << "  Edge capacities:\n"
         << "    Source  -> Paper_i  : " << inst.params.minReviewsPerSubmission << " (min reviews needed)\n"
         << "    Paper_i -> Rev_j    : 1  (if domains match under mode " << mode << ")\n"
         << "    Rev_j   -> Sink     : " << inst.params.maxReviewsPerReviewer   << " (max reviews allowed)\n";
}

// ─── Assignment display ───────────────────────────────────────────────────────

static void displayAssignmentResult(const FlowResult& r, const ProblemInstance& inst, int mode) {
    printHeader("Assignment — Mode " + to_string(mode));
    switch (mode) {
        case 1: cout << "  Matching: primary paper domain <-> primary reviewer expertise\n"; break;
        case 2: cout << "  Matching: (primary or secondary) paper <-> primary reviewer expertise\n"; break;
        case 3: cout << "  Matching: all combinations of primary and secondary domains\n"; break;
    }
    cout << "  Min reviews per paper    : " << inst.params.minReviewsPerSubmission << "\n"
         << "  Max reviews per reviewer : " << inst.params.maxReviewsPerReviewer   << "\n";
    printSep();

    int target = (int)inst.submissions.size() * inst.params.minReviewsPerSubmission;

    if (r.success) {
        cout << "  RESULT: SUCCESS — " << r.totalFlow << " review(s) assigned (target: " << target << ")\n";
        printSep();
        cout << left << setw(16) << "  Paper ID" << setw(14) << "Reviewer ID" << "Domain\n";
        printSep();
        for (const auto& a : r.assignments)
            cout << "  " << setw(14) << a.submissionId
                         << setw(14) << a.reviewerId
                         << a.matchDomain << "\n";
    } else {
        cout << "  RESULT: INCOMPLETE — " << r.totalFlow << " of " << target << " assigned\n";
        printSep();
        cout << left << setw(16) << "  Paper ID" << setw(12) << "Domain" << "Missing\n";
        printSep();
        for (const auto& [sub, dom, cnt] : r.missing)
            cout << "  " << setw(14) << sub << setw(12) << dom << cnt << "\n";
    }
}

// ─── Risk Analysis display ────────────────────────────────────────────────────

static void displayRiskResult(const RiskResult& result, int R, int K) {
    // Compute C(R, K) to show how many groups were tested
    long long combos = 1;
    for (int i = 0; i < K && i < R; ++i) combos = combos * (R - i) / (i + 1);

    printHeader("Risk Analysis — K=" + to_string(K));
    cout << "  Groups tested : C(" << R << "," << K << ") = " << combos << "\n";
    printSep();

    if (!result.anyRisky) {
        cout << "  RESULT: ROBUST — assignment is still possible even if\n"
             << "  any " << K << " reviewer(s) are simultaneously absent.\n";
    } else {
        cout << "  RESULT: " << result.riskySets.size() << " RISKY GROUP(S) FOUND\n"
             << "  If any of the following group(s) is absent,\n"
             << "  assignment becomes IMPOSSIBLE:\n\n";
        printSep();
        cout << "  #Risk Analysis: " << K << "\n";
        for (const auto& group : result.riskySets) {
            cout << "  ";
            for (int i = 0; i < (int)group.size(); ++i) {
                if (i) cout << ", ";
                cout << group[i];
            }
            cout << "\n";
        }
        printSep();
    }
}

// ─── Submenus ─────────────────────────────────────────────────────────────────

static void dataMenu(const Application& app) {
    while (true) {
        printHeader("Data Browser");
        cout << "  1. List papers\n"
             << "  2. List reviewers\n"
             << "  3. Parameters & control settings\n"
             << "  4. Flow network structure\n"
             << "  0. Back\n\nChoice: ";

        switch (readInt()) {
            case 1: showSubmissions(app.getInstance()); break;
            case 2: showReviewers(app.getInstance());   break;
            case 3: showParameters(app.getInstance());  break;
            case 4: showGraphInfo(app);                 break;
            case 0: return;
            default: cout << "  Invalid option.\n"; continue;
        }
        pause();
    }
}

static void assignmentMenu(Application& app) {
    while (true) {
        printHeader("Run Assignment");
        int cur = app.getInstance().control.generateAssignments;
        if (cur == 0) cur = 1;

        cout << "  Mode from file: " << cur << "\n\n"
             << "  1. Mode 1 — primary domains only                   (T2.1)\n"
             << "  2. Mode 2 — paper primary+secondary <-> reviewer primary\n"
             << "  3. Mode 3 — all domain combinations                (T2.4)\n"
             << "  4. Run with mode from file (mode " << cur << ")\n"
             << "  0. Back\n\nChoice: ";

        int c = readInt();
        if (c == 0) return;
        if (c < 1 || c > 4) { cout << "  Invalid option.\n"; continue; }

        int mode = (c == 4) ? cur : c;
        FlowResult result = app.runAssignment(mode);
        displayAssignmentResult(result, app.getInstance(), mode);

        // Write to file if GenerateAssignments is set and we used the file's mode
        if (c == 4 && app.getInstance().control.generateAssignments != 0) {
            printSep();
            try { app.writeAssignment(result); }
            catch (const exception& e) { cerr << "  [Warning] " << e.what() << "\n"; }
        }
        pause();
    }
}

static void riskMenu(Application& app) {
    while (true) {
        printHeader("Risk Analysis");
        int fileK = app.getInstance().control.riskAnalysis;

        cout << "  K from file: " << fileK << "\n\n"
             << "  1. K=1  — test each reviewer individually         (T2.2)\n"
             << "             O(R x V x E²)\n\n"
             << "  2. K>1  — test every group of K reviewers         (T2.3)\n"
             << "             O(C(R,K) x V x E²), you choose K\n\n"
             << "  3. Run with K from file (K=" << fileK << ")\n"
             << "  0. Back\n\nChoice: ";

        int c = readInt();
        int R = (int)app.getInstance().reviewers.size();

        switch (c) {
            case 1: {
                RiskResult r = app.runRiskAnalysis(1);
                displayRiskResult(r, R, 1);
                try { app.writeRisk(r); } catch (const exception& e) { cerr << "  [Warning] " << e.what() << "\n"; }
                pause(); break;
            }
            case 2: {
                cout << "  Enter K (must be > 1): ";
                int K = readInt();
                if (K < 2) { cout << "  Invalid.\n"; break; }
                RiskResult r = app.runRiskAnalysis(K);
                displayRiskResult(r, R, K);
                try { app.writeRisk(r); } catch (const exception& e) { cerr << "  [Warning] " << e.what() << "\n"; }
                pause(); break;
            }
            case 3:
                if (fileK == 0) { cout << "  RiskAnalysis=0 in file — nothing to run.\n"; pause(); }
                else {
                    RiskResult r = app.runRiskAnalysis(fileK);
                    displayRiskResult(r, R, fileK);
                    try { app.writeRisk(r); } catch (const exception& e) { cerr << "  [Warning] " << e.what() << "\n"; }
                    pause();
                }
                break;
            case 0: return;
            default: cout << "  Invalid option.\n";
        }
    }
}

// ─── Main menu ────────────────────────────────────────────────────────────────

void runInteractiveMenu(Application& app) {
    while (true) {
        printHeader("Scientific Conference Organization Tool");
        printSep();

        if (app.isLoaded()) {
            const auto& inst = app.getInstance();
            cout << "  File      : " << inst.control.outputFileName << "\n"
                 << "  Papers    : " << inst.submissions.size()
                 << "   Reviewers: " << inst.reviewers.size()
                 << "   Min/Max reviews: " << inst.params.minReviewsPerSubmission
                 << "/" << inst.params.maxReviewsPerReviewer << "\n";
        } else {
            cout << "  No file loaded.\n";
        }

        printSep();
        cout << "  1. Load input file (.csv)\n"
             << "  2. Browse data\n"
             << "  3. Run assignment\n"
             << "       Mode 1: primary domains only                  (T2.1)\n"
             << "       Mode 3: primary + secondary domains            (T2.4)\n"
             << "  4. Run risk analysis\n"
             << "       K=1: what if one reviewer is absent?          (T2.2)\n"
             << "       K>1: what if K reviewers are absent?          (T2.3)\n"
             << "  0. Exit\n";
        printSep();
        cout << "Choice: ";

        int c = readInt();

        if (c >= 2 && c <= 4 && !app.isLoaded()) {
            cout << "\n  [No file loaded — use option 1 first]\n";
            pause(); continue;
        }

        switch (c) {
            case 1: {
                cout << "\nEnter path to .csv file: ";
                string path;
                getline(cin, path);
                while (!path.empty() && (path.front() == '"' || path.front() == ' ')) path.erase(path.begin());
                while (!path.empty() && (path.back()  == '"' || path.back()  == ' ')) path.pop_back();
                try {
                    app.loadFile(path);
                    const auto& inst = app.getInstance();
                    cout << "\n  Loaded " << inst.submissions.size() << " paper(s) and "
                         << inst.reviewers.size() << " reviewer(s).\n";
                } catch (const exception& e) {
                    cerr << "\n  [Error] " << e.what() << "\n";
                }
                pause(); break;
            }
            case 2: dataMenu(app);      break;
            case 3: assignmentMenu(app); break;
            case 4: riskMenu(app);      break;
            case 0: cout << "\nGoodbye!\n"; return;
            default: cout << "  Invalid option.\n"; pause();
        }
    }
}
