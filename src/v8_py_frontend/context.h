#ifndef INCLUDE_MINI_RACER_CONTEXT_H
#define INCLUDE_MINI_RACER_CONTEXT_H

#include <v8-platform.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include "binary_value.h"
#include "callback.h"
#include "cancelable_task_runner.h"
#include "code_evaluator.h"
#include "context_holder.h"
#include "heap_reporter.h"
#include "isolate_manager.h"
#include "isolate_memory_monitor.h"
#include "object_manipulator.h"
#include "promise_attacher.h"

namespace MiniRacer {

class Context {
 public:
  explicit Context(v8::Platform* platform);

  void SetHardMemoryLimit(size_t limit);
  void SetSoftMemoryLimit(size_t limit);

  [[nodiscard]] auto IsSoftMemoryLimitReached() const -> bool;
  [[nodiscard]] auto IsHardMemoryLimitReached() const -> bool;
  void ApplyLowMemoryNotification();

  void FreeBinaryValue(BinaryValueHandle* val);
  template <typename... Params>
  auto AllocBinaryValue(Params&&... params) -> BinaryValueHandle*;
  void CancelTask(uint64_t task_id);
  auto HeapSnapshot(Callback callback, uint64_t callback_id) -> uint64_t;
  auto HeapStats(Callback callback, uint64_t callback_id) -> uint64_t;
  auto Eval(BinaryValueHandle* code_handle,
            Callback callback,
            uint64_t callback_id) -> uint64_t;
  auto AttachPromiseThen(BinaryValueHandle* promise_handle,
                         Callback callback,
                         uint64_t callback_id) -> BinaryValueHandle*;
  auto GetIdentityHash(BinaryValueHandle* obj_handle) -> BinaryValueHandle*;
  auto GetOwnPropertyNames(BinaryValueHandle* obj_handle) -> BinaryValueHandle*;
  auto GetObjectItem(BinaryValueHandle* obj_handle,
                     BinaryValueHandle* key_handle) -> BinaryValueHandle*;
  auto SetObjectItem(BinaryValueHandle* obj_handle,
                     BinaryValueHandle* key_handle,
                     BinaryValueHandle* val_handle) -> BinaryValueHandle*;
  auto DelObjectItem(BinaryValueHandle* obj_handle,
                     BinaryValueHandle* key_handle) -> BinaryValueHandle*;
  auto SpliceArray(BinaryValueHandle* obj_handle,
                   int32_t start,
                   int32_t delete_count,
                   BinaryValueHandle* new_val_handle) -> BinaryValueHandle*;
  auto CallFunction(BinaryValueHandle* func_handle,
                    BinaryValueHandle* this_handle,
                    BinaryValueHandle* argv_handle,
                    Callback callback,
                    uint64_t callback_id) -> uint64_t;
  auto BinaryValueCount() -> size_t;

 private:
  template <typename Runnable>
  auto RunTask(Runnable runnable,
               Callback callback,
               uint64_t callback_id) -> uint64_t;

  std::shared_ptr<IsolateManager> isolate_manager_;
  std::shared_ptr<IsolateMemoryMonitor> isolate_memory_monitor_;
  std::shared_ptr<BinaryValueFactory> bv_factory_;
  std::shared_ptr<ContextHolder> context_holder_;
  std::shared_ptr<CodeEvaluator> code_evaluator_;
  std::shared_ptr<HeapReporter> heap_reporter_;
  std::shared_ptr<PromiseAttacher> promise_attacher_;
  std::shared_ptr<ObjectManipulator> object_manipulator_;
  std::shared_ptr<CancelableTaskRunner> cancelable_task_runner_;
};

inline void Context::SetHardMemoryLimit(size_t limit) {
  isolate_memory_monitor_->SetHardMemoryLimit(limit);
}

inline void Context::SetSoftMemoryLimit(size_t limit) {
  isolate_memory_monitor_->SetSoftMemoryLimit(limit);
}

inline auto Context::IsSoftMemoryLimitReached() const -> bool {
  return isolate_memory_monitor_->IsSoftMemoryLimitReached();
}

inline auto Context::IsHardMemoryLimitReached() const -> bool {
  return isolate_memory_monitor_->IsHardMemoryLimitReached();
}

inline void Context::ApplyLowMemoryNotification() {
  isolate_memory_monitor_->ApplyLowMemoryNotification();
}

template <typename... Params>
inline auto Context::AllocBinaryValue(Params&&... params)
    -> BinaryValueHandle* {
  return bv_factory_->New(std::forward<Params>(params)...)->GetHandle();
}

}  // namespace MiniRacer

#endif  // INCLUDE_MINI_RACER_CONTEXT_H
