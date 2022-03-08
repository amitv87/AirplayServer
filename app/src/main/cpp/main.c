#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

// #include <dns_sd.h>
#include "mDNSResponder/mDNSShared/dns_sd.h"
#include "lib/raop.h"
#include "lib/logger.h"
#include "lib/stream.h"

#define LOG(fmt, ...) printf(fmt "\r\n", ##__VA_ARGS__)

static void audio_process(void *cls, pcm_data_struct *data){
  LOG("audio_process cls: %p, len: %u", cls, data->data_len);
}
static void video_process(void *cls, h264_decode_struct *data){
  LOG("video_process cls: %p, len: %u", cls, data->data_len);
}
static void audio_flush(void *cls, void *session){
  LOG("audio_flush cls: %p, session: %p", cls, session);
}
static void audio_set_volume(void *cls, void *opaque, float volume){
  LOG("audio_set_volume cls: %p, volume: %f", cls, volume);
}
static void audio_set_metadata(void *cls, void *session, const void *buffer, int buflen){
  LOG("audio_set_metadata cls: %p, session: %p, len: %d", cls, session, buflen);
}
static void audio_set_coverart(void *cls, void *session, const void *buffer, int buflen){
  LOG("audio_set_coverart cls: %p, session: %p, len: %d", cls, session, buflen);
}
static void audio_remote_control_id(void *cls, const char *dacp_id, const char *active_remote_header){
  LOG("audio_remote_control_id cls: %p, dacp_id: %s, active_remote_header: %s", cls, dacp_id, active_remote_header);
}
static void audio_set_progress(void *cls, void *session, unsigned int start, unsigned int curr, unsigned int end){
  LOG("audio_set_progress cls: %p, session: %p, start: %u, curr: %u, end: %u", cls, session, start, curr, end);
}

static void log_callback(void *cls, int level, const char *msg) {
  switch (level) {
    case LOGGER_DEBUG: LOG("DBG|" "%s", msg); break;
    case LOGGER_WARNING: LOG("WRN|" "%s", msg); break;
    case LOGGER_INFO: LOG("INF|" "%s", msg); break;
    case LOGGER_ERR: LOG("ERR|" "%s", msg); break;
    default: LOG("UNK|" "%s", msg); break;
  }
}

static void DNSSD_API ServiceRegisterReply(DNSServiceRef sdRef, DNSServiceFlags flags, DNSServiceErrorType errorCode, const char *serviceName, const char *regType, const char *domain, void *context){
  LOG("ServiceRegisterReply sdRef: %p, flags: %u, errorCode: %d, serviceName: %s, regType: %s, domain: %s, context: %p",
    sdRef, flags, errorCode, serviceName, regType, domain, context);
}

static bool should_run = true;
void sig_term_handler(int signum, siginfo_t *info, void *ptr){
  printf("\r");
  should_run = false;
}

int main(int argc, char const *argv[]){

  struct sigaction _sigact;
  memset(&_sigact, 0, sizeof(_sigact));
  _sigact.sa_sigaction = sig_term_handler;
  _sigact.sa_flags = SA_SIGINFO;
  sigaction(SIGINT, &_sigact, NULL);
  sigaction(SIGTERM, &_sigact, NULL);

  raop_callbacks_t raop_cbs = {
    .audio_process = audio_process,
    .video_process = video_process,
    .audio_flush = audio_flush,
    .audio_set_volume = audio_set_volume,
    .audio_set_metadata = audio_set_metadata,
    .audio_set_coverart = audio_set_coverart,
    .audio_remote_control_id = audio_remote_control_id,
    .audio_set_progress = audio_set_progress,
  };
  raop_t *raop = raop_init(10, &raop_cbs);
  raop_set_log_callback(raop, log_callback, NULL);
  raop_set_log_level(raop, RAOP_LOG_DEBUG);

  unsigned short port = 0;
  raop_start(raop, &port);
  raop_set_port(raop, port);
  LOG("raop port = % d", raop_get_port(raop));

  DNSServiceErrorType rc;
  TXTRecordRef txtRecord = {0};
  #define TXT_RECORD_SET(k,v) TXTRecordSetValue(&txtRecord, k, sizeof(v) - 1, v);

  TXTRecordCreate(&txtRecord, 0, NULL);
  TXT_RECORD_SET("deviceid", "f8:4d:89:96:36:27");
  TXT_RECORD_SET("features", "0x5A7FFFF7,0x1E");
  TXT_RECORD_SET("srcvers", "220.68");
  TXT_RECORD_SET("flags", "0x4");
  TXT_RECORD_SET("vv", "2");
  TXT_RECORD_SET("model", "AppleTV2,1");
  TXT_RECORD_SET("pw", "false");
  TXT_RECORD_SET("rhd", "5.6.0.0");
  TXT_RECORD_SET("pk", "b07727d6f6cd6e08b58ede525ec3cdeaa252ad9f683feb212ef8a205246554e7");
  TXT_RECORD_SET("pi", "2e388006-13ba-4041-9a67-25dd4a43d536");

  DNSServiceRef airplay_ServiceRef = NULL;
  rc = DNSServiceRegister(&airplay_ServiceRef, 0, 0, "t1", "_airplay._tcp", "local.", "", htons(port),
    TXTRecordGetLength(&txtRecord), TXTRecordGetBytesPtr(&txtRecord), ServiceRegisterReply, raop);
  LOG("DNSServiceRegister: %d, airplay_ServiceRef: %p", rc, airplay_ServiceRef);
  TXTRecordDeallocate(&txtRecord);

  TXTRecordCreate(&txtRecord, 0, NULL);
  TXT_RECORD_SET("ch", "2");
  TXT_RECORD_SET("cn", "0,1,2,3");
  TXT_RECORD_SET("da", "true");
  TXT_RECORD_SET("et", "0,3,5");
  TXT_RECORD_SET("vv", "2");
  TXT_RECORD_SET("ft", "0x5A7FFFF7,0x1E");
  TXT_RECORD_SET("am", "AppleTV2,1");
  TXT_RECORD_SET("md", "0,1,2");
  TXT_RECORD_SET("rhd", "5.6.0.0");
  TXT_RECORD_SET("pw", "false");
  TXT_RECORD_SET("sr", "44100");
  TXT_RECORD_SET("ss", "16");
  TXT_RECORD_SET("sv", "false");
  TXT_RECORD_SET("tp", "UDP");
  TXT_RECORD_SET("txtvers", "1");
  TXT_RECORD_SET("sf", "0x4");
  TXT_RECORD_SET("vs", "220.68");
  TXT_RECORD_SET("vn", "65537");
  TXT_RECORD_SET("pk", "b07727d6f6cd6e08b58ede525ec3cdeaa252ad9f683feb212ef8a205246554e7");

  DNSServiceRef raop_ServiceRef = NULL;
  rc = DNSServiceRegister(&raop_ServiceRef, 0, 0, "f84d89963627@t1", "_raop._tcp", "local.", "", htons(port),
    TXTRecordGetLength(&txtRecord), TXTRecordGetBytesPtr(&txtRecord), ServiceRegisterReply, raop);
  LOG("DNSServiceRegister: %d, raop_ServiceRef: %p", rc, raop_ServiceRef);
  TXTRecordDeallocate(&txtRecord);

  while(should_run) sleep(1);
  if(airplay_ServiceRef) DNSServiceRefDeallocate(airplay_ServiceRef);
  if(raop_ServiceRef) DNSServiceRefDeallocate(raop_ServiceRef);
  raop_stop(raop);
  LOG("bye :(");
  return 0;
}
