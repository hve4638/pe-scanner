#pragma once

#include <thread>
#include <chrono>
#include <comdef.h>
#include <Wbemidl.h>
#include "ProcWatch.h"

#pragma comment(lib, "wbemuuid.lib")

using namespace std;

namespace PEScan {

    class EventSink : public IWbemObjectSink
    {
    private:
        ProcWatch* m_processWatch = NULL;

    public:
        ULONG STDMETHODCALLTYPE AddRef()
        {
            return 1;
        }

        ULONG STDMETHODCALLTYPE Release()
        {
            return 1;
        }

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv)
        {
            if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
            {
                *ppv = (IWbemObjectSink*)this;
                return S_OK;
            }
            else
            {
                return E_NOINTERFACE;
            }
        }

        void setCallback(ProcWatch* callbackFunc)
        {
            m_processWatch = callbackFunc;
        }

        HRESULT STDMETHODCALLTYPE Indicate(LONG lObjectCount, IWbemClassObject** apObjArray)
        {
            VARIANT var;
            VARIANT varObject;
            HRESULT hr = NULL;
            DWORD pid = 0;
            tstring processName;
            tstring processPath;
            tstring processCmdLine;

            VariantInit(&var);
            VariantInit(&varObject);

            for (int i = 0; i < lObjectCount; ++i)
            {
                hr = apObjArray[i]->Get(L"TargetInstance", 0, &var, nullptr, nullptr);
                if (SUCCEEDED(hr))
                {
                    IWbemClassObject* pProcess = nullptr;
                    hr = var.pdispVal->QueryInterface(IID_IWbemClassObject, reinterpret_cast<void**>(&pProcess));
                    if (SUCCEEDED(hr))
                    {
                        if (SUCCEEDED(pProcess->Get(L"ProcessID", 0, &varObject, nullptr, nullptr)))
                        {
                            pid = varObject.uintVal;
                        }
                        VariantClear(&varObject);
                        if (SUCCEEDED(pProcess->Get(L"Name", 0, &varObject, nullptr, nullptr)) && varObject.vt == VT_BSTR)
                        {
                            processName = varObject.bstrVal;
                        }
                        VariantClear(&varObject);
                        if (SUCCEEDED(pProcess->Get(L"ExecutablePath", 0, &varObject, nullptr, nullptr)) && varObject.vt == VT_BSTR)
                        {
                            processPath = varObject.bstrVal;
                        }
                        VariantClear(&varObject);
                        if (SUCCEEDED(pProcess->Get(L"CommandLine", 0, &varObject, nullptr, nullptr)) && varObject.vt == VT_BSTR)
                        {
                            processCmdLine = varObject.bstrVal;
                        }
                        VariantClear(&varObject);
                        pProcess->Release();

                        if (m_processWatch != NULL)
                        {
                            m_processWatch->processCallback(pid, processName.c_str(), processPath.c_str(), processCmdLine.c_str());
                        }
                    }
                    Sleep(0);
                }
            }
            VariantClear(&var);
            VariantClear(&var);
            return WBEM_S_NO_ERROR;
        }

        HRESULT STDMETHODCALLTYPE SetStatus(LONG lFlags, HRESULT hResult, BSTR strParam, IWbemClassObject* pObjParam)
        {
            return WBEM_S_NO_ERROR;
        }
    };


    ProcWatch::ProcWatch()
    {
    };

    ProcWatch::~ProcWatch(void)
    {
        stopMonitoring();
    };

    BOOL ProcWatch::isAlive(void)
    {
        return isWatching;
    };

    void ProcWatch::watchProcessStart(ProcWatch* processWatch)
    {
        HRESULT hr = NULL;

        // Initialize COM
        if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
        {
            m_logger.log(_T("Failed to initialize COM."), GetLastError(), LOG_LEVEL_ERROR);
        }
        else
        {
            // Initialize security
            hr = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);
            if (FAILED(hr))
            {
                m_logger.log(_T("Failed to initialize security."), GetLastError(), LOG_LEVEL_ERROR);
            }
            else
            {
                // Obtain the initial locator to WMI
                IWbemLocator* pLocator = nullptr;
                hr = CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<LPVOID*>(&pLocator));
                if (FAILED(hr))
                {
                    m_logger.log(_T("Failed to create IWbemLocator instance."), GetLastError(), LOG_LEVEL_ERROR);
                }
                else
                {
                    IWbemServices* pServices = nullptr;
                    hr = pLocator->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, nullptr, 0, nullptr, nullptr, &pServices);
                    if (FAILED(hr))
                    {
                        m_logger.log(_T("Failed to connect to the WMI service."), GetLastError(), LOG_LEVEL_ERROR);
                        pLocator->Release();
                    }
                    else
                    {
                        // Set the security levels on the proxy
                        hr = CoSetProxyBlanket(pServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
                        if (FAILED(hr))
                        {
                            m_logger.log(_T("Failed to set security levels on the proxy."), GetLastError(), LOG_LEVEL_ERROR);
                            pServices->Release();
                            pLocator->Release();
                        }
                        else
                        {
                            // Set up the event notification query
                            IUnsecuredApartment* pUnsecApp = nullptr;
                            hr = CoCreateInstance(CLSID_UnsecuredApartment, nullptr, CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment, reinterpret_cast<void**>(&pUnsecApp));
                            if (FAILED(hr))
                            {
                                m_logger.log(_T("Failed to create IUnsecuredApartment instance."), GetLastError(), LOG_LEVEL_ERROR);
                                pServices->Release();
                                pLocator->Release();
                            }
                            else
                            {
                                // Create an instance of the EventSink class
                                // EventSink sink;
                                // sink.setCallback(this);
                                EventSink* pSink = new EventSink;
                                pSink->setCallback(processWatch);
                                pSink->AddRef();

                                IUnknown* pStubUnk = nullptr;
                                hr = pUnsecApp->CreateObjectStub(pSink, &pStubUnk);
                                if (FAILED(hr))
                                {
                                    m_logger.log(_T("Failed to create object stub."), GetLastError(), LOG_LEVEL_ERROR);
                                    pServices->Release();
                                    pLocator->Release();
                                    pUnsecApp->Release();
                                    pStubUnk->Release();
                                    pSink->Release();
                                }
                                else
                                {
                                    // Set up the event consumer
                                    IWbemObjectSink* pStubSink = nullptr;
                                    hr = pStubUnk->QueryInterface(IID_IWbemObjectSink, reinterpret_cast<void**>(&pStubSink));
                                    if (FAILED(hr))
                                    {
                                        m_logger.log(_T("Failed to query IWbemObjectSink interface."), GetLastError(), LOG_LEVEL_ERROR);
                                        pServices->Release();
                                        pLocator->Release();
                                        pUnsecApp->Release();
                                        pStubUnk->Release();
                                        pSink->Release();
                                    }
                                    else
                                    {
                                        // Execute the query asynchronously
                                        hr = pServices->ExecNotificationQueryAsync(
                                            _bstr_t(L"WQL"),
                                            _bstr_t(L"SELECT * FROM __InstanceCreationEvent WITHIN 1 WHERE TargetInstance ISA 'Win32_Process'"),
                                            WBEM_FLAG_SEND_STATUS,
                                            nullptr,
                                            pStubSink
                                        );
                                        if (FAILED(hr))
                                        {
                                            m_logger.log(_T("Failed to execute the query asynchronously."), GetLastError(), LOG_LEVEL_ERROR);
                                            pServices->Release();
                                            pLocator->Release();
                                            pUnsecApp->Release();
                                            pStubUnk->Release();
                                            pSink->Release();
                                            pStubSink->Release();
                                        }
                                        else
                                        {
                                            // 프로세스 감시
                                            if (processWatch != NULL)
                                            {
                                                while (processWatch->isWatching)
                                                {
                                                    // 1000 ns == 0.001 ms
                                                    this_thread::sleep_for(std::chrono::nanoseconds(1000));
                                                }

                                                // Cancel the event notification
                                                pServices->CancelAsyncCall(pSink);

                                                // Cleanup
                                                pServices->Release();
                                                pLocator->Release();
                                                pUnsecApp->Release();
                                                pStubUnk->Release();
                                                pSink->Release();
                                                pStubSink->Release();
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            CoUninitialize();
        }
        ExitThread(0);
    };

    void ProcWatch::processCallback(DWORD pid, tstring processName, tstring processPath, tstring processCmdLine)
    {
        m_processInfoList.push(make_tuple(pid, processName, processPath, processCmdLine));
    };

    void ProcWatch::startMonitoring(void)
    {
        if (isWatching)
        {
            stopMonitoring();
        }
        isWatching = TRUE;

        thread processWatchThread = thread([&]() { watchProcessStart(this); });
        processWatchThread.detach();
    };

    void ProcWatch::stopMonitoring(void)
    {
        if (isWatching)
        {
            isWatching = FALSE;
            Sleep(100);
        }
    };

    BOOL ProcWatch::getNextInfo(ProcessInfo& processInfo)
    {
        BOOL result = !m_processInfoList.empty();

        if (result)
        {
            processInfo = m_processInfoList.front();
            m_processInfoList.pop();
        }
        return result;
    };
}

