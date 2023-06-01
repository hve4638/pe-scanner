#pragma once
#include "Common/typedef.h"
#include "Common/IPEPrinter.h"

namespace PEScan {
    class PEPrinter : public IPEPrinter {
        shared_ptr<PEStructure> m_PEStructure;

    private:
        void printBaseAddress();
        void printDosHeader();
        void printNTHeader();
        void printSectionHeader();
        void printEAT();
        void printIAT();
        void printTLS();

    public:
        PEPrinter();
        PEPrinter(shared_ptr<PEStructure> peStructWeakPtr);
        ~PEPrinter() override;
        void reset(shared_ptr<PEStructure> peStructWeakPtr) override;

        void printPEStructure() override;
    };
};

