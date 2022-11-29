#ifndef OPENTELEMETRY_C_H
#define OPENTELEMETRY_C_H

#include <string>

#include <opentelemetry/common/timestamp.h>

struct TracerParams {
	std::string service_name;
	std::string service_version;
};

struct SpanParams {
	TracerParams *tracer_params;
	std::string span_name;
	std::string context;
	opentelemetry::common::SystemTimestamp start_system_time;
};

// #ifdef __cplusplus
extern "C" {
// #endif

void *init_tracer(char *service_name, char *service_version);
void destroy_tracer(void *tracer_params);

void *start_span(void *tracer_params, char *span_name, char *context_s);
char *extract_context_from_span(void *span);
// TODO(augustinsangam): Add support for span->SetStatus
void end_span(void *span_params);

// #ifdef __cplusplus
}
// #endif

#endif // !OPENTELEMETRY_C_H
