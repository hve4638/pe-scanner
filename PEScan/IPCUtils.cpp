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

        // 공유 메모리 생성
        m_sharedMemSize = bufferSize;
        m_sharedMemName = _T("Global\\SharedMemory_") + ipcName;
        m_sharedMemHandle = CreateFileMapping(
            INVALID_HANDLE_VALUE,                   // 파일 핸들
            NULL,                                   // 보안 속성
            PAGE_READWRITE,                         // 메모리 보호 옵션
            0,                                      // 메모리 객체 크기 (0으로 설정하면 시스템 페이지 크기로 자동 조정)
            m_sharedMemSize,                        // 공유 메모리 크기
            ipcName.c_str()                         // 공유 메모리 이름
        );
        if (m_sharedMemHandle != NULL)
        {
            // 매핑된 영역 설정
            m_sharedMemBuffer = MapViewOfFile(
                m_sharedMemHandle,                  // 공유 메모리 핸들
                FILE_MAP_ALL_ACCESS,                // 메모리 접근 권한
                0,                                  // 공유 메모리 위치의 오프셋
                0,                                  // 공유 메모리 위치의 오프셋
                m_sharedMemSize                     // 매핑할 영역의 크기
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

        // mutex 까지 정상적으로 성공 하지 않았을 경우 해제
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
            // 공유 메모리 오픈
            m_sharedMemSize = bufferSize;
            m_sharedMemName = _T("Global\\SharedMemory_") + ipcName;
            m_sharedMemHandle = OpenFileMapping(
                FILE_MAP_ALL_ACCESS,                    // Desired access
                FALSE,                                  // Inherit handle
                ipcName.c_str()                         // 공유 메모리 이름
            );
            if (m_sharedMemHandle != NULL)
            {
                // 매핑된 영역 설정
                m_sharedMemBuffer = MapViewOfFile(
                    m_sharedMemHandle,                  // 공유 메모리 핸들
                    FILE_MAP_ALL_ACCESS,                // 메모리 접근 권한
                    0,                                  // 공유 메모리 위치의 오프셋
                    0,                                  // 공유 메모리 위치의 오프셋
                    m_sharedMemSize                     // 매핑할 영역의 크기
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

            // mutex 까지 정상적으로 성공 하지 않았을 경우 해제
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
                // 메시지 전송
                PBYTE curBuffer = static_cast<PBYTE>(m_sharedMemBuffer);
                PBYTE dataBuffer = static_cast<PBYTE>(ipcData.data.data());

                try
                {
                    // Shared memory clear
                    memset(m_sharedMemBuffer, 0x0, m_sharedMemSize);

                    // 메시지 전송
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
                    // 메시지 타입 복원
                    PBYTE curBuffer = static_cast<PBYTE>(m_sharedMemBuffer);
                    memcpy_s(&ipcData.command, sizeof(WORD), curBuffer, sizeof(WORD));

                    // 메시지가 있고, 읽어야 할 메시지인 경우에만 처리
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
            // IPC 생성
            result = create(ipcName, bufferSize);
        }
        else
        {
            // IPC 오픈
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
