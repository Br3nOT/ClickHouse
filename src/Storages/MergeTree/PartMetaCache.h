#pragma once

#include <city.h>
#include <Core/Types.h>
#include <Interpreters/Context.h>


namespace DB
{

class SeekableReadBuffer;
class IMergeTreeDataPart;
class PartMetaCache;
using PartMetaCachePtr = std::shared_ptr<PartMetaCache>;

class PartMetaCache
{
public:
    using uint128 = CityHash_v1_0_2::uint128;

    PartMetaCache(const MergeTreeMetaCachePtr & cache_, const String & relative_data_path_, const String & relative_path_, const IMergeTreeDataPart * parent_part_)
        : cache(cache_)
        , relative_data_path(relative_data_path_)
        , relative_path(relative_path_)
        , parent_part(parent_part_)
    {
    }

    std::unique_ptr<SeekableReadBuffer>
    readOrSetMeta(const DiskPtr & disk, const String & file_name, String & value);
    void setMetas(const DiskPtr & disk, const Strings & file_names);
    void dropMetas(const Strings & file_names);
    void setMeta(const String & file_name, const String & value);
    void getFilesAndCheckSums(Strings & file_names, std::vector<uint128> & checksums) const;

private:
    std::string getFullRelativePath() const;

    MergeTreeMetaCachePtr cache;
    const String & relative_data_path; // relative path of table to disk
    const String & relative_path; // relative path of part to table
    const IMergeTreeDataPart * parent_part;
};

}
