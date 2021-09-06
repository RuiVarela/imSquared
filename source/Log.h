#pragma once

#ifdef SOME_RULE

#else
    #include <iostream>

    #define logDbg(TAG, MSG_TO_FORMAT) { TraceLog(LOG_DEBUG, tfm::format("%s: %s", TAG, MSG_TO_FORMAT).c_str()); }
    #define logInf(TAG, MSG_TO_FORMAT) { TraceLog(LOG_INFO, tfm::format("%s: %s", TAG, MSG_TO_FORMAT).c_str()); }
    #define logWrn(TAG, MSG_TO_FORMAT) { TraceLog(LOG_WARNING, tfm::format("%s: %s", TAG, MSG_TO_FORMAT).c_str()); }
    #define logErr(TAG, MSG_TO_FORMAT) { TraceLog(LOG_ERROR, tfm::format("%s: %s", TAG, MSG_TO_FORMAT).c_str()); }
    #define logFat(TAG, MSG_TO_FORMAT) { TraceLog(LOG_FATAL, tfm::format("%s: %s", TAG, MSG_TO_FORMAT).c_str()); }
#endif
