#pragma once

#include <opentelemetry/common/spin_lock_mutex.h>
#include <opentelemetry/nostd/type_traits.h>
#include <opentelemetry/sdk/trace/exporter.h>
#include <opentelemetry/sdk/trace/span_data.h>
#include <opentelemetry/version.h>

#include <iostream>
#include <map>
#include <sstream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter {
namespace trace {

/**
 * The LttngSpanExporter exports span data through LTTng
 */
class LttngSpanExporter final : public opentelemetry::sdk::trace::SpanExporter {
public:
	explicit LttngSpanExporter() noexcept;

	std::unique_ptr<opentelemetry::sdk::trace::Recordable>
	MakeRecordable() noexcept override;

	sdk::common::ExportResult
	Export(const opentelemetry::nostd::span<
	       std::unique_ptr<opentelemetry::sdk::trace::Recordable>>
	           &spans) noexcept override;

	bool Shutdown(std::chrono::microseconds timeout =
	                  std::chrono::microseconds::max()) noexcept override;
};
} // namespace trace
} // namespace exporter
OPENTELEMETRY_END_NAMESPACE
