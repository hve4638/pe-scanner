#pragma once
#include "Common/typedef.h"
#include <functional>

using namespace std;

namespace PEScan {
    interface IFileSearch {
        virtual ~IFileSearch() {}

        /// <summary>
        /// 실행중인 search() 정지
        /// </summary>
        virtual void stop() abstract;

        /// <summary>
        /// 상대경로 또는 환경변수가 포함된 경로로부터 절대경로를 리턴
        /// </summary>
        /// <param name="orignal"></param>
        /// <param name="real"></param>
        virtual BOOL getRealPath(tstring orignal, tstring& real) abstract;
        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="parentDirectory">기준 디렉토리</param>
        /// <param name="currentFileCount">찾은 파일수</param>
        /// <param name="scanFunction">콜백</param>
        virtual void search(tstring parentDirectory, DWORD& currentFileCount, function<void(tstring)> scanFunction = NULL) abstract;
    };
};
