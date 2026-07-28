#pragma once

#include <windows.foundation.h>

#include <wrl/implements.h>

inline BOOL SHProcessMessagesUpdateTimeout(DWORD dwStartTick, DWORD dwTimeoutTotal, DWORD* pdwTimeoutRemaining)
{
    *pdwTimeoutRemaining = 0;
    BOOL bRet = TRUE;

    if (dwTimeoutTotal != -1)
    {
        DWORD dwWaitedTick = GetTickCount() - dwStartTick;
        if (dwWaitedTick <= dwTimeoutTotal)
        {
            *pdwTimeoutRemaining = dwTimeoutTotal - dwWaitedTick;
        }
        else
        {
            bRet = FALSE;
        }
    }
    else
    {
        *pdwTimeoutRemaining = -1;
    }

    return bRet;
}

inline BOOL PeekMessageWithWakeMask(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, BOOL bRemove, DWORD dwWakeMask)
{
    BOOL bRet = FALSE;
    UINT wRemoveMsg = 0;

    if (dwWakeMask == QS_ALLINPUT)
    {
        if (bRemove)
            wRemoveMsg |= PM_REMOVE;
        bRet = PeekMessageW(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
    }
    else
    {
        if ((dwWakeMask & QS_INPUT) != 0)
            wRemoveMsg |= PM_QS_INPUT;

        if ((dwWakeMask & (QS_POSTMESSAGE | QS_ALLPOSTMESSAGE)) != 0)
            wRemoveMsg |= PM_QS_POSTMESSAGE;

        if ((dwWakeMask & QS_PAINT) != 0)
            wRemoveMsg |= PM_QS_PAINT;

        if ((dwWakeMask & QS_SENDMESSAGE) != 0)
            wRemoveMsg |= PM_QS_SENDMESSAGE;

        if (wRemoveMsg)
        {
            if (bRemove)
                wRemoveMsg |= PM_REMOVE;
            bRet = PeekMessageW(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
        }
    }

    return bRet;
}

inline DWORD SHProcessMessagesUntilEventsEx(HWND hwnd, HANDLE* pHandles, DWORD cHandles, DWORD dwTimeout, DWORD dwWakeMask, DWORD dwFlags)
{
    APTTYPE aptType;
    APTTYPEQUALIFIER aptTypeQualifier;
    BOOL fIsASTA = SUCCEEDED(CoGetApartmentType(&aptType, &aptTypeQualifier))
        && aptTypeQualifier == APTTYPEQUALIFIER_APPLICATION_STA
        && aptType == APTTYPE_STA;

    DWORD dwStartTick = GetTickCount();
    DWORD dwTimeoutRemaining = dwTimeout;

    if (!pHandles && dwTimeout == INFINITE)
    {
        return WAIT_FAILED;
    }

    DWORD dwReturn;

    do
    {
        do
        {
            if (fIsASTA)
            {
                HRESULT hrCoWait = CoWaitForMultipleHandles(
                    ((dwFlags & MWMO_INPUTAVAILABLE) != 0 ? COWAIT_INPUTAVAILABLE : 0)
                    | (COWAIT_DISPATCH_CALLS | COWAIT_DISPATCH_WINDOW_MESSAGES),
                    dwTimeoutRemaining, cHandles, pHandles, &dwReturn);
                if (hrCoWait == RPC_S_CALLPENDING)
                {
                    dwReturn = WAIT_TIMEOUT;
                }
                else if (FAILED(hrCoWait))
                {
                    dwReturn = WAIT_FAILED;
                }
            }
            else
            {
                dwReturn = MsgWaitForMultipleObjectsEx(cHandles, pHandles, dwTimeoutRemaining, dwWakeMask, dwFlags);
            }

            if (!SHProcessMessagesUpdateTimeout(dwStartTick, dwTimeout, &dwTimeoutRemaining))
            {
                dwReturn = WAIT_TIMEOUT;
            }

            if (dwReturn != cHandles)
            {
                break;
            }

            MSG msg;
            while (PeekMessageWithWakeMask(&msg, hwnd, 0, 0, TRUE, QS_ALLINPUT))
            {
                if (msg.message == WM_QUIT)
                {
                    PostQuitMessage((int)msg.wParam);
                    dwReturn = WAIT_TIMEOUT;
                    break;
                }

                TranslateMessage(&msg);
                if (msg.message == WM_SETCURSOR && LOWORD(msg.lParam) != HTERROR)
                {
                    SetCursor(LoadCursorW(nullptr, MAKEINTRESOURCEW(32514) /*IDC_WAIT*/));
                }
                else
                {
                    DispatchMessageW(&msg);
                }

                if (cHandles)
                {
                    DWORD dwReturnTemp = WaitForMultipleObjectsEx(cHandles, pHandles, 0, 0, FALSE);
                    if (dwReturnTemp != WAIT_TIMEOUT)
                    {
                        dwReturn = dwReturnTemp;
                        break;
                    }
                }

                if (!SHProcessMessagesUpdateTimeout(dwStartTick, dwTimeout, &dwTimeoutRemaining))
                {
                    dwReturn = WAIT_TIMEOUT;
                    break;
                }
            }
        }
        while (dwReturn == cHandles);
    }
    while (dwReturn == WAIT_IO_COMPLETION);

    if (dwReturn == WAIT_TIMEOUT && cHandles)
    {
        DWORD dwReturnTemp = WaitForMultipleObjectsEx(cHandles, pHandles, 0, 0, FALSE);
        if (dwReturnTemp != WAIT_TIMEOUT)
        {
            dwReturn = dwReturnTemp;
        }
    }

    return dwReturn;
}

template <typename THandler, typename TOperation>
HRESULT WaitForCompletion(TOperation* pOperation, COWAIT_FLAGS flags = (COWAIT_FLAGS)-1, HANDLE hEventCancelled = nullptr)
{
    class FTMEventDelegate : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>
        , THandler
        , Microsoft::WRL::FtmBase
    >
    {
    public:
        FTMEventDelegate()
            : _status(AsyncStatus::Started)
            , _hEventCompleted(nullptr)
        {
        }

        // ReSharper disable once CppHidingFunction
        HRESULT RuntimeClassInitialize()
        {
            _hEventCompleted = CreateEventExW(nullptr, nullptr, FALSE, EVENT_ALL_ACCESS);
            return _hEventCompleted ? S_OK : HRESULT_FROM_WIN32(GetLastError());
        }

        STDMETHODIMP Invoke(TOperation*, AsyncStatus status) override
        {
            _status = status;
            SetEvent(_hEventCompleted);
            return S_OK;
        }

        HANDLE GetEvent() const { return _hEventCompleted; }
        AsyncStatus GetStatus() const { return _status; }

    private:
        ~FTMEventDelegate() override
        {
            CloseHandle(_hEventCompleted);
        }

        AsyncStatus _status;
        HANDLE _hEventCompleted;
    };

    Microsoft::WRL::ComPtr<TOperation> spOperation;
    Microsoft::WRL::ComPtr<FTMEventDelegate> spCallback;
    HRESULT hr = Microsoft::WRL::MakeAndInitialize<FTMEventDelegate>(&spCallback);
    if (SUCCEEDED(hr))
    {
        hr = pOperation->put_Completed(spCallback.Get());
        if (SUCCEEDED(hr))
        {
            HANDLE rgHandles[] = { spCallback->GetEvent(), nullptr };
            bool fCancelOp = false;
            if (flags == (COWAIT_FLAGS)-1 && SHProcessMessagesUntilEventsEx(nullptr, rgHandles, 1, INFINITE, QS_ALLINPUT, 0) == WAIT_FAILED)
            {
                hr = E_FAIL;
            }
            else if (SUCCEEDED(hr) && flags != (COWAIT_FLAGS)-1)
            {
                DWORD dwHandleIndex;
                hr = CoWaitForMultipleHandles(flags, INFINITE, 1, rgHandles, &dwHandleIndex);
                if (SUCCEEDED(hr) && dwHandleIndex)
                {
                    hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
                    fCancelOp = true;
                }
            }

            Microsoft::WRL::ComPtr<IAsyncInfo> spAsyncInfo;
            if (fCancelOp && SUCCEEDED(pOperation->QueryInterface(IID_PPV_ARGS(&spAsyncInfo))))
            {
                spAsyncInfo->Cancel();
            }

            if (SUCCEEDED(hr) && spCallback->GetStatus() != AsyncStatus::Completed)
            {
                if (spAsyncInfo.Get() || SUCCEEDED(pOperation->QueryInterface(IID_PPV_ARGS(&spAsyncInfo))))
                {
                    spAsyncInfo->get_ErrorCode(&hr);
                }
            }
        }
    }

    return hr;
}

inline HRESULT WaitForCompletion(
    ABI::Windows::Foundation::IAsyncAction* pAction, COWAIT_FLAGS flags = (COWAIT_FLAGS)-1,
    HANDLE hEventCancelled = nullptr)
{
    return WaitForCompletion<ABI::Windows::Foundation::IAsyncActionCompletedHandler>(pAction, flags, hEventCancelled);
}

template <typename TOperationResult>
HRESULT WaitForCompletion(
    ABI::Windows::Foundation::IAsyncOperation<TOperationResult>* pOperation, COWAIT_FLAGS flags = (COWAIT_FLAGS)-1,
    HANDLE hEventCancelled = nullptr)
{
    return WaitForCompletion<ABI::Windows::Foundation::IAsyncOperationCompletedHandler<TOperationResult>>(
        pOperation, flags, hEventCancelled);
}

// Custom name and prototype
template <typename TOperationResult>
HRESULT WaitForCompletionAndGetResults(
    ABI::Windows::Foundation::IAsyncOperation<TOperationResult>* pOperation,
    typename ABI::Windows::Foundation::Internal::GetAbiType<typename ABI::Windows::Foundation::IAsyncOperation<TOperationResult>::TResult_complex>::type* pResult,
    COWAIT_FLAGS flags = (COWAIT_FLAGS)-1, HANDLE hEventCancelled = nullptr)
{
    HRESULT hr = WaitForCompletion(pOperation, flags, hEventCancelled);
    if (SUCCEEDED(hr))
    {
        hr = pOperation->GetResults(pResult);
    }
    return hr;
}
