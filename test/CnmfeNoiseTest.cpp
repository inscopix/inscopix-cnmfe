#include "isxCnmfeNoise.h"
#include "isxTest.h"
#include "catch.hpp"

TEST_CASE("CNMFeNoiseAveragePSD", "[cnmfe-noise]")
{
    const float data[60] = {
        // column-major input matrix
        1.03741539f, 38.64771696f, 61.05377408f,
        21.85586756f, 80.373977f, 18.73586471f,
        16.67763465f, 64.95404632f, 64.71167135f,
        57.27598156f, 87.34008431f, 16.95314142f,

        50.18745921f, 69.44386778f, 49.62817177f,
        41.85081805f, 98.33592482f, 68.76740932f,
        87.85590855f, 86.08319232f, 63.80539847f,
        89.80071182f, 90.62772769f, 79.4547887f,

        49.57732931f, 82.48704645f, 47.79740135f,
        24.81011684f, 16.96362576f, 70.35291738f,
        95.09640316f, 97.08460121f, 46.16976625f,
        66.68989729f, 75.82779868f, 16.26060466f,

        13.3829529f, 46.56684754f, 43.55554648f,
        8.40596512f, 52.10847115f, 92.52000773f,
        3.87483758f, 18.55416869f, 5.30594245f,
        54.7837783f, 34.69019538f, 13.87386998f,

        14.21110855f, 94.05244095f, 87.04082964f,
        34.54986401f, 94.5517417f, 3.94548177f,
        69.91073914f, 24.31156753f, 3.85709232f,
        70.24274241f, 46.3534714f, 35.29169532f
    };
    const isx::CubeFloat_t inputCube(data, 3, 4, 5);

    const isx::ColumnFloat_t inputColumn = {
        41.85081805f, 98.33592482f, 68.76740932f,
        3.87483758f, 18.55416869f, 5.30594245f,
        69.91073914f, 24.31156753f, 3.85709232f,
        16.67763465f, 64.95404632f, 64.71167135f,
    };

    SECTION("cube input - mean method")
    {
        const isx::MatrixFloat_t expResult = {
            { 3.58324246f, 3.62591549f, 5.22891502f, 5.82059371f},
            { 5.75497975f, 5.85092933f, 5.39432643f, 5.78652985f},
            { 5.37657626f, 5.04303164f, 4.28777181f, 4.02286092f }
        };

        isx::MatrixFloat_t actResult;
        isx::averagePSD(inputCube, actResult, isx::AveragingMethod_t::MEAN);
        REQUIRE(arma::approx_equal(expResult, actResult, "reldiff", 1e-5f));
    }

    SECTION("cube input - median method")
    {
        const isx::MatrixFloat_t expResult = {
            { 2.66562456f, 3.52208155f, 5.91230662f, 5.77450852f},
            { 5.89253204f, 6.33932082f, 5.69886157f, 6.15742636f},
            { 4.9813739f, 5.86376199f, 4.80467305f, 2.91145508f }
        };

        isx::MatrixFloat_t actResult;
        isx::averagePSD(inputCube, actResult, isx::AveragingMethod_t::MEDIAN);
        REQUIRE(arma::approx_equal(expResult, actResult, "reldiff", 1e-5f));
    }

    SECTION("cube input - exponential method")
    {
        const isx::MatrixFloat_t expResult = {
            { 2.62171872f, 3.39921376f, 4.04735868f, 5.77486163f},
            { 5.59943288f, 5.38861832f, 4.87952942f, 5.59685709f},
            { 5.28988412f, 3.99440801f, 3.22557416f, 3.56880412f }
        };

        isx::MatrixFloat_t actResult;
        isx::averagePSD(inputCube, actResult, isx::AveragingMethod_t::LOGMEXP);
        REQUIRE(arma::approx_equal(expResult, actResult, "reldiff", 1e-5f));
    }

    SECTION("column input - mean method")
    {
        const float expResult = 4.477312494771091f;
        float actResult = isx::averagePSD(inputColumn,isx::AveragingMethod_t::MEAN);
        REQUIRE(fabs(expResult - actResult) < 1e-5);
    }

    SECTION("column input - median method")
    {
        const float expResult = 4.0670132031996165f;
        float actResult = isx::averagePSD(inputColumn,isx::AveragingMethod_t::MEDIAN);
        REQUIRE(fabs(expResult - actResult) < 1e-5);
    }

    SECTION("column input - exponential method")
    {
        const float expResult = 3.5086080681027596f;
        float actResult = isx::averagePSD(inputColumn,isx::AveragingMethod_t::LOGMEXP);
        REQUIRE(fabs(expResult - actResult) < 1e-5f);
    }
}

TEST_CASE("CNMFeNoiseFFT", "[cnmfe-noise]")
{
    const float data[210] = {
        18.02696889f, 40.16764806f, 73.46677209f, 64.8056442f, 51.90077416f,
        95.6058264f,  7.83798077f,  1.27808146f, 98.66733304f, 65.26147399f,
        31.88702398f, 44.77711604f, 64.80464732f, 17.59034817f, 89.39041703f,
        7.00228026f, 94.98333057f, 84.49385309f, 55.65211059f, 98.90811616f,
        67.34901437f, 31.63673116f, 79.26213851f,  1.93433623f, 34.3713307f,
        42.03907748f, 47.82566402f,  2.77326608f, 11.1661232f, 73.28253363f,
        36.76176251f, 62.69978192f, 71.29629202f, 68.1559043f, 60.37265474f,
        1.94752415f, 84.79789975f, 93.29046272f, 69.60661748f, 92.24502383f,
        96.59308967f, 95.21754803f, 48.73716073f, 33.80540496f, 81.0966574f,
        56.82896689f, 55.0639471f,  5.29328491f, 96.13877718f, 85.71542471f,
        31.93677071f, 33.43641542f, 30.21646878f, 36.6427211f, 24.77996425f,
        24.84196822f, 15.79123067f, 56.18187101f, 75.96494676f, 70.93010178f,
        95.99626473f, 91.55030041f, 42.26599746f,  8.39531433f,  6.28876239f,
        16.90288106f, 82.56522291f, 52.15734914f, 94.79500298f, 59.96501042f,
        46.32185265f, 71.78491793f, 40.03284329f, 19.30180803f, 72.39112626f,
        35.77892413f, 70.61476278f, 94.18066523f, 23.98746791f, 64.22148587f,
        65.86923514f, 97.11752036f, 23.28828326f, 29.10379758f, 16.50866176f,
        7.03825951f, 61.16319967f, 43.82206709f, 91.17954843f,  5.41153045f,
        75.18671692f, 75.89795882f, 61.60183851f, 45.27591645f, 97.39682558f,
        59.54866905f, 35.58300799f,  6.54465827f, 71.27259357f,  2.07329789f,
        79.38724066f, 53.36939819f, 50.54731686f, 36.05798784f, 90.2647429f,
        72.49339292f, 60.20640512f, 35.84379283f, 76.76322349f, 49.39130011f,
        4.58702052f, 91.15328468f, 85.07950894f, 79.6435754f, 95.74440528f,
        57.55585411f, 93.24137189f, 70.58842586f, 79.0613526f, 63.23340138f,
        29.02636671f, 36.56701676f,  9.14145869f, 31.36791531f, 97.96933083f,
        87.92560631f, 81.82753579f, 36.14835422f, 62.28340077f, 62.52248401f,
        93.21486217f,  5.31798288f, 57.46559438f, 59.95433963f, 77.05480664f,
        83.30374086f, 91.11216648f, 13.64845133f, 24.69981527f, 53.10294059f,
        42.02036046f, 55.23838215f, 80.65667909f, 33.40828943f, 49.74963527f,
        55.39272728f, 81.44408709f, 72.99644702f,  6.36864308f, 33.38744215f,
        31.8187194f, 84.06854304f,  5.50848321f, 17.07901478f,  2.04836128f,
        79.01011234f, 46.15402914f, 30.09791698f, 93.07659225f, 76.72162846f,
        32.03724497f, 34.4624491f, 16.88172986f, 73.99515178f, 26.93035402f,
        53.88502919f, 61.39703292f, 68.79362469f,  5.56736796f, 29.99520094f,
        73.42129873f, 41.55036692f, 14.49194619f,  5.29518681f, 86.13926874f,
        48.54270982f, 94.91023977f, 76.44910965f, 43.79592061f, 64.87221983f,
        73.63094718f, 19.07216129f, 10.87360719f, 36.4615643f, 73.82526228f,
        21.80033753f, 12.70482611f, 84.64969771f, 29.90902303f, 11.67372688f,
        90.54003208f,  7.50019931f,  8.5694806f, 23.83461012f, 45.41978001f,
        29.87078764f, 31.87987969f, 43.62409337f, 41.86634302f, 65.67088062f,
        22.80368662f, 31.25447593f, 56.83058309f, 47.97972817f, 70.1164275f,
        87.529458f, 28.60446695f, 92.96051144f, 83.16599395f, 35.19715939f };
    const isx::CubeFloat_t inputCube(data, 7, 5, 6);

    const isx::ColumnFloat_t inputColumn = {
        37.50580701f, 67.38259067f,  7.39609437f, 10.79657594f, 14.7616523f,
        85.87640815f, 99.42567977f, 78.47982413f, 16.91985423f,  8.04452239f,
        96.29836385f, 16.87789913f, 87.43096959f, 39.83124574f, 79.74232168f,
        86.33517735f, 27.60875922f, 71.45010166f, 79.26162762f, 67.05656936f,
        41.22293777f, 94.93376938f, 18.71927235f
    };

    SECTION("cube input - exponential noise averaging")
    {
        const isx::MatrixFloat_t expResult = {
            { 11.63286577f, 11.01868931f, 30.76380325f, 11.23244967f, 20.75965109f},
            { 26.46888627f, 21.20759829f, 25.96616569f, 11.79174554f, 28.45843948f},
            { 10.810745f, 24.28069513f, 34.79205833f, 27.47540166f, 4.52168165f},
            { 28.68622891f, 1.30188468f, 26.42085985f, 21.03867101f, 21.57339259f},
            { 14.44399734f, 17.6477328f, 30.44902167f, 24.08048416f, 16.12364903f},
            { 9.76943745f, 35.56412776f, 16.59970533f, 16.14188227f, 19.14065606f},
            { 25.0579138f, 33.66449345f, 21.29724754f, 25.39608315f, 20.53558766f}
        };

        isx::MatrixFloat_t actResult;
        std::pair<float,float> noiseRange(0.25f, 0.5f);
        isx::getNoiseFft(inputCube, actResult, noiseRange, isx::AveragingMethod_t::LOGMEXP);

        REQUIRE(arma::approx_equal(expResult, actResult, "reldiff", 1e-5f));
    }

    SECTION("cube input - mean noise averaging with upper limit on num samples")
    {
        const isx::MatrixFloat_t expResult = {
            { 22.91625033f, 31.23855614f, 36.35501514f, 26.44625334f, 38.13866646f},
            { 16.27590723f, 12.84257551f, 34.21835172f, 19.04637745f, 28.46288475f},
            { 24.4642721f, 19.11674708f, 19.87729745f, 22.25305088f, 33.77691739f},
            { 20.927439f, 17.91834578f, 12.7746344f, 29.98853717f, 22.55949132f},
            { 15.34917475f, 33.21493275f, 31.49675401f, 30.99134435f, 33.77276222f},
            { 24.85698256f, 40.62128959f, 29.29606206f, 32.29330793f, 36.39279347f},
            { 36.07448606f, 33.89187465f, 23.5224712f, 26.23497049f, 22.75862f }
        };

        isx::MatrixFloat_t actResult;
        std::pair<float,float> noiseRange(0.17f, 0.61f);
        int maxSamplesFft = 4;
        isx::getNoiseFft(
            inputCube,
            actResult,
            noiseRange,
            isx::AveragingMethod_t::MEAN,
            maxSamplesFft);

        REQUIRE(arma::approx_equal(expResult, actResult, "reldiff", 1e-5f));
    }

    SECTION("cube input - median noise averaging from full frequency range")
    {
        const isx::MatrixFloat_t expResult = {
            { 19.95093362f, 12.51408789f, 41.17904319f, 24.72674436f, 25.80427224f},
            { 23.59780525f, 16.76945647f, 28.25452779f, 15.62956617f, 33.29640353f},
            { 25.05255112f, 16.4483943f, 23.75920891f, 25.00373411f, 27.30383791f},
            { 27.74513272f,  2.69859469f, 26.70046151f, 26.82024083f, 24.68742867f},
            { 15.74012423f, 26.51696523f, 15.82950998f, 25.12086547f, 28.38954095f},
            { 19.5366673f, 27.32405592f, 32.57394148f, 25.70060859f, 38.02903585f},
            { 33.76864603f, 29.03083662f, 23.39210193f, 32.02733391f, 17.02165862f }
        };

        isx::MatrixFloat_t actResult;
        std::pair<float,float> noiseRange(0.0f, 1.0f);
        int maxSamplesFft = 6;
        isx::getNoiseFft(
            inputCube,
            actResult,
            noiseRange,
            isx::AveragingMethod_t::MEDIAN,
            maxSamplesFft);

        REQUIRE(arma::approx_equal(expResult, actResult, "reldiff", 1e-5f));
    }

    SECTION("column input - exponential noise averaging")
    {
        const float expResult = 26.443470429738795f;

        std::pair<float,float> noiseRange(0.25f, 0.5f);
        float actResult = isx::getNoiseFft(inputColumn, noiseRange, isx::AveragingMethod_t::LOGMEXP);

        REQUIRE(fabs(expResult - actResult) < 1e-5f);
    }

    SECTION("column input - mean noise averaging with upper limit on num samples")
    {
        const float expResult = 34.640300294445645f;

        std::pair<float,float> noiseRange(0.14f, 0.49f);
        uint32_t maxSamplesFft = 12;
        float actResult = isx::getNoiseFft(
            inputColumn,
            noiseRange,
            isx::AveragingMethod_t::MEAN,
            maxSamplesFft);

        REQUIRE(fabs(expResult - actResult) < 1e-5f);
    }

    SECTION("column input - median noise averaging from full frequency range")
    {
        const float expResult = 31.35062072913993f;

        std::pair<float,float> noiseRange(0.0f, 1.0f);
        int maxSamplesFft = 18;
        float actResult = isx::getNoiseFft(
            inputColumn,
            noiseRange,
            isx::AveragingMethod_t::MEDIAN,
            maxSamplesFft);

        REQUIRE(fabs(expResult - actResult) < 1e-5f);
    }
}
