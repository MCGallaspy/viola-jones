//
// Created by Mike on 5/30/2017.
//
#include "basic_bitmap.h"
#include "haar_feature.h"
#include "integral_image.h"
#include "rect.h"
#include "weak_classifier.h"

#include <sstream>
#include <fstream>
#include <iomanip>
#include <tuple>
#include <chrono>
#include <thread>

#include "benchmark_api.h"


using namespace mcg;

using ii_t = integral_image<bitmap8>;
using data_t = typename weak_classifier<ii_t, 24, 24>::data_t;
using loaded_t = std::tuple<data_t, data_t>;

static constexpr std::size_t w = 12;

static
loaded_t
load_files()
{
    static constexpr auto FILE_MAX = 1520;
    std::vector<std::pair<ii_t, std::array<rect, 2>>> files ;
    for (auto i = 0; i < FILE_MAX; ++i)
    {
        std::stringstream sstr;
        sstr << "../bioid-faces/BioID_"
             << std::setw(4) << std::setfill('0') << i << ".pgm";
        std::ifstream in(sstr.str());
        std::string tmp;
        int width, height, max_pixel_val;
        in >> tmp >> width >> height >> max_pixel_val;
        bitmap8 bm(width, height);
        bm.m_data.resize(width * height);
        for (auto y = 0; y < height; ++y)
        {
            for (auto x = 0; x < width; ++x)
            {
                const auto c = in.get();
                bm.at(x, y) = c;
            }
        }
        in.close();
        sstr.str("");
        sstr << "../bioid-faces/BioID_"
             << std::setw(4) << std::setfill('0') << i << ".eye";
        in.open(sstr.str());
        std::string comment;
        std::getline(in, comment, '\n');
        int lx, ly, rx, ry;
        in >> lx >> ly >> rx >> ry;
        if (lx >= w && ly >= w && rx >= w && ry >= w) {
            rect left_eye {lx - w, ly - w, 2 * w, 2 * w},
                 right_eye{rx - w, ry - w, 2 * w, 2 * w};
            std::array<rect, 2> eyes{
                    std::move(left_eye),
                    std::move(right_eye)
            };
            files.push_back(std::make_pair(ii_t::create(bm), std::move(eyes)));
        }
    }
    data_t pos, neg;
    for (const auto& pair : files)
    {
        for (const auto& eye : pair.second)
        {
            pos.push_back(pair.first.vectorize_window(eye));
        }
        neg.push_back(pair.first.vectorize_window(rect{0, 0, 24, 24}));
    }
    return std::make_tuple(pos, neg);
}

static void train_sorted(benchmark::State& state)
{
    const auto data = load_files();
    haar_feature<ii_t, 24, 24> feature(
            std::vector<rect>{ rect{0, 0, w/2, w} },
    std::vector<rect>{ rect{w/2, 0, w/2, w} }
    );
    weak_classifier<ii_t, 24, 24> wc{std::move(feature)};
    benchmark::DoNotOptimize(wc.threshold);
    while (state.KeepRunning())
        wc.train(std::get<0>(data), std::get<1>(data));
    benchmark::ClobberMemory();
}
BENCHMARK(train_sorted);

static void train_unsorted(benchmark::State& state)
{
    const auto data = load_files();
    haar_feature<ii_t, 24, 24> feature(
            std::vector<rect>{ rect{0, 0, w/2, w} },
    std::vector<rect>{ rect{w/2, 0, w/2, w} }
    );
    weak_classifier<ii_t, 24, 24> wc{std::move(feature)};
    benchmark::DoNotOptimize(wc.threshold);
    while (state.KeepRunning())
        wc.train2(std::get<0>(data), std::get<1>(data));
    benchmark::ClobberMemory();
}
BENCHMARK(train_unsorted);

BENCHMARK_MAIN()