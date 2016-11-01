#include "lib.h"

static Jconf* jconf = nullptr;
static Recog* recog = nullptr;

static audio_read_callback_func_type audio_read_callback = nullptr;
static debug_log_func_type debug_log = nullptr;
static result_func_type output_result = nullptr;

EXPORT_API void set_audio_callback(audio_read_callback_func_type callback)
{
	audio_read_callback = callback;
}

EXPORT_API void set_debug_log_func(debug_log_func_type debug_log_func)
{
	debug_log = debug_log_func;
}

EXPORT_API void set_result_func(result_func_type result_func)
{
	output_result = result_func;
}

EXPORT_API void initialize(char* jconf_path)
{
	jlog_set_output(fopen("julius-log.txt", "w"));

	clean_up_if_exists();

	jconf = j_config_load_file_new(jconf_path);
	recog = j_recog_new();

	if (create_engine())
	{
		debug_log("エンジンの作成に成功しました");
	}
	else
	{
		debug_log("エンジンの作成に失敗しました");
	}

	j_recognize_stream(recog);
	clean_up_if_exists();
}

EXPORT_API void stop()
{
	j_close_stream(recog);
}

void clean_up_if_exists()
{
	if (recog != nullptr)
	{
		// jconf will be released inside this
		j_recog_free(recog);
		jconf = nullptr;
		recog = nullptr;
	}

	if (jconf != nullptr) {
		j_jconf_free(nullptr);
		jconf = nullptr;
	}
}

static int callback_adin_fetch_input(SP16 *sampleBuffer, int reqlen)
{
	// If shared audio buffer has some new data, or some data remains from the last call,
	// get the samples into sampleBuffer at most reqlen length.
	return 0;
}

static void on_process_online(Recog *recog, void *dummy)
{
	jlog("on_process_online");
	debug_log("on_process_online");
}

static void on_process_offline(Recog *recog, void *dummy)
{
	jlog("on_process_offline");
	debug_log("on_process_offline");
}

static void on_speech_ready(Recog *recog, void *dummy)
{
	jlog("on_speech_ready");
	debug_log("on_speech_ready");
}

static void on_speech_start(Recog *recog, void *dummy)
{
	jlog("on_speech_start");
	debug_log("on_speech_start");
}

static void on_speech_stop(Recog *recog, void *dummy)
{
	jlog("on_speech_stop");
	debug_log("on_speech_stop");
}

static void on_recognition_begin(Recog *recog, void *dummy)
{
	jlog("on_recognition_begin");
	debug_log("on_recognition_begin");
}

static void on_recognition_end(Recog *recog, void *dummy)
{
	jlog("on_recognition_end");
	debug_log("on_recognition_end");
}

static void on_pass1_frame(Recog *recog, void *dummy)
{
	// jlog("on_pass1_frame");
	// debug_log("on_pass1_frame");
}

static void on_pause(Recog *recog, void *dummy)
{
	jlog("on_pause");
	debug_log("on_pause");
}

static void on_resume(Recog *recog, void *dummy)
{
	jlog("on_resume");
	debug_log("on_resume");
}

static void on_pause_function(Recog *recog, void *dummy)
{
	jlog("on_pause_function");
	debug_log("on_pause_function");
}

static void on_result(Recog *recog, void *dummy)
{
	jlog("on_result");
	debug_log("on_result");

	int i, j;
	int len;
	WORD_INFO *winfo;
	WORD_ID *seq;
	int seqnum;
	int n;
	Sentence *s;
	RecogProcess *r;
	HMM_Logical *p;
	SentenceAlign *align;

	char result_string[65535] = "";
	char temp_buffer[65535];

	/* all recognition results are stored at each recognition process
	instance */
	for (r = recog->process_list; r; r = r->next) {

		/* skip the process if the process is not alive */
		if (!r->live) continue;

		/* result are in r->result.  See recog.h for details */

		/* check result status */
		if (r->result.status < 0) {      /* no results obtained */
										 /* outout message according to the status code */
			switch (r->result.status) {
			case J_RESULT_STATUS_REJECT_POWER:
				debug_log("<input rejected by power>\n");
				break;
			case J_RESULT_STATUS_TERMINATE:
				debug_log("<input teminated by request>\n");
				break;
			case J_RESULT_STATUS_ONLY_SILENCE:
				debug_log("<input rejected by decoder (silence input result)>\n");
				break;
			case J_RESULT_STATUS_REJECT_GMM:
				debug_log("<input rejected by GMM>\n");
				break;
			case J_RESULT_STATUS_REJECT_SHORT:
				debug_log("<input rejected by short input>\n");
				break;
			case J_RESULT_STATUS_REJECT_LONG:
				debug_log("<input rejected by long input>\n");
				break;
			case J_RESULT_STATUS_FAIL:
				debug_log("<search failed>\n");
				break;
			}
			/* continue to next process instance */
			continue;
		}

		/* output results for all the obtained sentences */
		winfo = r->lm->winfo;

		for (n = 0; n < r->result.sentnum; n++) { /* for all sentences */

			s = &(r->result.sent[n]);
			seq = s->word;
			seqnum = s->word_num;

			/* output word sequence like Julius */
			jlog("sentence%d:", n + 1);
			for (i = 0; i < seqnum; i++) jlog(" %s", winfo->woutput[seq[i]]);
			strncat(result_string, "sentence,", sizeof(result_string) - 1);
			debug_log(result_string);

			for (i = 0; i < seqnum; i++)
			{
				strncat(result_string, winfo->woutput[seq[i]], sizeof(result_string) - 1);
				if (i + 1 == seqnum)
				{
					strncat(result_string, "\n", sizeof(result_string) - 1);
				}
				else
				{
					strncat(result_string, ",", sizeof(result_string) - 1);
				}
			}

			debug_log(result_string);

			jlog("\n");
			/* LM entry sequence */
			jlog("wseq%d:", n + 1);
			for (i = 0; i < seqnum; i++) jlog(" %s", winfo->wname[seq[i]]);

			strncat(result_string, "words,", sizeof(result_string) - 1);
			debug_log(result_string);
			for (i = 0; i < seqnum; i++)
			{
				strncat(result_string, winfo->wname[seq[i]], sizeof(result_string) - 1);
				if (i + 1 == seqnum)
				{
					strncat(result_string, "\n", sizeof(result_string) - 1);
				}
				else
				{
					strncat(result_string, ",", sizeof(result_string) - 1);
				}
			}
			debug_log(result_string);

			jlog("\n");
			/* phoneme sequence */
			jlog("phseq%d:", n + 1);
			put_hypo_phoneme(seq, seqnum, winfo);
			jlog("\n");
			/* confidence scores */
			jlog("cmscore%d:", n + 1);
			for (i = 0; i < seqnum; i++) jlog(" %5.3f", s->confidence[i]);

			strncat(result_string, "confidence_score,", sizeof(result_string) - 1);
			for (i = 0; i < seqnum; i++)
			{
				snprintf(temp_buffer, sizeof(temp_buffer), "%5.3f", s->confidence[i]);
				strncat(result_string, temp_buffer, sizeof(result_string) - 1);
				if (i + 1 == seqnum)
				{
					strncat(result_string, "\n", sizeof(result_string) - 1);
				}
				else
				{
					strncat(result_string, ",", sizeof(result_string) - 1);
				}
			}


			jlog("\n");
			/* AM and LM scores */
			jlog("score%d: %f", n + 1, s->score);
			if (r->lmtype == LM_PROB) { /* if this process uses N-gram */
				jlog(" (AM: %f  LM: %f)", s->score_am, s->score_lm);
			}

			strncat(result_string, "total_score,", sizeof(result_string) - 1);
			snprintf(temp_buffer, sizeof(temp_buffer), "%f", s->score);
			strncat(result_string, temp_buffer, sizeof(result_string) - 1);
			strncat(result_string, "\n", sizeof(result_string) - 1);

			strncat(result_string, "acoustic_model_score,", sizeof(result_string) - 1);
			snprintf(temp_buffer, sizeof(temp_buffer), "%f", s->score_am);
			strncat(result_string, temp_buffer, sizeof(result_string) - 1);
			strncat(result_string, "\n", sizeof(result_string) - 1);

			strncat(result_string, "language_model_score,", sizeof(result_string) - 1);
			snprintf(temp_buffer, sizeof(temp_buffer), "%f", s->score_lm);
			strncat(result_string, temp_buffer, sizeof(result_string) - 1);
			strncat(result_string, "\n", sizeof(result_string) - 1);

			jlog("\n");
			if (r->lmtype == LM_DFA) { /* if this process uses DFA grammar */
									   /* output which grammar the hypothesis belongs to
									   when using multiple grammars */
				if (multigram_get_all_num(r->lm) > 1) {
					jlog("grammar%d: %d\n", n + 1, s->gram_id);
				}
			}

			/* output alignment result if exist */
			for (align = s->align; align; align = align->next) {
				jlog("=== begin forced alignment ===\n");
				switch (align->unittype) {
				case PER_WORD:
					jlog("-- word alignment --\n"); break;
				case PER_PHONEME:
					jlog("-- phoneme alignment --\n"); break;
				case PER_STATE:
					jlog("-- state alignment --\n"); break;
				}
				jlog(" id: from  to    n_score    unit\n");
				jlog(" ----------------------------------------\n");
				for (i = 0; i < align->num; i++) {
					jlog("[%4d %4d]  %f  ", align->begin_frame[i], align->end_frame[i], align->avgscore[i]);
					switch (align->unittype) {
					case PER_WORD:
						jlog("%s\t[%s]\n", winfo->wname[align->w[i]], winfo->woutput[align->w[i]]);
						break;
					case PER_PHONEME:
						p = align->ph[i];
						if (p->is_pseudo) {
							jlog("{%s}\n", p->name);
						}
						else if (strmatch(p->name, p->body.defined->name)) {
							jlog("%s\n", p->name);
						}
						else {
							jlog("%s[%s]\n", p->name, p->body.defined->name);
						}
						break;
					case PER_STATE:
						p = align->ph[i];
						if (p->is_pseudo) {
							jlog("{%s}", p->name);
						}
						else if (strmatch(p->name, p->body.defined->name)) {
							jlog("%s", p->name);
						}
						else {
							jlog("%s[%s]", p->name, p->body.defined->name);
						}
						if (r->am->hmminfo->multipath) {
							if (align->is_iwsp[i]) {
								jlog(" #%d (sp)\n", align->loc[i]);
							}
							else {
								jlog(" #%d\n", align->loc[i]);
							}
						}
						else {
							jlog(" #%d\n", align->loc[i]);
						}
						break;
					}
				}

				jlog("re-computed AM score: %f\n", align->allscore);

				jlog("=== end forced alignment ===\n");
			}
		}
	}

	setlocale(LC_ALL, "japanese");
	size_t convert_count;
	wchar_t wstr[65535];
	mbstowcs_s(&convert_count, wstr, result_string, strlen(result_string) + 1);

	// output_result(result_string);
	output_result(wstr);

	/* flush output buffer */
	fflush(stdout);
}

static void put_hypo_phoneme(WORD_ID *seq, int n, WORD_INFO *winfo)
{
	int i, j;
	WORD_ID w;
	static char buf[MAX_HMMNAME_LEN];

	if (seq != NULL) {
		for (i = 0; i < n; i++) {
			if (i > 0) jlog(" |");
			w = seq[i];
			for (j = 0; j < winfo->wlen[w]; j++) {
				center_name(winfo->wseq[w][j]->name, buf);
				jlog(" %s", buf);
			}
		}
	}
	jlog("\n");
}

static boolean adin_standby(int _, void* __)
{
	return true;
}

static boolean ad_begin(char* _)
{
	return true;
}

static boolean ad_end()
{
	return true;
}

static char* ad_input_name()
{
	return nullptr;
}

static boolean ad_pause()
{
	return true;
}

static boolean ad_terminate()
{
	return true;
}

static boolean ad_resume()
{
	return true;
}

bool create_engine()
{
	ADIn* ad_in;

	jconf->input.type = INPUT_WAVEFORM;
	jconf->input.speech_input = SP_MIC;
	jconf->decodeopt.realtime_flag = true;

	recog = j_create_instance_from_jconf(jconf);
	if (recog == NULL) {
		return false;
	}

	// Register callbacks
	callback_add(recog, CALLBACK_EVENT_PROCESS_ONLINE, on_process_online, nullptr);
	callback_add(recog, CALLBACK_EVENT_PROCESS_OFFLINE, on_process_offline, nullptr);
	callback_add(recog, CALLBACK_EVENT_SPEECH_READY, on_speech_ready, nullptr);
	callback_add(recog, CALLBACK_EVENT_SPEECH_START, on_speech_start, nullptr);
	callback_add(recog, CALLBACK_EVENT_SPEECH_STOP, on_speech_stop, nullptr);
	callback_add(recog, CALLBACK_EVENT_RECOGNITION_BEGIN, on_recognition_begin, nullptr);
	callback_add(recog, CALLBACK_EVENT_RECOGNITION_END, on_recognition_end, nullptr);
	callback_add(recog, CALLBACK_EVENT_PASS1_FRAME, on_pass1_frame, nullptr);
	callback_add(recog, CALLBACK_EVENT_PAUSE, on_pause, nullptr);
	callback_add(recog, CALLBACK_EVENT_RESUME, on_resume, nullptr);
	callback_add(recog, CALLBACK_RESULT, on_result, nullptr);
	callback_add(recog, CALLBACK_PAUSE_FUNCTION, on_pause_function, nullptr);

	j_adin_init(recog);

	/*
	recog->adin->ad_standby = adin_standby;
	recog->adin->ad_begin = ad_begin;
	recog->adin->ad_end = ad_end;
	recog->adin->ad_input_name = ad_input_name;
	recog->adin->ad_read = audio_read_callback;
	recog->adin->ad_pause = ad_pause;
	recog->adin->ad_terminate = ad_terminate;
	recog->adin->ad_resume = ad_resume;
	*/

	jlog("認識情報");
	j_recog_info(recog);
	jlog("ストリーム開く");
	j_open_stream(recog, NULL);
	jlog("ストリーム開けた");

	return true;
}
