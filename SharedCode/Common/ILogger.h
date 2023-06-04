#pragma once
#include "Common/typedef.h"
#include "Common/LogLevel.h"
#include "Common/LogDirection.h"
#include "Common/ILogInfo.h"

namespace PEScan {
    struct ILogger {
        virtual ~ILogger() {}
        virtual void setLogLevel(LogLevel) abstract;                // LogLevel ����
        virtual void setLogDirection(LogDirection) abstract;        // LogDirection ���� (�ܼ� ��� or DebugView)
        virtual ILogger& operator<<(LogLevel) abstract;             // ��� LogLevel ����
                                                                    // ������ LogLevel���� ��� ������ ���ƾ� �αװ� ��µ�

        // �޼��� ���
        virtual ILogger& operator<<(const tstring&) abstract;
        virtual ILogger& operator<<(const TCHAR*) abstract;

        // ILogInfo�� ������ �޼��� ���
        virtual ILogger& operator<<(ILogInfo*) abstract;
        virtual ILogger& operator<<(shared_ptr<ILogInfo>) abstract;

        virtual ILogger& operator<<(LogDirection) abstract;
    };
}

