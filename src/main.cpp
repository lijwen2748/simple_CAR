#include "BMC.h"
#include "BackwardChecker.h"
#include "ForwardChecker.h"
#include "Log.h"
#include "Model.h"
#include "Settings.h"
#include <cstdio>
#include <memory>
#include <string.h>

using namespace car;

void PrintUsage() {
    cout << "Usage: ./simplecar AIG_FILE.aig" << endl;
    cout << "Configs:" << endl;
    cout << "   -f | -b             forward | backward CAR" << endl;
    cout << "   -bmc                BMC" << endl;
    cout << "   -slv n                Solver (1: minisat 2: CaDiCaL 3: Kissat (BMC ONLY))" << endl;
    cout << "   -br n               branching (1: sum 2: VSIDS 3: ACIDS 0: static)" << endl;
    cout << "   -rs                 refer-skipping" << endl;
    cout << "   -is                 internal signals" << endl;
    cout << "   -seed n             seed (works when > 0) for random var ordering" << endl;
    cout << "   -w OUTPUT_PATH/     output witness" << endl;
    cout << "   -v n                verbosity" << endl;
    cout << "   -h                  print help information" << endl;
    exit(0);
}


Settings GetArgv(int argc, char **argv) {
    bool hasInputFile = false;
    Settings settings;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            settings.forward = true;
        } else if (strcmp(argv[i], "-b") == 0) {
            settings.backward = true;
        } else if (strcmp(argv[i], "-bmc") == 0) {
            settings.bmc = true;
        } else if (strcmp(argv[i], "-k") == 0) {
            settings.bmc_k = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "-slv") == 0) {
            settings.solver = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "-pk") == 0) {
            settings.bmc_per_k = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "-br") == 0) {
            settings.Branching = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "-rs") == 0) {
            settings.skip_refer = true;
        } else if (strcmp(argv[i], "-is") == 0) {
            settings.internalSignals = true;
        } else if (strcmp(argv[i], "-seed") == 0) {
            settings.seed = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "-w") == 0) {
            settings.witness = true;
            settings.outputDir = string(argv[i + 1]);
            if (settings.outputDir.back() != '/') settings.outputDir += "/";
            i++;
        } else if (strcmp(argv[i], "-v") == 0) {
            settings.verbosity = atoi(argv[i + 1]);
            i++;
        } else if (!hasInputFile) {
            settings.aigFilePath = string(argv[i]);
            hasInputFile = true;
        } else {
            PrintUsage();
        }
    }
    return settings;
}


int main(int argc, char **argv) {
    Settings settings = GetArgv(argc, argv);

    shared_ptr<Log> log(new Log(settings.verbosity));
    shared_ptr<Model> aigerModel(new Model(settings));
    log->StatInit();
    if (settings.solver == 3 && !settings.bmc) // check kissat usage for BMC only!
    {
        log->L(0, "Kissat is used only for BMC!");
        exit(0);
    }
    shared_ptr<BaseChecker> checker;
    if (settings.forward) {
        checker = make_shared<ForwardChecker>(settings, aigerModel, log);
    } else if (settings.backward) {
        checker = make_shared<BackwardChecker>(settings, aigerModel, log);
    } else if (settings.bmc) {
        checker = make_shared<BMC>(settings, aigerModel, log);
    }
    checker->Run();
    return 0;
}