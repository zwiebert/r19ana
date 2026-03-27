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

int main(int argc, char** argv) {
  constexpr const char* usage_txt =
      "Usage: xr25-hex2human -i=FILE [OPTION...]\n"
      "\n"
      "  -i, --infile=FILE\n"
      "  -o, --outfile=FILE\n"
      "  -m, --model=MODEL (x53b-740|raw|exp)\n"
      "\n";

  constexpr struct option long_options[] = {
      {"infile", required_argument, 0, 'i'},
      {"outfile", required_argument, 0, 'o'},
      {"model", required_argument, 0, 'm'},
      {"help", no_argument, 0, 'h'},
      {}};

  constexpr const char* short_options = "i:m:o:h";

  int c;
  int digit_optind = 0;

  const char* infile_name = nullptr;
  const char* outfile_name = nullptr;
  const char* model_name = "73PS";

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

      case 'o':
        outfile_name = optarg;
        break;

      case 'm':
        model_name = optarg;
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

  if (!infile_name) {
    std::cerr << usage_txt;
    return EXIT_FAILURE;
  }

  select_model(model_name);
  std::unique_ptr<std::ifstream> is;
  std::unique_ptr<std::ofstream> os;

  if (infile_name) {
    is = std::unique_ptr<std::ifstream>(new std::ifstream(infile_name, std::ios_base::binary));
    if (!is && *is) {
      std::cerr << "could not open file for input\n";
      return EXIT_FAILURE;
    }
    std::cin.rdbuf(is->rdbuf());
  }

  if (outfile_name) {
    os = std::unique_ptr<std::ofstream>(new std::ofstream(outfile_name));
    if (!os && *os) {
      std::cerr << "could not open file for output\n";
      return EXIT_FAILURE;
    }
    std::cout.rdbuf(os->rdbuf());
  }

  FrameProcessor processor([](const XR25Frame::voc_t& frame) {
    print_car_diag->push_frame(frame);
    char* dst = 0;
    if (auto dst_len = r19_alloc_and_print(dst, *print_car_diag, Mask);
        dst_len > 0) {
      if (term_transport.write((const uint8_t*)dst, dst_len, true)) {
        free(dst);
        return;
      }
      free(dst);
    }
  });

  for (uint8_t buf[64];;) {
    if (auto len = is->read((char*)buf, sizeof buf).gcount(); len > 0) {
      processor.feedBytes(buf, len, true);
    } else {
      break;
    }
  }
}