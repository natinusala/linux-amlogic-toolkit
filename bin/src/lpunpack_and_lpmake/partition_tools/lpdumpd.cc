/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include <android-base/logging.h>
#include <android-base/strings.h>
#include <android/lpdump/BnLpdump.h>
#include <android/lpdump/ILpdump.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>

int LpdumpMain(int argc, char* argv[], std::ostream&, std::ostream&);

namespace android {
namespace lpdump {

using binder::Status;

class Lpdump : public BnLpdump {
  public:
    Lpdump() = default;
    virtual ~Lpdump() = default;

    Status run(const std::vector<std::string>& args, std::string* aidl_return) override {
        std::vector<char*> local_argv;
        std::vector<std::string> local_args = args;
        for (auto& arg : local_args) {
            local_argv.push_back(arg.data());
        }
        LOG(DEBUG) << "Dumping with args: " << base::Join(args, " ");
        std::stringstream output;
        std::stringstream error;
        int ret = LpdumpMain((int)local_argv.size(), local_argv.data(), output, error);
        std::string error_str = error.str();
        if (ret == 0) {
            if (!error_str.empty()) {
                LOG(WARNING) << error_str;
            }
            *aidl_return = output.str();
            return Status::ok();
        } else {
            return Status::fromServiceSpecificError(ret, error_str.c_str());
        }
    }
};

}  // namespace lpdump
}  // namespace android

int main(int, char**) {
    using namespace android;

    sp<lpdump::Lpdump> service = new lpdump::Lpdump();
    defaultServiceManager()->addService(String16("lpdump_service"), service);
    LOG(VERBOSE) << "lpdumpd starting";
    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();
    return 0;
}
