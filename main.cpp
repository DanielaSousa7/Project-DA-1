#include "Parser.h"
#include "FlowNetwork.h"
#include "RiskAnalysis.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <optional>

static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
static void pause() {
    std::cout << "\nPress Enter to continue...";
    clearInput();
}
static void printHeader(const std::string& title) {
    const int W = 60;
    std::cout << "\n" << std::string(W,'=') << "\n";
    int pad = (W - (int)title.size()) / 2;
    std::cout << std::string(std::max(0,pad),' ') << title << "\n";
    std::cout << std::string(W,'=') << "\n";
}
static void printSep() { std::cout << std::string(60,'-') << "\n"; }

// ─── Data display ─────────────────────────────────────────────────────────────

static void showSubmissions(const ProblemInstance& inst) {
    printHeader("Submissions (" + std::to_string(inst.submissions.size()) + ")");
    std::cout << std::left << std::setw(6)<<"ID" << std::setw(36)<<"Title"
              << std::setw(10)<<"Primary" << "Secondary\n";
    printSep();
    for (const auto& s : inst.submissions) {
        std::string t = s.title.size()>33 ? s.title.substr(0,30)+"..." : s.title;
        std::cout << std::setw(6)<<s.id << std::setw(36)<<t << std::setw(10)<<s.primaryTopic;
        if (s.secondaryTopic) std::cout << s.secondaryTopic;
        std::cout << "\n";
    }
}
static void showReviewers(const ProblemInstance& inst) {
    printHeader("Reviewers (" + std::to_string(inst.reviewers.size()) + ")");
    std::cout << std::left << std::setw(6)<<"ID" << std::setw(28)<<"Name"
              << std::setw(10)<<"Primary" << "Secondary\n";
    printSep();
    for (const auto& r : inst.reviewers) {
        std::cout << std::setw(6)<<r.id << std::setw(28)<<r.name << std::setw(10)<<r.primaryExpertise;
        if (r.secondaryExpertise) std::cout << r.secondaryExpertise;
        std::cout << "\n";
    }
}
static void showParameters(const ProblemInstance& inst) {
    printHeader("Parameters & Control");
    std::cout << "  MinReviewsPerSubmission    : " << inst.params.minReviewsPerSubmission   << "\n"
              << "  MaxReviewsPerReviewer      : " << inst.params.maxReviewsPerReviewer      << "\n"
              << "  PrimaryReviewerExpertise   : " << inst.params.primaryReviewerExpertise   << "\n"
              << "  SecondaryReviewerExpertise : " << inst.params.secondaryReviewerExpertise << "\n"
              << "  PrimarySubmissionDomain    : " << inst.params.primarySubmissionDomain    << "\n"
              << "  SecondarySubmissionDomain  : " << inst.params.secondarySubmissionDomain  << "\n";
    printSep();
    std::cout << "  GenerateAssignments : " << inst.control.generateAssignments << "\n"
              << "  RiskAnalysis        : " << inst.control.riskAnalysis        << "\n"
              << "  OutputFileName      : " << inst.control.outputFileName      << "\n";
}
static void showGraphInfo(const ProblemInstance& inst) {
    printHeader("Flow Network Structure");
    if (inst.submissions.empty()) { std::cout << "  No data loaded.\n"; return; }
    int mode = (inst.control.generateAssignments==0)?1:inst.control.generateAssignments;
    FlowNetwork fn(inst, mode);
    const auto& g = fn.getGraph();
    int ns=(int)inst.submissions.size(), nr=(int)inst.reviewers.size(), ne=0;
    for (auto* v : g.getVertexSet()) ne += (int)v->getAdj().size();
    printSep();
    std::cout << "  Node layout:\n"
              << "    [0]          Source\n"
              << "    [1 .. " << std::setw(3) << ns << "]    Submissions (" << ns << " nodes)\n"
              << "    [" << std::setw(3) << ns+1 << " .. " << std::setw(3) << ns+nr << "]  Reviewers   (" << nr << " nodes)\n"
              << "    [" << std::setw(3) << ns+nr+1 << "]         Sink\n";
    printSep();
    std::cout << "  Total vertices         : " << g.getNumVertex() << "\n"
              << "  Total edges (+ reverse): " << ne              << "\n"
              << "  Active assignment mode : " << mode            << "\n";
    printSep();
    std::cout << "  Edge capacities:\n"
              << "    Source  -> Sub_i  : " << inst.params.minReviewsPerSubmission << " (MinReviewsPerSubmission)\n"
              << "    Sub_i   -> Rev_j  : 1  (if domains match under mode " << mode << ")\n"
              << "    Rev_j   -> Sink   : " << inst.params.maxReviewsPerReviewer   << " (MaxReviewsPerReviewer)\n";
}

// ─── Assignment ───────────────────────────────────────────────────────────────

static void runAssignment(const ProblemInstance& inst, int modeOverride = -1) {
    int mode = (modeOverride > 0) ? modeOverride
             : (inst.control.generateAssignments == 0) ? 1
             : inst.control.generateAssignments;

    printHeader("Assignment — Mode " + std::to_string(mode));
    switch (mode) {
        case 1: std::cout << "  Matching: primary submission domain <-> primary reviewer expertise\n"; break;
        case 2: std::cout << "  Matching: (primary+secondary) submission <-> primary reviewer expertise\n"; break;
        case 3: std::cout << "  Matching: all domain combinations (primary & secondary both sides)\n"; break;
    }
    std::cout << "  Min reviews per submission : " << inst.params.minReviewsPerSubmission << "\n"
              << "  Max reviews per reviewer   : " << inst.params.maxReviewsPerReviewer   << "\n";
    printSep();
    std::cout << "  Running Edmonds-Karp Max-Flow...\n\n";

    FlowNetwork fn(inst, mode);
    const auto& r = fn.getResult();

    if (r.success) {
        std::cout << "  RESULT: SUCCESS — " << r.totalFlow << " review(s) assigned"
                  << " (target: " << inst.submissions.size()*inst.params.minReviewsPerSubmission << ")\n";
        printSep();
        std::cout << std::left << std::setw(16)<<"  SubmissionID" << std::setw(14)<<"ReviewerID" << "Domain\n";
        printSep();
        for (const auto& a : r.assignments)
            std::cout << "  " << std::setw(14)<<a.submissionId << std::setw(14)<<a.reviewerId << a.matchDomain << "\n";
    } else {
        int req = (int)inst.submissions.size()*inst.params.minReviewsPerSubmission;
        std::cout << "  RESULT: INCOMPLETE — " << r.totalFlow << " of " << req << " review(s) assigned\n";
        printSep();
        std::cout << std::left << std::setw(16)<<"  SubmissionID" << std::setw(12)<<"Domain" << "Missing\n";
        printSep();
        for (const auto& [sub,dom,cnt] : r.missing)
            std::cout << "  " << std::setw(14)<<sub << std::setw(12)<<dom << cnt << "\n";
    }

    if (inst.control.generateAssignments != 0 && modeOverride < 0) {
        printSep();
        try { writeAssignmentOutput(r, inst, inst.control.outputFileName); }
        catch (const std::exception& e) { std::cerr<<"  [Warning] "<<e.what()<<"\n"; }
    }
}

// ─── Assignment mode submenu ──────────────────────────────────────────────────

static void assignmentMenu(const ProblemInstance& inst) {
    while (true) {
        printHeader("Run Assignment");
        int cur = (inst.control.generateAssignments==0)?1:inst.control.generateAssignments;
        std::cout << "  Current mode from file: " << cur << "\n\n";
        std::cout << "  1. Mode 1 — primary domains only          (T2.1)\n"
                  << "  2. Mode 2 — sub primary+secondary <-> rev primary\n"
                  << "  3. Mode 3 — all combinations, primary & secondary (T2.4)\n"
                  << "  4. Run with mode from file (mode " << cur << ")\n"
                  << "  0. Back\n"
                  << "\nChoice: ";
        int c; if(!(std::cin>>c)){clearInput();continue;} clearInput();
        if (c==0) return;
        if (c>=1 && c<=3) { runAssignment(inst, c); pause(); }
        else if (c==4)    { runAssignment(inst);     pause(); }
        else std::cout << "  Invalid option.\n";
    }
}

// ─── Risk Analysis ────────────────────────────────────────────────────────────

static void doRiskAnalysis(const ProblemInstance& inst, int K) {
    int mode = (inst.control.generateAssignments==0)?1:inst.control.generateAssignments;

    printHeader("Risk Analysis — K=" + std::to_string(K));
    int R = (int)inst.reviewers.size();
    // Compute C(R,K) to show user
    long long combos = 1;
    for (int i=0;i<K;i++) combos = combos*(R-i)/(i+1);
    std::cout << "  Removing subsets of K=" << K << " reviewer(s) from " << R << " total\n"
              << "  Subsets to test : C(" << R << "," << K << ") = " << combos << "\n"
              << "  Assignment mode : " << mode << "\n";
    printSep();
    std::cout << "  Running...\n\n";

    RiskResult result = (K==1) ? riskAnalysisK1(inst,mode) : riskAnalysisKN(inst,mode,K);

    if (!result.anyRisky) {
        std::cout << "  RESULT: ROBUST — assignment is possible even if\n"
                  << "  any " << K << " reviewer(s) are simultaneously absent.\n";
    } else {
        std::cout << "  RESULT: " << result.riskySets.size() << " RISKY SET(S) FOUND\n"
                  << "  If any of the following group(s) is absent,\n"
                  << "  a valid assignment becomes IMPOSSIBLE:\n\n";
        printSep();
        std::cout << "  #Risk Analysis: " << K << "\n";
        for (const auto& set : result.riskySets) {
            std::cout << "  ";
            for (size_t i=0;i<set.size();++i) { if(i) std::cout<<", "; std::cout<<set[i]; }
            std::cout << "\n";
        }
        printSep();
    }

    std::string riskFile = "risk_" + inst.control.outputFileName;
    try { writeRiskOutput(result, riskFile); }
    catch (const std::exception& e) { std::cerr<<"  [Warning] "<<e.what()<<"\n"; }
}

static void riskMenu(const ProblemInstance& inst) {
    while (true) {
        printHeader("Risk Analysis");
        int fileK = inst.control.riskAnalysis;
        std::cout << "  K from file: " << fileK << "\n\n"
                  << "  1. K=1  — single reviewer absence      (T2.2)\n"
                  << "             O(R x V x E²)\n\n"
                  << "  2. K>1  — group of K reviewers absent  (T2.3)\n"
                  << "             O(C(R,K) x V x E²), enter K manually\n\n"
                  << "  3. Run with K from file (K=" << fileK << ")\n"
                  << "  0. Back\n"
                  << "\nChoice: ";
        int c; if(!(std::cin>>c)){clearInput();continue;} clearInput();
        switch(c) {
            case 1: doRiskAnalysis(inst, 1); pause(); break;
            case 2: {
                std::cout << "  Enter K (>1): ";
                int K; if(!(std::cin>>K)||K<2){clearInput();std::cout<<"  Invalid.\n";break;}
                clearInput();
                doRiskAnalysis(inst, K); pause(); break;
            }
            case 3:
                if (fileK==0) { std::cout<<"  RiskAnalysis=0 in file, nothing to run.\n"; pause(); }
                else { doRiskAnalysis(inst, fileK); pause(); }
                break;
            case 0: return;
            default: std::cout<<"  Invalid option.\n";
        }
    }
}

// ─── Data browser ─────────────────────────────────────────────────────────────

static void dataMenu(const ProblemInstance& inst) {
    while (true) {
        printHeader("Data Browser");
        std::cout << "  1. List submissions\n"
                  << "  2. List reviewers\n"
                  << "  3. Parameters & control settings\n"
                  << "  4. Flow network structure\n"
                  << "  0. Back\n"
                  << "\nChoice: ";
        int c; if(!(std::cin>>c)){clearInput();continue;} clearInput();
        switch(c) {
            case 1: showSubmissions(inst); break;
            case 2: showReviewers(inst);   break;
            case 3: showParameters(inst);  break;
            case 4: showGraphInfo(inst);   break;
            case 0: return;
            default: std::cout<<"  Invalid option.\n";
        }
        pause();
    }
}

// ─── Batch mode ───────────────────────────────────────────────────────────────

static int batchMode(const std::string& inputFile) {
    try {
        ProblemInstance inst = parseInputFile(inputFile);
        std::cout << "Loaded: " << inst.submissions.size() << " submission(s), "
                  << inst.reviewers.size() << " reviewer(s)\n";
        if (inst.control.generateAssignments != 0) runAssignment(inst);
        if (inst.control.riskAnalysis        != 0) doRiskAnalysis(inst, inst.control.riskAnalysis);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[Error] " << e.what() << "\n";
        return 1;
    }
}

// ─── Main menu ────────────────────────────────────────────────────────────────

static void interactiveMenu() {
    std::optional<ProblemInstance> inst;
    std::string loadedFile;

    while (true) {
        printHeader("Scientific Conference Organization Tool");
        printSep();
        if (inst)
            std::cout << "  File   : " << loadedFile << "\n"
                      << "  Papers : " << inst->submissions.size()
                      << "   Reviewers: " << inst->reviewers.size()
                      << "   Min/Max: " << inst->params.minReviewsPerSubmission
                      << "/" << inst->params.maxReviewsPerReviewer << "\n";
        else
            std::cout << "  No file loaded.\n";
        printSep();
        std::cout << "  1. Load input file (.csv)\n"
                  << "  2. Browse data\n"
                  << "  3. Run assignment\n"
                  << "       Mode 1: primary only                    (T2.1)\n"
                  << "       Mode 3: primary + secondary domains      (T2.4)\n"
                  << "  4. Run risk analysis\n"
                  << "       K=1: single reviewer failure             (T2.2)\n"
                  << "       K>1: group of K reviewers failure        (T2.3)\n"
                  << "  0. Exit\n";
        printSep();
        std::cout << "Choice: ";

        int c; if(!(std::cin>>c)){clearInput();continue;} clearInput();
        if (c>=2 && c<=4 && !inst) {
            std::cout<<"\n  [No file loaded — use option 1 first]\n"; pause(); continue;
        }
        switch(c) {
            case 1: {
                std::cout << "\nEnter path to .csv file: ";
                std::string path; std::getline(std::cin, path);
                while (!path.empty() && (path.front()=='"'||path.front()==' ')) path.erase(path.begin());
                while (!path.empty() && (path.back() =='"'||path.back() ==' ')) path.pop_back();
                try {
                    inst = parseInputFile(path); loadedFile = path;
                    std::cout << "\n  Loaded " << inst->submissions.size()
                              << " submission(s) and " << inst->reviewers.size() << " reviewer(s).\n";
                } catch (const std::exception& e) {
                    std::cerr<<"\n  [Error] "<<e.what()<<"\n"; inst.reset();
                }
                pause(); break;
            }
            case 2: dataMenu(*inst);       break;
            case 3: assignmentMenu(*inst); break;
            case 4: riskMenu(*inst);       break;
            case 0: std::cout<<"\nGoodbye!\n"; return;
            default: std::cout<<"  Invalid option.\n"; pause();
        }
    }
}

// ─── Entry point ──────────────────────────────────────────────────────────────

int main(int argc, char* argv[]) {
    if (argc >= 3 && std::string(argv[1]) == "-b") return batchMode(argv[2]);
    interactiveMenu();
    return 0;
}
