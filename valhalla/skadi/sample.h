#ifndef __VALHALLA_SAMPLE_H__
#define __VALHALLA_SAMPLE_H__

#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

#include <boost/property_tree/ptree.hpp>

#include "valhalla/baldr/tilegetter.h"

namespace valhalla {
namespace skadi {

struct cache_t;

class sample {
public:
  // non-default-constructable and non-copyable
  sample() = delete;
  sample(sample&&) = delete;
  sample& operator=(sample&&) = delete;
  sample(const sample&) = delete;
  sample& operator=(const sample&) = delete;

  /**
   * @brief Constructor
   * @param[in] config  Configuration settings
   */
  sample(const boost::property_tree::ptree& config);

  /// TODO(neyromancer): combine both constructors in one with config as an input parameter
  /// when valhalla_benchmark_skadi start using config instead of folder
  /**
   * @brief Constructor
   * @param[in] data_source  directory name of the datasource from which to sample
   */
  sample(const std::string& data_source);
  ~sample();

  /**
   * Get a single sample from the datasource
   * @param coord  the single posting at which to sample the datasource
   */
  template <class coord_t> double get(const coord_t& coord);

  /**
   * Get multiple samples from the datasource
   * @param coords  the list of postings at which to sample the datasource
   */
  template <class coords_t> std::vector<double> get_all(const coords_t& coords);

  /**
   * @brief Add tile to cache and store it in local filesystem.
   * @param[in] path path to the tile
   * @param[in] raw_data Data to store.
   */
  bool store(const std::string& path, const std::vector<char>& raw_data);

protected:
  /**
   * @return A tile index value from a coordinate
   */
  template <class coord_t> static uint16_t get_tile_index(const coord_t& coord);

  /**
   * Adds single tile in cache. Used only in tests
   * @param path path to the tile
   */
  void add_single_tile(const std::string& path);

  /**
   * Get a single sample from a remote source
   * @param coord the single posting at which to sample the datasource
   */
  template <class coord_t> double get_from_remote(const coord_t& coord);

  /**
   * Get a single sample from cache
   * @param coord the single posting at which to sample the datasource
   */
  template <class coord_t> double get_from_cache(const coord_t& coord);

  friend cache_t;
  std::unique_ptr<cache_t> cache_;

private:
  /**
   * @brief used to warm cache
   * @param[in] source_path File local storage directory.
   */
  void cache_initialisation(const std::string& source_path);

  std::string url_;
  std::unique_ptr<baldr::tile_getter_t> remote_loader_;
  std::unordered_set<std::string> st_;
  std::mutex st_lck_;
  std::uint32_t num_threads_{1};
  // This parameter is used only in tests
  std::string remote_path_;
};

/**
 * @return The file name of a tile for a given index
 */
std::string get_hgt_file_name(uint16_t index);

/**
 * @return the no data value for this data source
 */
double get_no_data_value();

} // namespace skadi
} // namespace valhalla

#endif //__VALHALLA_SAMPLE_H__
