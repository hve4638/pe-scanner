#pragma once
#include "Common/ILogger.h"
#include "CodeLogInfo.h"
#include "ExceptionLogInfo.h"

namespace PEScan {
    class Logger : public ILogger {
        LogLevel m_logLevel = LogLevel::ALL;
        LogDirection m_logDirection = LogDirection::CONSOLE;
        LogLevel m_currentLogLevel = LogLevel::ALL;

        BOOL enabled();
        void write(const tstring&);
        void writeln(const tstring&);
    public:
        // �����ڿ��� LogLevel�� LogDirection �ʱ�ȭ
        Logger();
        Logger(LogLevel);
        Logger(LogDirection);                                   
        Logger(LogLevel, LogDirection);
        Logger(LogDirection, LogLevel);


        void setLogLevel(LogLevel) override;                // LogLevel ����
        void setLogDirection(LogDirection) override;        // LogDirection ���� (�ܼ� ��� or DebugView)
        ILogger& operator<<(LogLevel) override;             // ��� LogLevel ����
                                                            // ������ LogLevel���� ��� ������ ���ƾ� �α� ���

        ILogger& operator<<(LogDirection) override;         // LogDirection ���� (�ܼ� ��� or DebugView)

        ILogger& operator<<(const tstring&) override;       // �޼��� ���
        ILogger& operator<<(const TCHAR*) override;         // �޼��� ���

        // ILogInfo�� ������ �޼��� ���
        // CodeLogInfo�� ExceptionLogInfo�� ILogInfo�� ����
        ILogger& operator<<(ILogInfo*) override;
        ILogger& operator<<(shared_ptr<ILogInfo>) override; 
        ILogger& operator<<(CodeLogInfo);                   
        ILogger& operator<<(ExceptionLogInfo);

        // ȣȯ��
        void log(const TCHAR* logMessage, DWORD errorCode, LogLevel logLevel);
        void log(const TCHAR* logMessage, LogLevel logLevel);
    };
}


