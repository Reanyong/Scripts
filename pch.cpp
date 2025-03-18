// pch.cpp: 미리 컴파일된 헤더에 해당하는 소스 파일

#include "pch.h"
Basic g_Basic;
CTagCollector* g_pTagCol = NULL;
char g_szLocalModNode[64] = { 0 };

#ifdef _DEBUG
    void DebugLog(const char* format, ...)
    {
        // 파일에 로그 기록
        FILE* logFile = fopen("c:\\temp\\EVScripts\\debug_log.txt", "a");
        if (logFile)
        {
            // 현재 시간 추가
            SYSTEMTIME st;
            GetLocalTime(&st);
            fprintf(logFile, "[%04d-%02d-%02d %02d:%02d:%02d.%03d] ",
                st.wYear, st.wMonth, st.wDay,
                st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

            // 가변 인자 처리
            va_list args;
            va_start(args, format);
            vfprintf(logFile, format, args);
            va_end(args);
            fprintf(logFile, "\n");
            fclose(logFile);
        }

        // Visual Studio 출력창에도 로그 표시
        char buffer[2048];

        // 현재 시간 추가
        SYSTEMTIME st;
        GetLocalTime(&st);
        int prefixLen = sprintf_s(buffer, "[%04d-%02d-%02d %02d:%02d:%02d.%03d] ",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

        // 가변 인자 처리
        va_list args;
        va_start(args, format);
        vsprintf_s(buffer + prefixLen, sizeof(buffer) - prefixLen, format, args);
        va_end(args);

        OutputDebugStringA(buffer);
        OutputDebugStringA("\n");
    }
#else
    inline void DebugLog(const char* format, ...) {}
#endif