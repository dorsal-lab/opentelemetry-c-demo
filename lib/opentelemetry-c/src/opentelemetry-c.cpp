#include "opentelemetry-c.h"

#include "utils/lttng_span_exporter.h"
#include "utils/map.h"
#include "utils/socket_carrier.h"

#include <cstddef>
#include <opentelemetry/context/propagation/global_propagator.h>
#include <opentelemetry/context/propagation/text_map_propagator.h>
#include <opentelemetry/context/runtime_context.h>
#include <opentelemetry/exporters/ostream/span_exporter.h>
#include <opentelemetry/sdk/trace/simple_processor.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/trace/propagation/http_trace_context.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/tracer.h>
#include <string>

namespace context = opentelemetry::context;
namespace trace = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;

void *init_tracer(char *service_name, char *service_version) {
	auto exporter = std::unique_ptr<trace_sdk::SpanExporter>(
	    new opentelemetry::exporter::trace::LttngSpanExporter);
	auto processor = std::unique_ptr<trace_sdk::SpanProcessor>(
	    new trace_sdk::SimpleSpanProcessor(std::move(exporter)));
	std::vector<std::unique_ptr<trace_sdk::SpanProcessor>> processors;
	processors.push_back(std::move(processor));
	// Default is an always-on sampler.
	auto context =
	    std::make_shared<trace_sdk::TracerContext>(std::move(processors));
	auto provider =
	    opentelemetry::nostd::shared_ptr<opentelemetry::trace::TracerProvider>(
	        new trace_sdk::TracerProvider(context));
	// Set the global trace provider
	opentelemetry::trace::Provider::SetTracerProvider(provider);

	// set global propagator
	opentelemetry::context::propagation::GlobalTextMapPropagator::
	    SetGlobalPropagator(
	        opentelemetry::nostd::shared_ptr<
	            opentelemetry::context::propagation::TextMapPropagator>(
	            new opentelemetry::trace::propagation::HttpTraceContext()));

	auto *params = new TracerParams;
	params->service_name = std::string(service_name);
	params->service_version = std::string(service_version);
	return params;
}

void destroy_tracer(void *tracer_params) {
	delete static_cast<TracerParams *>(tracer_params);
}

void *start_span(void *tracer_params, char *span_name, char *context_s) {
	auto *params = new SpanParams;
	params->tracer_params = static_cast<TracerParams *>(tracer_params);
	params->span_name = std::string(span_name);
	params->context =
	    (context_s == NULL) ? "" : std::string(context_s); // NOLINT
	params->start_system_time = opentelemetry::common::SystemTimestamp(
	    std::chrono::system_clock::now());
	return params;
}

// char *extract_context_from_span(void *span) {
// 	auto *span_p = static_cast<trace::Span *>(span);
// 	auto ctx = span_p->GetContext();
// 	auto prop =
// 	    context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
// 	SocketTextMapCarrier carrier;
// 	prop->Inject(carrier, ctx);
// 	std::string ctx_s = carrier.Serialize();
// 	const auto size = ctx_s.size();
// 	char *buffer = new char[size + 1]; // NOLINT
// 	memcpy(buffer, ctx_s.c_str(), size + 1);
// 	return buffer;
// }

void end_span(void *span_params) {
	auto *params = static_cast<SpanParams *>(span_params);

	// Span options
	trace::StartSpanOptions options;
	options.start_system_time = params->start_system_time;
	// TODO(augustinsangam): Give ability to change
	options.kind = trace::SpanKind::kInternal;

	// Context extraction
	if (!params->context.empty()) { // NOLINT
		auto prop = context::propagation::GlobalTextMapPropagator::
		    GetGlobalPropagator();
		auto current_ctx = context::RuntimeContext::GetCurrent();
		std::map<std::string, std::string> context_map =
		    deserialize_map(params->context);
		const SocketTextMapCarrier carrier(params->context);
		prop->Extract(carrier, current_ctx);
	}

	// Create the span
	auto provider = opentelemetry::trace::Provider::GetTracerProvider();
	auto tracer = provider->GetTracer(params->tracer_params->service_name,
	                                  params->tracer_params->service_version);
	auto span = tracer->StartSpan(params->span_name, options);
	span->End();

	delete params;
}
