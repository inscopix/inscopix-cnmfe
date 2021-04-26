#include "isxCnmfeInitialization.h"
#include "catch.hpp"
#include <string>

TEST_CASE("CnmfeInitializationExtractAC", "[cnmfe-initialization]")
{
    SECTION("identical square matrices")
    {
        const float data[45] = {
            1.03741539f, 57.27598156f, 64.95404632f, 21.85586756f, 38.64771696f, 87.34008431f,
            16.67763465f, 80.373977f, 61.05377408f, 50.18745921f, 89.80071182f, 86.08319232f,
            41.85081805f, 69.44386778f, 90.62772769f, 87.85590855f, 98.33592482f, 49.62817177f,
            49.57732931f, 66.68989729f, 97.08460121f, 24.81011684f, 82.48704645f, 75.82779868f,
            95.09640316f, 16.96362576f, 47.79740135f, 13.3829529f, 54.7837783f, 18.55416869f,
            8.40596512f, 46.56684754f, 34.69019538f, 3.87483758f, 52.10847115f, 43.55554648f,
            14.21110855f, 70.24274241f, 24.31156753f, 34.54986401f, 94.05244095f, 46.3534714f,
            69.91073914f, 94.5517417f, 87.04082964f
        };
        const isx::CubeFloat_t inData(data, 3, 3, 5);

        const isx::MatrixFloat_t exptAi= {
            { 23.77120085f, 16.17992966f, 70.76011498f },
            { 14.86637148f, 46.86288691f, 0.0f },
            { 0.0f, 0.0f, 0.0f }
        };
        const isx::ColumnFloat_t exptCi = { -0.79631757f, -0.1391632f,  0.1391632f, -0.62733245f,  0.38595539f };

        const std::pair<int32_t,int32_t> neuronCenter(1,1);
        isx::MatrixFloat_t outAi;
        isx::ColumnFloat_t outCi;
        isx::extractAC(inData, inData, neuronCenter, outAi, outCi);

        REQUIRE(arma::approx_equal(exptAi, outAi, "reldiff", 1e-5f));
        REQUIRE(arma::approx_equal(exptCi, outCi, "reldiff", 1e-5f));
    }

    SECTION("distinct square matrices")
    {
        const float dataFiltered[80] = {
            96.7029839f, 43.61466469f, 39.69000029f, 90.46675887f, 21.60894956f,
            7.4948587f, 50.25330593f, 86.10143118f, 77.93829218f, 52.79088234f,
            89.55011661f, 73.25934486f, 59.73339439f, 54.52026517f, 18.24454046f,
            54.00747296f, 54.72322492f, 94.89773068f, 62.39161135f, 40.48252173f,
            97.62744548f, 60.07427214f, 68.66670799f, 59.56881171f, 19.76850746f,
            93.7571584f, 36.28535884f, 89.46512923f,  0.89860977f, 52.44040801f,
            91.83830391f, 12.91867837f, 97.268436f, 78.63059859f, 76.74049671f,
            33.11745031f,  0.62302552f, 16.79721837f, 25.36796487f,  8.4661611f,
            86.29932356f, 52.16961224f, 63.7557073f, 51.47339709f, 38.65712826f,
            63.76102441f, 43.18220713f, 61.45628464f, 71.48159937f, 86.62892986f,
            17.897391f, 57.21387703f, 25.29823624f, 73.33801675f, 55.47408629f,
            59.72666073f, 98.34006772f, 10.81933829f, 19.14464021f, 60.35635104f,
            4.41600579f, 40.14954436f, 83.01881021f, 36.3650347f, 69.77288246f,
            17.31654215f, 37.55757686f, 84.54436491f, 43.47915324f, 40.84438602f,
            62.49308441f, 24.54537086f, 16.38422414f, 15.82234072f, 49.77941146f,
            6.50678124f, 95.66529677f, 64.9805109f, 41.67763047f, 76.77580343f
        };
        const isx::CubeFloat_t inDataFiltered(dataFiltered, 4, 4, 5);

        const float dataRaw[80] = {
            22.19931711f, 59.99291966f, 25.90975964f, 63.53562056f, 61.17438629f,
            14.41643007f, 46.94883718f,  9.42027322f,  8.07412688f,  2.43065616f,
            77.08540871f,  5.11428031f, 27.4086462f, 57.76628581f, 25.12451117f,
            35.20949358f, 87.07323062f, 26.58191175f, 80.24968853f, 81.19023912f,
            76.59078565f, 16.56128612f, 98.14687376f, 36.10484185f, 73.84402962f,
            20.45555464f, 48.4931075f, 18.86677358f, 41.42350191f,  0.16421727f,
            96.49152925f, 63.88776821f, 20.67191553f, 28.46858806f, 87.0483087f,
            92.66826152f, 51.84179879f, 96.39305291f, 39.89448039f,  3.55090318f,
            44.13092229f, 69.98436141f,  2.91115637f, 36.54777679f, 29.60799327f,
            51.54726119f, 63.17660527f, 49.34150519f, 91.86109079f, 25.35882058f,
            92.27496139f, 91.26267637f, 29.68005016f, 96.02267153f, 81.37324776f,
            54.63583485f, 15.83098677f, 77.95145856f,  8.65256884f, 24.4290867f,
            62.87879089f, 63.97951761f, 81.66602026f, 58.34997437f, 48.84111888f,
            32.75639477f,  0.22142125f, 82.48107204f, 18.77212287f, 18.84146556f,
            54.6456498f, 79.61427209f, 87.99370312f,  2.29330924f, 11.14538124f,
            79.50874729f, 57.98378102f, 98.56244028f, 56.60819961f, 93.9299352f
        };
        const isx::CubeFloat_t inDataRaw(dataRaw, 4, 4, 5);

        const isx::MatrixFloat_t exptAi= {
            { 0.0f, 27.22893539f, 28.99692663f, 0.0f },
            { 0.0f, 2.05840054f,  3.75731618f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }
        };

        const isx::ColumnFloat_t exptCi = { -0.80642722f, -0.31231246f,  0.0f, -1.28075699f, -0.85284075f };

        const std::pair<int32_t,int32_t> neuronCenter(2,0);
        isx::MatrixFloat_t outAi;
        isx::ColumnFloat_t outCi;
        isx::extractAC(inDataFiltered, inDataRaw, neuronCenter, outAi, outCi);

        REQUIRE(arma::approx_equal(exptAi, outAi, "reldiff", 1e-5f));
        REQUIRE(arma::approx_equal(exptCi, outCi, "reldiff", 1e-5f));
    }

    SECTION("non-square matrices")
    {
        const float dataFiltered[210] = {
            3.44557117e+01f,-9.86893995e+01f,9.15959998e+01f,-7.86753260e+01f,3.63517110e+01f,-1.01753802e+01f,
            1.31234839e+01f,8.41753976e+00f,3.87515799e+01f,-5.04748745e+00f,-5.36477496e+01f,-4.99017641e+01f,
            5.39586056e+01f,6.69837703e+01f,9.25372707e+00f,5.02707423e+01f,-7.14391999e+01f,-9.23191480e+01f,
            -2.09091911e+01f,9.87568356e+00f,-7.88919982e+01f,6.96465371e+01f,6.38913551e+01f,-6.96251511e+01f,
            -5.76626429e+01f,6.74987185e+01f,3.82059757e+01f,1.09727992e+01f,2.55720872e+01f,6.72635147e+01f,
            -5.56845257e+01f,-1.80082928e+01f,-5.34351711e+01f,2.35817799e+01f,-8.33592071e+01f,-3.44207286e+01f,
            -7.11978002e+01f,-5.89226578e+01f,5.40075261e+01f,-5.76284243e+01f,-8.66986449e+01f,8.57039292e+01f,
            -2.38432015e+00f,7.97288374e+01f,-9.68435559e+01f,-1.70117432e+01f,6.82125803e+01f,3.98065252e+01f,
            -4.04755003e+01f,-9.16482937e+01f,6.53117048e+01f,-1.31716400e+01f,-6.92450613e+01f,8.38993641e+01f,
            4.93534202e+01f,-4.94735540e+01f,3.25058166e+00f,2.30078820e+01f,-8.27017956e+01f,3.36233212e+01f,
            9.47912594e+01f,-2.45332426e+01f,2.27760084e+01f,3.22233986e+01f,8.73772066e+01f,2.17250050e+01f,
            1.08691569e+01f,-1.34421330e+01f,-2.90393971e+01f,3.79063207e+01f,3.70041100e+01f,1.92923113e+00f,
            2.89669403e+01f,2.83140202e+01f,8.88348254e+00f,2.01572244e+01f,8.00333120e-02f,1.37685176e+01f,
            -5.75607649e+01f,-4.13869774e+01f,5.80663513e+01f,-4.03097650e+01f,1.66119294e+01f,7.34222483e+01f,
            6.50990348e+01f,1.04468854e+01f,-3.74811085e+01f,-6.90759734e+01f,8.00114670e+01f,-6.82793346e+01f,
            -9.06608559e+01f,4.97536799e+00f,8.36696297e+01f,-3.36593033e+01f,2.61532447e+01f,-6.20199000e+01f,
            -2.45122127e+01f,9.36547026e+01f,4.40835115e+01f,8.04358822e+01f,6.37956974e+01f,-9.71914938e+01f,
            4.88294308e+00f,-5.92543304e+01f,-5.12687392e+01f,3.28730654e+00f,-1.72318545e+01f,6.94512557e+01f,
            -4.15461768e+01f,4.32225904e+01f,-7.93067339e+01f,-5.01725316e+01f,4.26259579e+01f,-7.86652181e+00f,
            -8.09636756e+01f,9.52902644e+01f,-2.90177567e+01f,2.21486696e+01f,8.80205205e+01f,5.63939983e+01f,
            -4.66876911e+00f,-6.79979852e+01f,5.31726781e+00f,9.32141074e+01f,1.66578859e+01f,8.49559271e+01f,
            -9.37107225e+01f,-6.64906745e+01f,-9.02174626e+01f,1.41590330e+00f,-5.84060237e+01f,-6.25428008e+01f,
            9.81254799e+01f,2.80285081e+01f,-6.93763090e+01f,-8.46503232e+01f,-9.26861215e+01f,-2.00580386e+01f,
            -1.17050955e+00f,-6.75237306e+00f,6.45050298e+01f,-7.33727879e+01f,9.26539689e+01f,8.56333251e+01f,
            -8.12773813e+01f,8.30924998e+01f,6.53892923e+01f,5.45161041e+01f,-3.62204434e+01f,-1.27255750e+01f,
            6.32284720e+01f,7.02687981e+01f,-4.02241203e+01f,4.06729706e+01f,4.30011270e+01f,-4.12726637e+01f,
            -1.76673522e+01f,4.10576718e+01f,4.89294504e+01f,4.06186655e+01f,4.68940518e+01f,4.43928298e+01f,
            -8.44772327e+01f,-3.92133592e+01f,-4.87414740e+01f,3.15231288e+01f,3.48729353e+01f,-9.15902110e+01f,
            6.16099927e+01f,8.57756367e+01f,7.49541785e+01f,6.13753228e+01f,8.95109208e+01f,-1.22347959e+01f,
            -9.86348534e+01f,6.01316327e+01f,-5.99427585e+00f,4.48450870e+01f,5.91771801e+01f,6.34940438e+01f,
            8.57896784e+01f,-4.71817660e+01f,-5.29466921e+01f,-5.98681484e+01f,5.22039994e+01f,-7.71604014e+01f,
            6.26616826e+01f,8.78354846e+01f,-6.89415699e+00f,5.52846183e+01f,4.14565808e+01f,-6.92277599e+00f,
            6.56085132e+01f,-8.41906629e+01f,-6.73582247e+01f,-2.01079184e+01f,-5.55990754e+01f,-2.75156666e+01f,
            -8.06269477e+01f,2.73240205e+01f,2.96317641e+01f,-8.46222193e+01f,-6.71051260e+01f,3.57305736e+01f,
            -5.29912437e+01f,4.37012760e+01f,9.90729761e+01f,-6.51837954e+01f,-5.15306853e+01f,-2.12362577e+01f
        };
        const isx::CubeFloat_t inDataFiltered(dataFiltered, 6, 7, 5);

        const float dataRaw[210] = {
            3.27972554e+00f,-3.12736844e+01f,6.24531790e+01f,-4.22885985e+01f,2.51546902e+01f,-3.61644348e+01f,
            6.67793725e+01f,-8.05707040e+01f,-5.81677002e+01f,4.67135226e+01f,-4.67654602e+01f,8.31921089e+01f,
            1.08455182e+01f,-4.48854910e+01f,5.69043700e+01f,4.72982663e+01f,-5.07641575e+01f,-9.97622678e+01f,
            -5.35292676e+01f,3.61355536e+01f,-5.92944759e+01f,-5.99624165e+01f,1.61848802e+01f,2.06995922e+01f,
            3.70612658e+01f,-7.59132680e+01f,-8.39792923e+01f,-2.86804671e+01f,3.43212124e+00f,6.13065018e+01f,
            9.88635802e+01f,-1.15263737e+01f,6.84438824e+01f,-2.75453290e+01f,-7.40292071e+00f,-9.39028753e+00f,
            8.26307115e+01f,2.74578062e+01f,3.89129330e+01f,9.61608917e+00f,5.99322732e+01f,5.10920283e+00f,
            1.41335174e+01f,7.30199633e+01f,3.40520841e+01f,-7.05376775e+01f,-7.98300763e+01f,6.30998434e+01f,
            -3.86067561e+01f,-7.94305014e+01f,3.19849038e+01f,8.27779054e+01f,8.19927600e+01f,-1.85609455e+01f,
            -2.95736092e+01f,3.44983059e+01f,3.74484075e+01f,-5.23071224e+01f,5.60888211e+01f,-5.46436622e+01f,
            -8.32877133e+01f,4.30963847e+00f,-4.01959181e+01f,4.25192947e+01f,8.21324525e+01f,-9.11609176e+00f,
            3.57349484e+00f,2.33427602e-02f,2.14432544e+01f,-5.20525186e+01f,8.90764172e+01f,-7.38944548e+01f,
            4.13307934e+00f,7.55174649e+01f,-2.30124508e+01f,-2.86995972e+01f,2.62014304e+01f,-5.98709555e+01f,
            6.15840302e+01f,-3.50784007e+01f,-3.15086464e+01f,1.21815789e+01f,7.53243317e+01f,-2.58124187e+01f,
            -9.43051547e+01f,6.60555424e+01f,3.03535407e+01f,-7.41319120e+01f,4.21705578e+01f,7.44812842e+01f,
            7.87226159e+01f,4.03014591e+01f,5.92467975e+00f,4.01974988e+01f,1.59236725e+01f,-6.74779424e+01f,
            -6.36215195e+01f,-6.71393752e+01f,3.90156993e+01f,6.47547457e+01f,-9.08980678e+01f,8.61958833e+01f,
            2.07096845e+01f,-9.13206611e+01f,-5.44688214e+01f,6.90617794e+01f,9.85182093e+01f,-3.73042211e+01f,
            -9.03030925e+01f,-7.23980863e+01f,2.61693407e+01f,3.06756283e+01f,9.38621686e+00f,4.61130254e+01f,
            1.57579071e+01f,8.98528258e+01f,1.03331690e+01f,-6.09496799e+01f,2.24725992e+00f,2.62257955e+01f,
            -1.94004339e+01f,-7.64843814e+01f,-1.75218873e+01f,-8.50490625e+01f,-3.94752396e+01f,-1.48500948e+01f,
            -6.56956688e+01f,7.63228985e+00f,-1.50862113e+01f,8.88112101e+01f,7.62659010e+01f,7.81415104e+01f,
            4.43087724e+01f,7.80959738e+01f,4.97040740e+01f,-2.22937456e+01f,-2.80626210e+01f,-6.31817793e+01f,
            5.71203524e+01f,4.02742273e+01f,-6.26696081e-01f,5.21226890e+01f,7.70082377e+01f,4.77037105e+01f,
            4.57985515e+01f,-5.52126793e+01f,-9.03662229e+01f,-2.62706918e+01f,-8.75551921e+01f,-2.38036265e+01f,
            -7.24261525e+01f,-8.94383198e+01f,-2.44116136e+01f,-2.19121598e+01f,-4.71676740e+01f,-9.89123770e+01f,
            4.69638117e+01f,-4.36651664e+00f,4.21076280e+01f,9.74625501e+01f,-5.55238228e+01f,-9.64417483e+01f,
            -2.85551314e+01f,-8.97798007e+01f,-5.20418069e+01f,7.84542018e+01f,2.18543330e+01f,-4.69809301e+01f,
            3.70553963e+01f,8.44938745e+01f,3.13190672e+01f,-2.97131471e+01f,3.90579926e+01f,3.79819805e+01f,
            -6.20122092e+01f,-6.80879398e+01f,-8.12486288e+01f,8.84518578e+01f,7.05268475e+01f,-5.52457579e+01f,
            9.30966445e+01f,-2.47295556e+00f,9.50722254e+01f,-2.10581852e+01f,-4.47157559e+01f,8.34954402e+01f,
            -4.47522343e+01f,1.50410174e+01f,8.07942715e+01f,-1.26039762e+01f,8.56962875e+01f,2.85624024e+01f,
            -6.26065148e+01f,-6.43446155e+01f,-9.73517978e+01f,3.15566036e+01f,2.94426962e+01f,3.01322647e+01f,
            8.39235445e+00f,-7.77613154e+00f,3.50557701e+01f,-9.53389015e+01f,4.68990778e+01f,6.77101332e+01f,
            9.05753429e+01f,2.75317306e+01f,9.37671952e+01f,-7.78754177e+01f,-8.29997833e+01f,-1.07184765e+01f
        };
        const isx::CubeFloat_t inDataRaw(dataRaw, 6, 7, 5);

        const isx::MatrixFloat_t exptAi= {
            { 0.0f, 0.0f, 108.33110235f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 60.44111192f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 24.57989132f, 96.81770054f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
        };

        const isx::ColumnFloat_t exptCi = { -0.44576189f, 0.0f, -0.40483367f, 0.21439196f, 0.70818984f };

        const std::pair<int32_t,int32_t> neuronCenter(3,0);
        isx::MatrixFloat_t outAi;
        isx::ColumnFloat_t outCi;
        isx::extractAC(inDataFiltered, inDataRaw, neuronCenter, outAi, outCi);

        REQUIRE(arma::approx_equal(exptAi, outAi, "reldiff", 1e-5f));
        REQUIRE(arma::approx_equal(exptCi, outCi, "reldiff", 1e-5f));
    }

    SECTION("black (aka zero) input")
    {
        const isx::CubeFloat_t inData = arma::zeros<isx::CubeFloat_t>(3, 3, 5);
        const isx::MatrixFloat_t exptAi= arma::zeros<isx::MatrixFloat_t>(3,3);
        const isx::ColumnFloat_t exptCi = arma::zeros<isx::ColumnFloat_t>(5);
        const std::pair<int32_t,int32_t> neuronCenter(1,1);

        // output matrices initialized to 1 to ensure they get changed to 0
        isx::MatrixFloat_t outAi = arma::ones<isx::MatrixFloat_t>(3,3);

        isx::ColumnFloat_t outCi = arma::ones<isx::ColumnFloat_t>(5);
        isx::extractAC(inData, inData, neuronCenter, outAi, outCi);

        REQUIRE(arma::approx_equal(exptAi, outAi, "reldiff", 1e-5f));
        REQUIRE(arma::approx_equal(exptCi, outCi, "reldiff", 1e-5f));
    }

    SECTION("empty baseline")
    {
        const float data[45] = {
            55.07979026f,59.08628174f,97.59954225f,89.62930889f,28.35250818f,9.22170089f,2.98762109f,
            78.03147645f,40.65199164f,70.81478226f,2.39818824f,67.23836759f,12.55853105f,69.31379183f,
            65.34109025f,45.68332244f,30.63635324f,46.89402487f,29.09047389f,55.8854088f,90.28341085f,
            20.72428781f,44.04537177f,55.78407624f,64.91440476f,22.19578839f,26.92355783f,51.08276052f,
            25.92524469f,84.57508713f,5.14672033f,15.68677385f,36.15647631f,27.84872826f,38.79712576f,
            29.17927742f,89.29469543f,41.5101197f,37.79940413f,44.08098437f,54.4649018f,22.50545048f,
            67.6254902f,93.63836499f,45.76863993f
        };
        const isx::CubeFloat_t inData(data, 3, 3, 5);

        const isx::MatrixFloat_t exptAi= {
            { 23.88554768f, 20.80159779f, 21.7612446f },
            { 0.0f, 42.23204989f, 16.14867269f },
            { 0.0f, 9.63831827f, 15.6324923f }
        };
        const isx::ColumnFloat_t exptCi = { -0.33197918f,  0.63793073f,  0.03960741f, -0.63188729f,  0.28632833f };

        const std::pair<int32_t,int32_t> neuronCenter(1,1);
        isx::MatrixFloat_t outAi;
        isx::ColumnFloat_t outCi;
        isx::extractAC(inData, inData, neuronCenter, outAi, outCi);

        REQUIRE(arma::approx_equal(exptAi, outAi, "reldiff", 1e-5f));
        REQUIRE(arma::approx_equal(exptCi, outCi, "reldiff", 1e-5f));
    }
}
