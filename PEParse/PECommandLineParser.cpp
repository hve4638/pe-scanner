#include <format>
#include <string>
#include "PECommandLineParser.h"
#include "peparser.h"
#include "PEPrinter.h"
    
using namespace PEParse;
using namespace PELog;
//using namespace PEUtils;

namespace CommandLineUtils {
    PECommandLineParser::PECommandLineParser() {
        addCommand(_T("help"), [](vector<tstring> args, ArgsAdditional additional) {
            tcout << _T("scan") << endl;
            tcout << _T("print") << endl;
        });

        addCommand(_T("print"), [](vector<tstring> args, ArgsAdditional additional) {
            auto options = additional.options;
            PEParser parser;
            if (options->has(_T('p'))) {
                int pid = std::stoi(options->get(_T('p')));
                parser.parsePEProcess(pid);
            }
            else if (options->has(_T('f'))) {
                auto filename = options->get(_T('f'));
                parser.parsePEFile(filename.c_str());
            }
            else {
                tcout << _T("Usage: ") << endl;
                tcout << endl;
                tcout << _T("print -f [filename]") << endl;
                tcout << _T("print -p [pid]") << endl;
                tcout << endl;

                return;
            }
            
            PEPrinter printer = parser.getPEStructure();
            printer.printPEStructure();
        }, _T("pf"), _T(""));

        addCommand(_T("scan"), [](vector<tstring> args, ArgsAdditional additional) {
            auto options = additional.options;
            PEParser parser;
            if (options->has(_T('p'))) {
                int pid = std::stoi(options->get(_T('p'))); 
                parser.parsePEProcess(pid);
            }
            else if (options->has(_T('f'))) {
                auto filename = options->get(_T('f'));
                parser.parsePEFile(filename.c_str());
            }
            else {
                tcout << _T("Usage: ") << endl;
                tcout << endl;
                tcout << _T("scan [hash] -f [filename]") << endl;
                tcout << _T("scan [hash] -p [pid]") << endl;
                tcout << endl;
                return;
            }
            tstring expectedHash = args.at(0);

            tstring textSectionHash = _T("");
            tstring codeSectionHash = _T("");
            tstring entryPointHash = _T("");
            tstring pdbHash = _T("");
            parser.tryGetSectionHash(_T(".text"), textSectionHash);
            parser.tryGetEntryPointSectionHash(entryPointHash);
            parser.tryGetCodeSectionHash(codeSectionHash);
            parser.tryGetPDBFilePathHash(pdbHash);

            tcout << _T("<Scan>") << endl;
            tcout << format(_T("Hash : {:s}"), expectedHash) << endl;
            tcout << _T("----------------------------------------") << endl;
            if (expectedHash.compare(textSectionHash) == 0) {
                tcout << format(_T("Detected (.text section)")) << endl;
            }
            if (expectedHash.compare(entryPointHash) == 0) {
                tcout << format(_T("Detected (Entry point section)")) << endl;
            }
            if (expectedHash.compare(codeSectionHash) == 0) {
                tcout << format(_T("Detected (code Section)")) << endl;
            }
            if (expectedHash.compare(pdbHash) == 0) {
                tcout << format(_T("Detected (PDB file path)")) << endl;
            }
            tcout << endl;
        }, _T("pf"), _T(""));

    }
}