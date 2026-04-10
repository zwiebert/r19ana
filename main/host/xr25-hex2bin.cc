// main.cc

#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#include "ConsoleTransport.hh"
#include "FrameProcessor.hh"
#include "UartTransport.hh"
#include "Xr25Transport.hh"
#include "cli.hh"
#include "main.hh"
#include "select_model.hh"

#define D(x)

Transport&& term_transport = ConsoleTransport();

static int hex_nibble(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}

void hex2voc(XR25Frame::voc_t& voc, const char* src) {
  voc.frame_len = 0;
  if (!src) return;

  unsigned dst_idx = 0;
  int high_nibble = -1;

  for (const char* p = src; *p != '\0'; ++p) {
    int val = hex_nibble(*p);
    if (val < 0) {
      // invalid char, abort and reset
      voc.frame_len = 0;
      return;
    }
    if (high_nibble < 0) {
      high_nibble = val;
      continue;
    }
    // complete byte
    if (dst_idx >= XR25Frame::FRAME_MAX_SIZE) {
      break;
    }
    voc.frame[dst_idx++] = static_cast<uint8_t>((high_nibble << 4) | val);
    high_nibble = -1;
  }

  // ignore final nibble if odd length
  voc.frame_len = dst_idx;
}

int main(int argc, char** argv) {
  constexpr const char* usage_txt =
      "Usage: xr25-hex2human  [OPTION...]\n"
      "\n"
      "  -i, --infile=FILE\n"
      "  -p, --line-prefix=PREFIX (default: \"01 \")\n"
      "  -o, --outfile=FILE\n"
      "\n";

  constexpr struct option long_options[] = {
      {"infile", required_argument, 0, 'i'},
      {"line-prefix", required_argument, 0, 'p'},
      {"outfile", required_argument, 0, 'o'},
      {"help", no_argument, 0, 'h'},
      {}};

  constexpr const char* short_options = "i:p:m:o:h";

  int c;
  int digit_optind = 0;

  const char* infile_name = nullptr;
  const char* outfile_name = nullptr;
  const char* model_name = "73PS";
  const char* line_prefix = "01 ";

  while (1) {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;

    c = getopt_long(argc, argv, short_options, long_options, &option_index);
    if (c == -1) break;

    switch (c) {
      case 0:
        printf("option %s", long_options[option_index].name);
        if (optarg) printf(" with arg %s", optarg);
        printf("\n");
        break;

      case 'i':
        infile_name = optarg;
        break;

      case 'p':
        line_prefix = optarg;
        break;

      case 'o':
        outfile_name = optarg;
        break;

      case 'h':
        std::cout << usage_txt;
        return EXIT_SUCCESS;
        break;

      case ':':
        return EXIT_FAILURE;
        break;

      case '?':
        return EXIT_FAILURE;
        break;

      default:
        printf("?? getopt returned character code 0%o ??\n", c);
    }
  }

  select_model(model_name);
  std::unique_ptr<std::ifstream> is;
  std::unique_ptr<std::ofstream> os;

  if (!outfile_name) {
    std::cerr << "no file name given for output\n";
    return EXIT_FAILURE;
  }
  if (infile_name) {
    is = std::unique_ptr<std::ifstream>(new std::ifstream(infile_name));
    if (!is && *is) {
      std::cerr << "could not open file for input\n";
      return EXIT_FAILURE;
    }
    std::cin.rdbuf(is->rdbuf());
  }

  if (outfile_name) {
    os = std::unique_ptr<std::ofstream>(
        new std::ofstream(outfile_name, std::ios_base::binary));
    if (!os && *os) {
      std::cerr << "could not open file for output\n";
      return EXIT_FAILURE;
    }
  }

  XR25Frame::voc_t voc{};
  for (std::string line; std::getline(std::cin, line);) {
    if (!line.starts_with(line_prefix)) {
      continue;
    }
    const char* cline = line.c_str() + strlen(line_prefix);

    if (true) {  // validate line
      ++voc.counter;
    }

    hex2voc(voc, cline);
    if (voc.frame_len != 29)  // XXX: hard coded r19-x53b-740
      continue;

    os->put('\xff');
    os->put('\x00');
    for (int i = 0; i < voc.frame_len; ++i) {
      char c = char(voc.frame[i]);
      os->put(c);
      if (c == 0xff) {
        os->put(c);
      }
    }
    continue;
  }
}