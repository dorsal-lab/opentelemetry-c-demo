#define TRACEPOINT_CREATE_PROBES
#define TRACEPOINT_DEFINE

#include <opentelemetry/exporters/otlp/otlp_recordable.h>
#include <opentelemetry/proto/trace/v1/trace.pb.h>
#include <opentelemetry/sdk/trace/span_data.h>
#include <string>

#include "utils/lttng_span_exporter.h"
#include "utils/lttng_span_exporter_lttng_tracepoint.h"

#include <iostream>

namespace otlp_exporter = opentelemetry::exporter::otlp;
namespace sdk_trace = opentelemetry::sdk::trace;

opentelemetry::v1::exporter::trace::LttngSpanExporter::
    LttngSpanExporter() noexcept = default;

std::unique_ptr<sdk_trace::Recordable> opentelemetry::v1::exporter::trace::
    LttngSpanExporter::MakeRecordable() noexcept {
	return std::unique_ptr<sdk_trace::Recordable>(
	    new otlp_exporter::OtlpRecordable());
}

opentelemetry::sdk::common::ExportResult
opentelemetry::v1::exporter::trace::LttngSpanExporter::Export(
    const opentelemetry::nostd::span<std::unique_ptr<sdk_trace::Recordable>>
        &spans) noexcept {
	for (auto &recordable_span : spans) {
		auto span = std::unique_ptr<otlp_exporter::OtlpRecordable>(
		    static_cast<otlp_exporter::OtlpRecordable *>(
		        recordable_span.release()));
		std::string span_serialized = span->span().SerializeAsString();
		lttng_ust_tracepoint(opentelemetry, otel_span,
		                     span_serialized.c_str());
		std::string span_debug = span->span().DebugString();
		lttng_ust_tracepoint(opentelemetry, otel_debug_span,
		                     span_debug.c_str());
	}
	return opentelemetry::sdk::common::ExportResult::kSuccess;
}

bool opentelemetry::v1::exporter::trace::LttngSpanExporter::Shutdown(
    std::chrono::microseconds /*timeout*/) noexcept {
	return true;
}
