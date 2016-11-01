#pragma once

#if _MSC_VER // this is defined when compiling with Visual Studio
#define EXPORT_API __declspec(dllexport) // Visual Studio needs annotating exported functions with this
#else
#define EXPORT_API // XCode does not need annotating exported functions, so define is empty
#endif

#include "julius/juliuslib.h"
#include <locale.h>

extern "C"
{
	typedef int(*audio_read_callback_func_type)(short*, int);
	typedef void(*debug_log_func_type)(const char*);
	typedef void(*result_func_type)(const wchar_t*);

	EXPORT_API void set_audio_callback(audio_read_callback_func_type callback);
	EXPORT_API void set_debug_log_func(debug_log_func_type debug_log_func);
	EXPORT_API void set_result_func(result_func_type result_func);

	EXPORT_API void initialize(char* jconf_path);
	EXPORT_API void stop();
}

void clean_up_if_exists();
bool create_engine();
static int callback_adin_fetch_input(SP16 *sampleBuffer, int reqlen);
static void put_hypo_phoneme(WORD_ID * seq, int n, WORD_INFO * winfo);
static void on_result(Recog * recog, void * dummy);