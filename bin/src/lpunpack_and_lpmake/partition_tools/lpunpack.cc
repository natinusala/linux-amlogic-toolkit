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

#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <sysexits.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <android-base/file.h>
#include <android-base/parseint.h>
#include <liblp/liblp.h>
#include <sparse/sparse.h>

using namespace android::fs_mgr;
using android::base::unique_fd;
using SparsePtr = std::unique_ptr<sparse_file, decltype(&sparse_file_destroy)>;

class ImageExtractor final {
  public:
    ImageExtractor(unique_fd&& image_fd, std::unique_ptr<LpMetadata>&& metadata,
                   std::unordered_set<std::string>&& partitions, const std::string& output_dir);

    bool Extract();

  private:
    bool BuildPartitionList();
    bool ExtractPartition(const LpMetadataPartition* partition);
    bool ExtractExtent(const LpMetadataExtent& extent, int output_fd);

    unique_fd image_fd_;
    std::unique_ptr<LpMetadata> metadata_;
    std::unordered_set<std::string> partitions_;
    std::string output_dir_;
    std::unordered_map<std::string, const LpMetadataPartition*> partition_map_;
};

// Note that "sparse" here refers to filesystem sparse, not the Android sparse
// file format.
class SparseWriter final {
  public:
    SparseWriter(int output_fd, int image_fd, uint32_t block_size);

    bool WriteExtent(const LpMetadataExtent& extent);
    bool Finish();

  private:
    bool WriteBlock(const uint8_t* data);

    int output_fd_;
    int image_fd_;
    uint32_t block_size_;
    off_t hole_size_ = 0;
};

/* Prints program usage to |where|. */
static int usage(int /* argc */, char* argv[]) {
    fprintf(stderr,
            "%s - command-line tool for extracting partition images from super\n"
            "\n"
            "Usage:\n"
            "  %s [options...] SUPER_IMAGE [OUTPUT_DIR]\n"
            "\n"
            "Options:\n"
            "  -p, --partition=NAME     Extract the named partition. This can\n"
            "                           be specified multiple times.\n"
            "  -S, --slot=NUM           Slot number (default is 0).\n",
            argv[0], argv[0]);
    return EX_USAGE;
}

int main(int argc, char* argv[]) {
    // clang-format off
    struct option options[] = {
        { "partition",  required_argument,  nullptr, 'p' },
        { "slot",       required_argument,  nullptr, 'S' },
        { nullptr,      0,                  nullptr, 0 },
    };
    // clang-format on

    uint32_t slot_num = 0;
    std::unordered_set<std::string> partitions;

    int rv, index;
    while ((rv = getopt_long_only(argc, argv, "+p:sh", options, &index)) != -1) {
        switch (rv) {
            case 'h':
                usage(argc, argv);
                return EX_OK;
            case '?':
                std::cerr << "Unrecognized argument.\n";
                return usage(argc, argv);
            case 'S':
                if (!android::base::ParseUint(optarg, &slot_num)) {
                    std::cerr << "Slot must be a valid unsigned number.\n";
                    return usage(argc, argv);
                }
                break;
            case 'p':
                partitions.emplace(optarg);
                break;
        }
    }

    if (optind + 1 > argc) {
        std::cerr << "Missing super image argument.\n";
        return usage(argc, argv);
    }
    std::string super_path = argv[optind++];

    std::string output_dir = ".";
    if (optind + 1 <= argc) {
        output_dir = argv[optind++];
    }

    if (optind < argc) {
        std::cerr << "Unrecognized command-line arguments.\n";
        return usage(argc, argv);
    }

    // Done reading arguments; open super.img. PartitionOpener will decorate
    // relative paths with /dev/block/by-name, so get an absolute path here.
    std::string abs_super_path;
    if (!android::base::Realpath(super_path, &abs_super_path)) {
        std::cerr << "realpath failed: " << super_path << ": " << strerror(errno) << "\n";
        return EX_OSERR;
    }

    unique_fd fd(open(super_path.c_str(), O_RDONLY | O_CLOEXEC));
    if (fd < 0) {
        std::cerr << "open failed: " << abs_super_path << ": " << strerror(errno) << "\n";
        return EX_OSERR;
    }

    auto metadata = ReadMetadata(abs_super_path, slot_num);
    if (!metadata) {
        SparsePtr ptr(sparse_file_import(fd, false, false), sparse_file_destroy);
        if (ptr) {
            std::cerr << "This image appears to be a sparse image. It must be "
                         "unsparsed to be"
                      << " unpacked.\n";
            return EX_USAGE;
        }
        std::cerr << "Image does not appear to be in super-partition format.\n";
        return EX_USAGE;
    }

    ImageExtractor extractor(std::move(fd), std::move(metadata), std::move(partitions), output_dir);
    if (!extractor.Extract()) {
        return EX_SOFTWARE;
    }
    return EX_OK;
}

ImageExtractor::ImageExtractor(unique_fd&& image_fd, std::unique_ptr<LpMetadata>&& metadata,
                               std::unordered_set<std::string>&& partitions,
                               const std::string& output_dir)
    : image_fd_(std::move(image_fd)),
      metadata_(std::move(metadata)),
      partitions_(std::move(partitions)),
      output_dir_(output_dir) {}

bool ImageExtractor::Extract() {
    if (!BuildPartitionList()) {
        return false;
    }

    for (const auto& [name, info] : partition_map_) {
        if (!ExtractPartition(info)) {
            return false;
        }
    }
    return true;
}

bool ImageExtractor::BuildPartitionList() {
    bool extract_all = partitions_.empty();

    for (const auto& partition : metadata_->partitions) {
        auto name = GetPartitionName(partition);
        if (extract_all || partitions_.count(name)) {
            partition_map_[name] = &partition;
            partitions_.erase(name);
        }
    }

    if (!extract_all && !partitions_.empty()) {
        std::cerr << "Could not find partition: " << *partitions_.begin() << "\n";
        return false;
    }
    return true;
}

bool ImageExtractor::ExtractPartition(const LpMetadataPartition* partition) {
    // Validate the extents and find the total image size.
    uint64_t total_size = 0;
    for (uint32_t i = 0; i < partition->num_extents; i++) {
        uint32_t index = partition->first_extent_index + i;
        const LpMetadataExtent& extent = metadata_->extents[index];

        if (extent.target_type != LP_TARGET_TYPE_LINEAR) {
            std::cerr << "Unsupported target type in extent: " << extent.target_type << "\n";
            return false;
        }
        if (extent.target_source != 0) {
            std::cerr << "Split super devices are not supported.\n";
            return false;
        }
        total_size += extent.num_sectors * LP_SECTOR_SIZE;
    }

    // Make a temporary file so we can import it with sparse_file_read.
    std::string output_path = output_dir_ + "/" + GetPartitionName(*partition) + ".img";
    unique_fd output_fd(open(output_path.c_str(), O_RDWR | O_CLOEXEC | O_CREAT | O_TRUNC, 0644));
    if (output_fd < 0) {
        std::cerr << "open failed: " << output_path << ": " << strerror(errno) << "\n";
        return false;
    }

    SparseWriter writer(output_fd, image_fd_, metadata_->geometry.logical_block_size);

    // Extract each extent into output_fd.
    for (uint32_t i = 0; i < partition->num_extents; i++) {
        uint32_t index = partition->first_extent_index + i;
        const LpMetadataExtent& extent = metadata_->extents[index];

        if (!writer.WriteExtent(extent)) {
            return false;
        }
    }
    return writer.Finish();
}

SparseWriter::SparseWriter(int output_fd, int image_fd, uint32_t block_size)
    : output_fd_(output_fd), image_fd_(image_fd), block_size_(block_size) {}

bool SparseWriter::WriteExtent(const LpMetadataExtent& extent) {
    auto buffer = std::make_unique<uint8_t[]>(block_size_);

    off_t super_offset = extent.target_data * LP_SECTOR_SIZE;
    if (lseek(image_fd_, super_offset, SEEK_SET) < 0) {
        std::cerr << "image lseek failed: " << strerror(errno) << "\n";
        return false;
    }

    uint64_t remaining_bytes = extent.num_sectors * LP_SECTOR_SIZE;
    while (remaining_bytes) {
        if (remaining_bytes < block_size_) {
            std::cerr << "extent is not block-aligned\n";
            return false;
        }
        if (!android::base::ReadFully(image_fd_, buffer.get(), block_size_)) {
            std::cerr << "read failed: " << strerror(errno) << "\n";
            return false;
        }
        if (!WriteBlock(buffer.get())) {
            return false;
        }
        remaining_bytes -= block_size_;
    }
    return true;
}

static bool ShouldSkipChunk(const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (data[i] != 0) {
            return false;
        }
    }
    return true;
}

bool SparseWriter::WriteBlock(const uint8_t* data) {
    if (ShouldSkipChunk(data, block_size_)) {
        hole_size_ += block_size_;
        return true;
    }

    if (hole_size_) {
        if (lseek(output_fd_, hole_size_, SEEK_CUR) < 0) {
            std::cerr << "lseek failed: " << strerror(errno) << "\n";
            return false;
        }
        hole_size_ = 0;
    }
    if (!android::base::WriteFully(output_fd_, data, block_size_)) {
        std::cerr << "write failed: " << strerror(errno) << "\n";
        return false;
    }
    return true;
}

bool SparseWriter::Finish() {
    if (hole_size_) {
        off_t offset = lseek(output_fd_, 0, SEEK_CUR);
        if (offset < 0) {
            std::cerr << "lseek failed: " << strerror(errno) << "\n";
            return false;
        }
        if (ftruncate(output_fd_, offset + hole_size_) < 0) {
            std::cerr << "ftruncate failed: " << strerror(errno) << "\n";
            return false;
        }
    }
    return true;
}
