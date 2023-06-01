#pragma once

#include "IPCUtil.h"

using namespace std;

namespace PEScan {

    IPCUtil::IPCUtil()
    {
    };

    IPCUtil::~IPCUtil(void)
    {
        stop();
    };

    BOOL IPCUtil::isAlive(void)
    {
        return m_isWatching;
    };

    BOOL IPCUtil::create(const tstring ipcName, DWORD bufferSize)
    {
        BOOL result = FALSE;

        // ���� �޸� ����
        m_sharedMemSize = bufferSize;
        m_sharedMemName = _T("Global\\SharedMemory_") + ipcName;
        m_sharedMemHandle = CreateFileMapping(
            INVALID_HANDLE_VALUE,                   // ���� �ڵ�
            NULL,                                   // ���� �Ӽ�
            PAGE_READWRITE,                         // �޸� ��ȣ �ɼ�
            0,                                      // �޸� ��ü ũ�� (0���� �����ϸ� �ý��� ������ ũ��� �ڵ� ����)
            m_sharedMemSize,                        // ���� �޸� ũ��
            ipcName.c_str()                         // ���� �޸� �̸�
        );
        if (m_sharedMemHandle != NULL)
        {
            // ���ε� ���� ����
            m_sharedMemBuffer = MapViewOfFile(
                m_sharedMemHandle,                  // ���� �޸� �ڵ�
                FILE_MAP_ALL_ACCESS,                // �޸� ���� ����
                0,                                  // ���� �޸� ��ġ�� ������
                0,                                  // ���� �޸� ��ġ�� ������
                m_sharedMemSize                     // ������ ������ ũ��
            );
            if (m_sharedMemBuffer != NULL)
            {
                m_mutexName = _T("Global\\Mutex_") + ipcName;
                m_mutexHandle = CreateMutex(
                    NULL,                           // default security attributes
                    FALSE,                          // initially not owned
                    m_mutexName.c_str()             // mutex name
                );
                if (m_mutexHandle != NULL)
                {
                    m_command = reinterpret_cast<PDWORD>(m_sharedMemBuffer);
                    result = TRUE;
                }
                else
                {
                    m_logger.log(_T("CreateMutex error"), GetLastError(), LOG_LEVEL_ERROR);
                }
            }
            else
            {
                m_logger.log(_T("Failed to map view of shared memory"), GetLastError(), LOG_LEVEL_ERROR);
            }
        }
        else
        {
            m_logger.log(_T("Failed to create shared memory"), GetLastError(), LOG_LEVEL_ERROR);
        }
        m_isWatching = result;

        // mutex ���� ���������� ���� ���� �ʾ��� ��� ����
        if (!result)
        {
            stop();
        }
        return result;
    };

    BOOL IPCUtil::open(const tstring ipcName, DWORD bufferSize)
    {
        BOOL result = FALSE;

        if (m_sharedMemHandle != NULL)
        {
            m_logger.log(_T("Already exist shared memory handle."), GetLastError(), LOG_LEVEL_ERROR);
        }
        else
        {
            // ���� �޸� ����
            m_sharedMemSize = bufferSize;
            m_sharedMemName = _T("Global\\SharedMemory_") + ipcName;
            m_sharedMemHandle = OpenFileMapping(
                FILE_MAP_ALL_ACCESS,                    // Desired access
                FALSE,                                  // Inherit handle
                ipcName.c_str()                         // ���� �޸� �̸�
            );
            if (m_sharedMemHandle != NULL)
            {
                // ���ε� ���� ����
                m_sharedMemBuffer = MapViewOfFile(
                    m_sharedMemHandle,                  // ���� �޸� �ڵ�
                    FILE_MAP_ALL_ACCESS,                // �޸� ���� ����
                    0,                                  // ���� �޸� ��ġ�� ������
                    0,                                  // ���� �޸� ��ġ�� ������
                    m_sharedMemSize                     // ������ ������ ũ��
                );
                if (m_sharedMemBuffer != NULL)
                {
                    m_mutexName = _T("Global\\Mutex_") + ipcName;
                    m_mutexHandle = OpenMutex(
                        MUTEX_ALL_ACCESS,               // Desired access
                        FALSE,                          // Inherit Handle
                        m_mutexName.c_str()             // mutex name
                    );
                    if (m_mutexHandle != NULL)
                    {
                        m_command = reinterpret_cast<PDWORD>(m_sharedMemBuffer);
                        result = TRUE;
                    }
                    else
                    {
                        m_logger.log(_T("OpenMutex error"), GetLastError(), LOG_LEVEL_ERROR);
                    }
                }
                else
                {
                    m_logger.log(_T("Failed to map view of shared memory"), GetLastError(), LOG_LEVEL_ERROR);
                }
            }
            else
            {
                m_logger.log(_T("Failed to open shared memory"), GetLastError(), LOG_LEVEL_ERROR);
            }
            m_isWatching = result;

            // mutex ���� ���������� ���� ���� �ʾ��� ��� ����
            if (!result)
            {
                stop();
            }
        }
        return result;
    };

    BOOL IPCUtil::sendMsg(IPC_MESSAGE ipcData)
    {
        BOOL result = FALSE;
        DWORD waitResult = 0;

        if ((m_sharedMemHandle != NULL) && (m_mutexHandle != NULL))
        {
            waitResult = WaitForSingleObject(m_mutexHandle, INFINITE);
            if (waitResult == WAIT_OBJECT_0)
            {
                // �޽��� ����
                PBYTE curBuffer = static_cast<PBYTE>(m_sharedMemBuffer);
                PBYTE dataBuffer = static_cast<PBYTE>(ipcData.data.data());

                try
                {
                    // Shared memory clear
                    memset(m_sharedMemBuffer, 0x0, m_sharedMemSize);

                    // �޽��� ����
                    memcpy_s(curBuffer, sizeof(WORD), &ipcData.command, sizeof(WORD));
                    curBuffer += sizeof(WORD);
                    memcpy_s(curBuffer, sizeof(DWORD), &ipcData.scanType, sizeof(DWORD));
                    curBuffer += sizeof(DWORD);
                    memcpy_s(curBuffer, sizeof(DWORD), &ipcData.size, sizeof(DWORD));
                    if (ipcData.size > 0)
                    {
                        curBuffer += sizeof(DWORD);
                        memcpy_s(curBuffer, ipcData.size, dataBuffer, ipcData.size);
                    }
                }
                catch (...) {};

                // Release ownership of the mutex object
                if (!ReleaseMutex(m_mutexHandle))
                {
                    // Handle error.
                    m_logger.log(_T("Fail - ReleaseMutex"), GetLastError(), LOG_LEVEL_ERROR);
                }
            }
            else
            {
                m_logger.log(_T("Fail - WaitForSingleObject mutex"), GetLastError(), LOG_LEVEL_ERROR);
            }
        }
        return result;
    };

    BOOL IPCUtil::receiveMsg(IPC_MESSAGE& ipcData, IPCCommandType acceptCommandType)
    {
        BOOL result = FALSE;
        DWORD waitResult = 0;

        if ((m_sharedMemHandle != NULL) && (m_mutexHandle != NULL))
        {
            waitResult = WaitForSingleObject(m_mutexHandle, INFINITE);
            if (waitResult == WAIT_OBJECT_0)
            {
                try
                {
                    // �޽��� Ÿ�� ����
                    PBYTE curBuffer = static_cast<PBYTE>(m_sharedMemBuffer);
                    memcpy_s(&ipcData.command, sizeof(WORD), curBuffer, sizeof(WORD));

                    // �޽����� �ְ�, �о�� �� �޽����� ��쿡�� ó��
                    if ((ipcData.command != 0x0) && ((ipcData.command & acceptCommandType) > 0x0))
                    {
                        curBuffer += sizeof(WORD);
                        memcpy_s(&ipcData.scanType, sizeof(DWORD), curBuffer, sizeof(DWORD));
                        curBuffer += sizeof(DWORD);
                        memcpy_s(&ipcData.size, sizeof(DWORD), curBuffer, sizeof(DWORD));
                        if (ipcData.size > 0)
                        {
                            curBuffer += sizeof(DWORD);
                            ipcData.data.assign(ipcData.size, 0);
                            memcpy_s(ipcData.data.data(), ipcData.size, curBuffer, ipcData.size);
                        }
                        // Shared memory clear
                        memset(m_sharedMemBuffer, 0x0, m_sharedMemSize);

                        result = TRUE;
                    }
                }
                catch (...)
                {
                }
                // Release ownership of the mutex object
                if (!ReleaseMutex(m_mutexHandle))
                {
                    // Handle error.
                    m_logger.log(_T("Fail - ReleaseMutex"), GetLastError(), LOG_LEVEL_ERROR);
                }
            }
            else
            {
                m_logger.log(_T("Fail - WaitForSingleObject mutex"), GetLastError(), LOG_LEVEL_ERROR);
            }
        }
        return result;
    };

    BOOL IPCUtil::start(const tstring ipcName, BOOL ipcCreate, DWORD bufferSize)
    {
        BOOL result = FALSE;

        if (ipcCreate)
        {
            // IPC ����
            result = create(ipcName, bufferSize);
        }
        else
        {
            // IPC ����
            result = open(ipcName, bufferSize);
        }
        m_isWatching = result;

        return result;
    };

    void IPCUtil::stop(void)
    {
        if (m_isWatching)
        {
            m_isWatching = FALSE;
            Sleep(100);
        }
        if (m_sharedMemBuffer != NULL)
        {
            UnmapViewOfFile(m_sharedMemBuffer);
            m_sharedMemBuffer = NULL;
        }
        if (m_sharedMemHandle != NULL)
        {
            CloseHandle(m_sharedMemHandle);
            m_sharedMemHandle = NULL;
        }
        if (m_mutexHandle != NULL)
        {
            CloseHandle(m_mutexHandle);
            m_mutexHandle = NULL;
        }
        m_mutexName.clear();
        m_sharedMemName.clear();
    };

    BOOL IPCUtil::isAvailable(void)
    {
        BOOL result = FALSE;

        if (m_command != NULL)
        {
            if ((*m_command) == 0x0)
            {
                result = TRUE;
            }
        }
        return result;
    }

    void IPCUtil::clearSharedMemory(void)
    {
        DWORD waitResult = 0;

        if ((m_sharedMemHandle != NULL) && (m_mutexHandle != NULL))
        {
            waitResult = WaitForSingleObject(m_mutexHandle, INFINITE);
            if (waitResult == WAIT_OBJECT_0)
            {
                try
                {
                    // Shared memory clear
                    PBYTE curBuffer = static_cast<PBYTE>(m_sharedMemBuffer);
                    memset(curBuffer, 0x0, m_sharedMemSize);
                }
                catch (...)
                {
                }
                // Release ownership of the mutex object
                if (!ReleaseMutex(m_mutexHandle))
                {
                    // Handle error.
                    m_logger.log(_T("Fail - ReleaseMutex"), GetLastError(), LOG_LEVEL_ERROR);
                }
            }
            else
            {
                m_logger.log(_T("Fail - WaitForSingleObject mutex"), GetLastError(), LOG_LEVEL_ERROR);
            }
        }
    };

    BOOL IPCUtil::getNextInfo(IPC_MESSAGE& ipcData, IPCCommandType acceptCommandType)
    {
        BOOL result = FALSE;

        if (m_isWatching && (m_command != NULL) && ((*m_command) != 0x0))
        {
            if (receiveMsg(ipcData, acceptCommandType))
            {
                result = TRUE;
            }
        }
        return result;
    };

};
