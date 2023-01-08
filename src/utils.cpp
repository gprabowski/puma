#include <utils.hpp>

namespace pusn {
namespace utils {
std::string read_text_file(std::filesystem::path shader_file) {
  std::ifstream ifs;

  auto ex = ifs.exceptions();
  ex |= std::ios_base::badbit | std::ios_base::failbit;
  ifs.exceptions(ex);

  ifs.open(shader_file);
  ifs.ignore(std::numeric_limits<std::streamsize>::max());
  auto size = ifs.gcount();
  LOGGER_INFO("[FILE] Read {0} bytes from {1}", size, shader_file.string());

  ifs.clear();
  ifs.seekg(0, std::ios_base::beg);

  return std::string{std::istreambuf_iterator<char>{ifs}, {}};
}

std::vector<std::string>
read_text_lines_file(const std::filesystem::path input_file) {
  std::vector<std::string> ret;
  std::ifstream ifs;

  auto ex = ifs.exceptions();
  ex |= std::ios_base::badbit | std::ios_base::failbit;
  ifs.exceptions(ex);

  ifs.open(input_file);
  ifs.ignore(std::numeric_limits<std::streamsize>::max());
  auto size = ifs.gcount();
  LOGGER_INFO("[FILE] Read {0} bytes from {1}", size, input_file.string());

  ifs.clear();
  ifs.seekg(0, std::ios_base::beg);

  auto full_program = std::string{std::istreambuf_iterator<char>{ifs}, {}};
  std::stringstream ss(full_program);

  std::string current;
  while (getline(ss, current)) {
    ret.push_back(current);
  }
  return ret;
}
} // namespace utils
} // namespace pusn
