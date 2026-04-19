//
// Created by kaede on 2026/4/18.
//

#ifndef PRECOMPUTER_KTXWRITER_HPP
#define PRECOMPUTER_KTXWRITER_HPP

#include <filesystem>

#include "BrdfLutBaker.hpp"
#include "EnvironmentBaker.hpp"

class KTXWriter {
public:
    static void writeBrdfLut(const BrdfLutBakeResult& image, const std::filesystem::path& output_path);
    static void writeEnvironmentCubemap(const EnvironmentCubemapBakeResult& image, const std::filesystem::path& output_path);
    static void writePrefilterCubemap(const PrefilterCubemapBakeResult& image, const std::filesystem::path& output_path);
};



#endif //PRECOMPUTER_KTXWRITER_HPP
