/**
 * This is a copy of the mlpack implementation of the Lasso Lars algorithm
 * The LARS class has been modified so that it is templated, such that it can accept either doubles or floats.
 * The original implementation of this class restricted the input data type to doubles
 * The CNMFE algorithm uses Lasso Lars to update spatial components.
 * To improve performance of CNMFe, the main data type was switched from doubles to floats,
 * which is why we templated LassoLars to fit the performance needs of CNMFE
*/

/**
Format: http://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: mlpack
Upstream-Contact: Ryan Curtin <ryan@ratml.org>
Source:
  http://www.mlpack.org/
  git://github.com/mlpack/mlpack.git

Files: *
Copyright:
  Copyright 2008-2018, Ryan Curtin <ryan@ratml.org>
  Copyright 2008-2013, Bill March <march@gatech.edu>
  Copyright 2008-2012, Dongryeol Lee <dongryel@cc.gatech.edu>
  Copyright 2008-2013, Nishant Mehta <niche@cc.gatech.edu>
  Copyright 2008-2013, Parikshit Ram <p.ram@gatech.edu>
  Copyright 2010-2012, James Cline <james.cline@gatech.edu>
  Copyright 2010-2013, Sterling Peet <sterling.peet@gatech.edu>
  Copyright 2011-2012, Matthew Amidon <mamidon@gatech.edu>
  Copyright 2011-2012, Neil Slagle <npslagle@gmail.com>
  Copyright 2011, Ajinkya Kale <kaleajinkya@gmail.com>
  Copyright 2011, Vlad Grantcharov <vlad321@gatech.edu>
  Copyright 2011, Noah Kauffman <notoriousnoah@gmail.com>
  Copyright 2012, Rajendran Mohan <rmohan88@gatech.edu>
  Copyright 2012, Trironk Kiatkungwanglai <trironk@gmail.com>
  Copyright 2012, Patrick Mason <patrick.s.mason@gmail.com>
  Copyright 2013-2018, Marcus Edel <marcus.edel@fu-berlin.de>
  Copyright 2013, Mudit Raj Gupta <mudit.raaj.gupta@gmail.com>
  Copyright 2013-2018, Sumedh Ghaisas <sumedhghaisas@gmail.com>
  Copyright 2014, Michael Fox <michaelfox99@gmail.com>
  Copyright 2014, Ryan Birmingham <birm@gatech.edu>
  Copyright 2014, Siddharth Agrawal <siddharth.950@gmail.com>
  Copyright 2014, Saheb Motiani <saheb210692@gmail.com>
  Copyright 2014, Yash Vadalia <yashdv@gmail.com>
  Copyright 2014, Abhishek Laddha <laddhaabhishek11@gmail.com>
  Copyright 2014, Vahab Akbarzadeh <v.akbarzadeh@gmail.com>
  Copyright 2014, Andrew Wells <andrewmw94@gmail.com>
  Copyright 2014, Zhihao Lou <lzh1984@gmail.com>
  Copyright 2014, Udit Saxena <saxenda.udit@gmail.com>
  Copyright 2014-2015, Stephen Tu <tu.stephenl@gmail.com>
  Copyright 2014-2015, Jaskaran Singh <jaskaranvirdi@ymail.com>
  Copyright 2015&2017, Shangtong Zhang <zhangshangtong.cpp@gmail.com>
  Copyright 2015, Hritik Jain <hritik.jain.cse13@itbhu.ac.in>
  Copyright 2015, Vladimir Glazachev <glazachev.vladimir@gmail.com>
  Copyright 2015, QiaoAn Chen <kazenoyumechen@gmail.com>
  Copyright 2015, Janzen Brewer <jahabrewer@gmail.com>
  Copyright 2015, Trung Dinh <dinhanhtrung@gmail.com>
  Copyright 2015-2017, Tham Ngap Wei <thamngapwei@gmail.com>
  Copyright 2015, Grzegorz Krajewski <krajekg@gmail.com>
  Copyright 2015, Joseph Mariadassou <joe.mariadassou@gmail.com>
  Copyright 2015, Pavel Zhigulin <pashaworking@gmail.com>
  Copyright 2016, Andy Fang <AndyFang.DZ@gmail.com>
  Copyright 2016-2018, Barak Pearlmutter <barak+git@pearlmutter.net>
  Copyright 2016, Ivari Horm <ivari@risk.ee>
  Copyright 2016, Dhawal Arora <d.p.arora1@gmail.com>
  Copyright 2016, Alexander Leinoff <alexander-leinoff@uiowa.edu>
  Copyright 2016, Palash Ahuja <abhor902@gmail.com>
  Copyright 2016, Yannis Mentekidis <mentekid@gmail.com>
  Copyright 2016, Ranjan Mondal <ranjan.rev@gmail.com>
  Copyright 2016-2018, Mikhail Lozhnikov <lozhnikovma@gmail.com>
  Copyright 2016, Marcos Pividori <marcos.pividori@gmail.com>
  Copyright 2016, Keon Kim <kwk236@gmail.com>
  Copyright 2016, Nilay Jain <nilayjain13@gmail.com>
  Copyright 2016, Peter Lehner <peter.lehner@dlr.de>
  Copyright 2016, Anuraj Kanodia <akanuraj200@gmail.com>
  Copyright 2016, Ivan Georgiev <ivan@jonan.info>
  Copyright 2016, Shikhar Bhardwaj <shikharbhardwaj68@gmail.com>
  Copyright 2016, Yashu Seth <yashuseth2503@gmail.com>
  Copyright 2016, Mike Izbicki <mike@izbicki.me>
  Copyright 2017, Sudhanshu Ranjan <sranjan.sud@gmail.com>
  Copyright 2017, Piyush Jaiswal <piyush.jaiswal@st.niituniversity.in>
  Copyright 2017, Dinesh Raj <dinu.iota@gmail.com>
  Copyright 2017, Vivek Pal <vivekpal.dtu@gmail.com>
  Copyright 2017, Prasanna Patil <prasannapatil08@gmail.com>
  Copyright 2017, Lakshya Agrawal <zeeshan.lakshya@gmail.com>
  Copyright 2017, Praveen Ch <chvsp972911@gmail.com>
  Copyright 2017, Kirill Mishchenko <ki.mishchenko@gmail.com>
  Copyright 2017, Abhinav Moudgil <abhinavmoudgil95@gmail.com>
  Copyright 2017, Thyrix Yang <thyrixyang@gmail.com>
  Copyright 2017, Sagar B Hathwar <sagarbhathwar@gmail.com>
  Copyright 2017, Nishanth Hegde <hegde.nishanth@gmail.com>
  Copyright 2017, Parminder Singh <parmsingh101@gmail.com>
  Copyright 2017, CodeAi <benjamin.bales@assrc.us>
  Copyright 2017, Franciszek Stokowacki <franek.stokowacki@gmail.com>
  Copyright 2017, Samikshya Chand <samikshya289@gmail.com>
  Copyright 2017, N Rajiv Vaidyanathan <rajivvaidyanathan4@gmail.com>
  Copyright 2017, Kartik Nighania <kartiknighania@gmail.com>
  Copyright 2017-2018, Eugene Freyman <evg.freyman@gmail.com>
  Copyright 2017-2018, Manish Kumar <manish887kr@gmail.com>
  Copyright 2017-2018, Haritha Sreedharan Nair <haritha1313@gmail.com>
  Copyright 2017-2018, Sourabh Varshney <sourabhvarshney111@gmail.com>
  Copyright 2018, Projyal Dev <projyal@gmail.com>
  Copyright 2018, Nikhil Goel <nikhilgoel199797@gmail.com>
  Copyright 2018, Shikhar Jaiswal <jaiswalshikhar87@gmail.com>
  Copyright 2018, B Kartheek Reddy <bkartheekreddy@gmail.com>
  Copyright 2018, Atharva Khandait <akhandait45@gmail.com>
  Copyright 2018, Wenhao Huang <wenhao.huang.work@gmail.com>
  Copyright 2018-2019, Roberto Hueso <robertohueso96@gmail.com>
  Copyright 2018, Prabhat Sharma <prabhatsharma7298@gmail.com>
  Copyright 2018, Tan Jun An <yamidarkxxx@gmail.com>
  Copyright 2018, Moksh Jain <mokshjn00@gmail.com>
  Copyright 2018, Manthan-R-Sheth <manthanrsheth96@gmail.com>
  Copyright 2018, Namrata Mukhija <namratamukhija@gmail.com>
  Copyright 2018, Conrad Sanderson
  Copyright 2018, Thanasis Mattas <mattasa@auth.gr>
  Copyright 2018, Shashank Shekhar <contactshashankshekhar@gmail.com>
  Copyright 2018, Yasmine Dumouchel <yasmine.dumouchel@gmail.com>
  Copyright 2018, German Lancioni
  Copyright 2018, Arash Abghari <arash.abghari@gmail.com>
  Copyright 2018, Ayush Chamoli
  Copyright 2018, Tommi Laivamaa <tommi.laivamaa@protonmail.com>
  Copyright 2019, Kim SangYeon <sy0814k@gmail.com>
  Copyright 2019, Niteya Shah <niteya.56@gmail.com>
  Copyright 2019, Toshal Agrawal <tagrawal1339@gmail.com>
  Copyright 2019, Dan Timson
  Copyright 2019, Miguel Canteras <mcanteras@gmail.com>
  Copyright 2019, Bishwa Karki <karkeebishwa1@gmail.com>
  Copyright 2019, Mehul Kumar Nirala <mehulkumarnirala@gmail.com>
  Copyright 2019, Yashwant Singh Parihar <yashwantsingh.sngh@gmail.com>
  Copyright 2019, Heet Sankesara <heetsankesara3@gmail.com>
  Copyright 2019, Jeffin Sam <sam.jeffin@gmail.com>
  Copyright 2019, Vikas S Shetty <shettyvikas209@gmail.com>
  Copyright 2019, Khizir Siddiqui <khizirsiddiqui@gmail.com>
  Copyright 2019, Tejasvi Tomar <tstomar@outlook.com>
  Copyright 2019, Jai Agarwal <jai.bhageria@gmail.com>
  Copyright 2019, Ziyang Jiang <zij004@alumni.stanford.edu>
  Copyright 2019, Rohit Kartik <rohit.audrey@gmail.com>
  Copyright 2019, Aditya Viki <adityaviki01@gmail.com>
  Copyright 2019, Kartik Dutt <kartikdutt@live.in>
  Copyright 2020, Sriram S K <sriramsk1999@gmail.com>
  Copyright 2020, Manoranjan Kumar Bharti ( Nakul Bharti ) <knakul853@gmail.com>
  Copyright 2020, Saraansh Tandon <saraanshtandon1999@gmail.com>
  Copyright 2020, Gaurav Singh <gs8763076@gmail.com>
  Copyright 2020, Lakshya Ojha <ojhalakshya@gmail.com>
  Copyright 2020, Bisakh Mondal <bisakhmondal00@gmail.com>
  Copyright 2020, Benson Muite <benson_muite@emailplus.org>
  Copyright 2020, Sarthak Bhardwaj <7sarthakbhardwaj@gmail.com>
  Copyright 2020, Aakash Kaushik <kaushikaakash7539@gmail.com>
  Copyright 2020, Anush Kini <anushkini@gmail.com>

License: BSD-3-clause
  All rights reserved.
  .
  Redistribution and use of mlpack in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  .
  1. Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
  .
  2. Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
  .
  3. Neither the name of the copyright holder nor the names of its contributors
  may be used to endorse or promote products derived from this software without
  specific prior written permission.
  .
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ISX_LASSO_LARS_HPP
#define ISX_LASSO_LARS_HPP

#include "isxArmaUtils.h"

namespace isx
{

/**
 * @file methods/lars/lars.hpp
 * @author Nishant Mehta (niche)
 *
 * Definition of the LARS class, which performs Least Angle Regression and the
 * LASSO.
 *
 * Only minor modifications of LARS are necessary to handle the constrained
 * version of the problem:
 *
 * \f[
 * \min_{\beta} 0.5 || X \beta - y ||_2^2 + 0.5 \lambda_2 || \beta ||_2^2
 * \f]
 * subject to \f$ ||\beta||_1 <= \tau \f$
 *
 * Although this option currently is not implemented, it will be implemented
 * very soon.
 */

// beta is the estimator
// yHat is the prediction from the current estimator

/**
 * An implementation of LARS, a stage-wise homotopy-based algorithm for
 * l1-regularized linear regression (LASSO) and l1+l2 regularized linear
 * regression (Elastic Net).
 *
 * Let \f$ X \f$ be a matrix where each row is a point and each column is a
 * dimension and let \f$ y \f$ be a vector of responses.
 *
 * The Elastic Net problem is to solve
 *
 * \f[ \min_{\beta} 0.5 || X \beta - y ||_2^2 + \lambda_1 || \beta ||_1 +
 *     0.5 \lambda_2 || \beta ||_2^2 \f]
 *
 * where \f$ \beta \f$ is the vector of regression coefficients.
 *
 * If \f$ \lambda_1 > 0 \f$ and \f$ \lambda_2 = 0 \f$, the problem is the LASSO.
 * If \f$ \lambda_1 > 0 \f$ and \f$ \lambda_2 > 0 \f$, the problem is the
 *   elastic net.
 * If \f$ \lambda_1 = 0 \f$ and \f$ \lambda_2 > 0 \f$, the problem is ridge
 *   regression.
 * If \f$ \lambda_1 = 0 \f$ and \f$ \lambda_2 = 0 \f$, the problem is
 *   unregularized linear regression.
 *
 * Note: This algorithm is not recommended for use (in terms of efficiency)
 * when \f$ \lambda_1 \f$ = 0.
 *
 * For more details, see the following papers:
 *
 * @code
 * @article{efron2004least,
 *   title={Least angle regression},
 *   author={Efron, B. and Hastie, T. and Johnstone, I. and Tibshirani, R.},
 *   journal={The Annals of statistics},
 *   volume={32},
 *   number={2},
 *   pages={407--499},
 *   year={2004},
 *   publisher={Institute of Mathematical Statistics}
 * }
 * @endcode
 *
 * @code
 * @article{zou2005regularization,
 *   title={Regularization and variable selection via the elastic net},
 *   author={Zou, H. and Hastie, T.},
 *   journal={Journal of the Royal Statistical Society Series B},
 *   volume={67},
 *   number={2},
 *   pages={301--320},
 *   year={2005},
 *   publisher={Royal Statistical Society}
 * }
 * @endcode
 */

        template <typename T>
        class LARS
        {
        public:
            /**
   * Set the parameters to LARS.  Both lambda1 and lambda2 default to 0.
   *
   * @param useCholesky Whether or not to use Cholesky decomposition when
   *    solving linear system (as opposed to using the full Gram matrix).
   * @param lambda1 Regularization parameter for l1-norm penalty.
   * @param lambda2 Regularization parameter for l2-norm penalty.
   * @param tolerance Run until the maximum correlation of elements in (X^T y)
   *     is less than this.
   */
            LARS(const bool useCholesky,
                 const T lambda1,
                 const T lambda2,
                 const T tolerance);

            /**
   * Set the parameters to LARS, and pass in a precalculated Gram matrix.  Both
   * lambda1 and lambda2 default to 0.
   *
   * @param useCholesky Whether or not to use Cholesky decomposition when
   *    solving linear system (as opposed to using the full Gram matrix).
   * @param gramMatrix Gram matrix.
   * @param lambda1 Regularization parameter for l1-norm penalty.
   * @param lambda2 Regularization parameter for l2-norm penalty.
   * @param tolerance Run until the maximum correlation of elements in (X^T y)
   *     is less than this.
   */
            LARS(const bool useCholesky,
                 const arma::Mat<T> &gramMatrix,
                 const T lambda1,
                 const T lambda2,
                 const T tolerance);

            /**
   * Set the parameters to LARS and run training. Both lambda1 and lambda2
   * are set by default to 0.
   *
   * @param data Input data.
   * @param responses A vector of targets.
   * @param transposeData Should be true if the input data is column-major and
   *     false otherwise.
   * @param useCholesky Whether or not to use Cholesky decomposition when
   *     solving linear system (as opposed to using the full Gram matrix).
   * @param lambda1 Regularization parameter for l1-norm penalty.
   * @param lambda2 Regularization parameter for l2-norm penalty.
   * @param tolerance Run until the maximum correlation of elements in (X^T y)
   *     is less than this.
   */
            LARS(const arma::Mat<T> &data,
                 const arma::Row<T> &responses,
                 const bool transposeData,
                 const bool useCholesky,
                 const T lambda1,
                 const T lambda2,
                 const T tolerance);

            /**
   * Set the parameters to LARS, pass in a precalculated Gram matrix, and run
   * training. Both lambda1 and lambda2 are set by default to 0.
   *
   * @param data Input data.
   * @param responses A vector of targets.
   * @param transposeData Should be true if the input data is column-major and
   *     false otherwise.
   * @param useCholesky Whether or not to use Cholesky decomposition when
   *     solving linear system (as opposed to using the full Gram matrix).
   * @param gramMatrix Gram matrix.
   * @param lambda1 Regularization parameter for l1-norm penalty.
   * @param lambda2 Regularization parameter for l2-norm penalty.
   * @param tolerance Run until the maximum correlation of elements in (X^T y)
   *     is less than this.
   */
            LARS(const arma::Mat<T> &data,
                 const arma::Row<T> &responses,
                 const bool transposeData,
                 const bool useCholesky,
                 const arma::Mat<T> &gramMatrix,
                 const T lambda1,
                 const T lambda2,
                 const T tolerance);

            /**
   * Construct the LARS object by copying the given LARS object.
   *
   * @param other LARS object to copy.
   */
            LARS(const LARS &other);

            /**
   * Construct the LARS object by taking ownership of the given LARS object.
   *
   * @param other LARS object to take ownership of.
   */
            LARS(LARS &&other);

            /**
   * Copy the given LARS object.
   *
   * @param other LARS object to copy.
   */
            LARS &operator=(const LARS &other);

            /**
   * Take ownership of the given LARS object.
   *
   * @param other LARS object to take ownership of.
   */
            LARS &operator=(LARS &&other);

            /**
   * Run LARS.  The input matrix (like all mlpack matrices) should be
   * column-major -- each column is an observation and each row is a dimension.
   * However, because LARS is more efficient on a row-major matrix, this method
   * will (internally) transpose the matrix.  If this transposition is not
   * necessary (i.e., you want to pass in a row-major matrix), pass 'false' for
   * the transposeData parameter.
   *
   * @param data Column-major input data (or row-major input data if rowMajor =
   *     true).
   * @param responses A vector of targets.
   * @param beta Vector to store the solution (the coefficients) in.
   * @param transposeData Set to false if the data is row-major.
   * @return minimum cost error(||y-beta*X||2 is used to calculate error).
   */
            T Train(const arma::Mat<T> &data,
                    const arma::Row<T> &responses,
                    arma::Col<T> &beta,
                    const bool transposeData = true);

            /**
   * Run LARS.  The input matrix (like all mlpack matrices) should be
   * column-major -- each column is an observation and each row is a dimension.
   * However, because LARS is more efficient on a row-major matrix, this method
   * will (internally) transpose the matrix.  If this transposition is not
   * necessary (i.e., you want to pass in a row-major matrix), pass 'false' for
   * the transposeData parameter.
   *
   * @param data Input data.
   * @param responses A vector of targets.
   * @param transposeData Should be true if the input data is column-major and
   *     false otherwise.
   * @return minimum cost error(||y-beta*X||2 is used to calculate error).
   */
            T Train(const arma::Mat<T> &data,
                    const arma::Row<T> &responses,
                    const bool transposeData = true);

            /**
   * Predict y_i for each data point in the given data matrix using the
   * currently-trained LARS model.
   *
   * @param points The data points to regress on.
   * @param predictions y, which will contained calculated values on completion.
   * @param rowMajor Should be true if the data points matrix is row-major and
   *     false otherwise.
   */
            void Predict(const arma::Mat<T> &points,
                         arma::Row<T> &predictions,
                         const bool rowMajor = false) const;

            //! Access the set of active dimensions.
            const std::vector<size_t> &ActiveSet() const { return activeSet; }

            //! Access the set of coefficients after each iteration; the solution is the
            //! last element.
            const std::vector<arma::Col<T>> &BetaPath() const { return betaPath; }

            //! Access the solution coefficients
            const arma::Col<T> &Beta() const { return betaPath.back(); }

            //! Access the set of values for lambda1 after each iteration; the solution is
            //! the last element.
            const std::vector<T> &LambdaPath() const { return lambdaPath; }

            //! Access the upper triangular cholesky factor.
            const arma::Mat<T> &MatUtriCholFactor() const { return matUtriCholFactor; }

            /**
   * Serialize the LARS model.
   */
            template <typename Archive>
            void serialize(Archive &ar, const unsigned int /* version */);

            /**
   * Compute cost error of the given data matrix using the
   * currently-trained LARS model. Only ||y-beta*X||2 is used to calculate
   * cost error.
   *
   * @param matX Column-major input data (or row-major input data if rowMajor =
   *     true).
   * @param y responses A vector of targets.
   * @param rowMajor Should be true if the data points matrix is row-major and
   *   false otherwise.
   * @return The minimum cost error.
   */
            T ComputeError(const arma::Mat<T> &matX,
                           const arma::Row<T> &y,
                           const bool rowMajor = false);

        private:
            //! Gram matrix.
            arma::Mat<T> matGramInternal;

            //! Pointer to the Gram matrix we will use.
            const arma::Mat<T> *matGram;

            //! Upper triangular cholesky factor; initially 0x0 matrix.
            arma::Mat<T> matUtriCholFactor;

            //! Whether or not to use Cholesky decomposition when solving linear system.
            bool useCholesky;

            //! True if this is the LASSO problem.
            bool lasso;
            //! Regularization parameter for l1 penalty.
            T lambda1;

            //! True if this is the elastic net problem.
            bool elasticNet;
            //! Regularization parameter for l2 penalty.
            T lambda2;

            //! Tolerance for main loop.
            T tolerance;

            //! Solution path.
            std::vector<arma::Col<T>> betaPath;

            //! Value of lambda_1 for each solution in solution path.
            std::vector<T> lambdaPath;

            //! Active set of dimensions.
            std::vector<size_t> activeSet;

            //! Active set membership indicator (for each dimension).
            std::vector<bool> isActive;

            // Set of variables that are ignored (if any).

            //! Set of ignored variables (for dimensions in span{active set dimensions}).
            std::vector<size_t> ignoreSet;

            //! Membership indicator for set of ignored variables.
            std::vector<bool> isIgnored;

            /**
   * Remove activeVarInd'th element from active set.
   *
   * @param activeVarInd Index of element to remove from active set.
   */
            void Deactivate(const size_t activeVarInd);

            /**
   * Add dimension varInd to active set.
   *
   * @param varInd Dimension to add to active set.
   */
            void Activate(const size_t varInd);

            /**
   * Add dimension varInd to ignores set (never removed).
   *
   * @param varInd Dimension to add to ignores set.
   */
            void Ignore(const size_t varInd);

            // compute "equiangular" direction in output space
            void ComputeYHatDirection(const arma::Mat<T> &matX,
                                      const arma::Col<T> &betaDirection,
                                      arma::Col<T> &yHatDirection);

            // interpolate to compute last solution vector
            void InterpolateBeta();

            void CholeskyInsert(const arma::Col<T> &newX, const arma::Mat<T> &X);

            void CholeskyInsert(T sqNormNewX, const arma::Col<T> &newGramCol);

            void GivensRotate(const arma::Col<T> &x,
                              arma::Col<T> &rotatedX,
                              arma::Mat<T> &G);

            void CholeskyDelete(const size_t colToKill);
        };

        template <typename T>
        LARS<T>::LARS(const bool useCholesky,
                      const T lambda1,
                      const T lambda2,
                      const T tolerance) : matGram(&matGramInternal),
                                           useCholesky(useCholesky),
                                           lasso((lambda1 != 0)),
                                           lambda1(lambda1),
                                           elasticNet((lambda1 != 0) && (lambda2 != 0)),
                                           lambda2(lambda2),
                                           tolerance(tolerance)
        { /* Nothing left to do. */
        }

        template <typename T>
        LARS<T>::LARS(const bool useCholesky,
                      const arma::Mat<T> &gramMatrix,
                      const T lambda1,
                      const T lambda2,
                      const T tolerance) : matGram(&gramMatrix),
                                           useCholesky(useCholesky),
                                           lasso((lambda1 != 0)),
                                           lambda1(lambda1),
                                           elasticNet((lambda1 != 0) && (lambda2 != 0)),
                                           lambda2(lambda2),
                                           tolerance(tolerance)
        { /* Nothing left to do */
        }

        template <typename T>
        LARS<T>::LARS(const arma::Mat<T> &data,
                      const arma::Row<T> &responses,
                      const bool transposeData,
                      const bool useCholesky,
                      const T lambda1,
                      const T lambda2,
                      const T tolerance) : LARS(useCholesky, lambda1, lambda2, tolerance)
        {
            Train(data, responses, transposeData);
        }

        template <typename T>
        LARS<T>::LARS(const arma::Mat<T> &data,
                      const arma::Row<T> &responses,
                      const bool transposeData,
                      const bool useCholesky,
                      const arma::Mat<T> &gramMatrix,
                      const T lambda1,
                      const T lambda2,
                      const T tolerance) : LARS(useCholesky, gramMatrix, lambda1, lambda2, tolerance)
        {
            Train(data, responses, transposeData);
        }

        // Copy Constructor.
        template <typename T>
        LARS<T>::LARS(const LARS &other) : matGramInternal(other.matGramInternal),
                                           matGram(other.matGram != &other.matGramInternal ? other.matGram : &matGramInternal),
                                           matUtriCholFactor(other.matUtriCholFactor),
                                           useCholesky(other.useCholesky),
                                           lasso(other.lasso),
                                           lambda1(other.lambda1),
                                           elasticNet(other.elasticNet),
                                           lambda2(other.lambda2),
                                           tolerance(other.tolerance),
                                           betaPath(other.betaPath),
                                           lambdaPath(other.lambdaPath),
                                           activeSet(other.activeSet),
                                           isActive(other.isActive),
                                           ignoreSet(other.ignoreSet),
                                           isIgnored(other.isIgnored)
        {
            // Nothing to do here.
        }

        // Move constructor.
        template <typename T>
        LARS<T>::LARS(LARS &&other) : matGramInternal(std::move(other.matGramInternal)),
                                      matGram(other.matGram != &other.matGramInternal ? other.matGram : &matGramInternal),
                                      matUtriCholFactor(std::move(other.matUtriCholFactor)),
                                      useCholesky(other.useCholesky),
                                      lasso(other.lasso),
                                      lambda1(other.lambda1),
                                      elasticNet(other.elasticNet),
                                      lambda2(other.lambda2),
                                      tolerance(other.tolerance),
                                      betaPath(std::move(other.betaPath)),
                                      lambdaPath(std::move(other.lambdaPath)),
                                      activeSet(std::move(other.activeSet)),
                                      isActive(std::move(other.isActive)),
                                      ignoreSet(std::move(other.ignoreSet)),
                                      isIgnored(std::move(other.isIgnored))
        {
            // Nothing to do here.
        }

        // Copy operator.
        template <typename T>
        LARS<T> &LARS<T>::operator=(const LARS<T> &other)
        {
            if (&other == this)
                return *this;

            matGramInternal = other.matGramInternal;
            matGram = other.matGram != &other.matGramInternal ? other.matGram : &matGramInternal;
            matUtriCholFactor = other.matUtriCholFactor;
            useCholesky = other.useCholesky;
            lasso = other.lasso;
            lambda1 = other.lambda1;
            elasticNet = other.elasticNet;
            lambda2 = other.lambda2;
            tolerance = other.tolerance;
            betaPath = other.betaPath;
            lambdaPath = other.lambdaPath;
            activeSet = other.activeSet;
            isActive = other.isActive;
            ignoreSet = other.ignoreSet;
            isIgnored = other.isIgnored;
            return *this;
        }

        // Move Operator.
        template <typename T>
        LARS<T> &LARS<T>::operator=(LARS<T> &&other)
        {
            if (&other == this)
                return *this;

            matGramInternal = std::move(other.matGramInternal);
            matGram = other.matGram != &other.matGramInternal ? other.matGram : &matGramInternal;
            matUtriCholFactor = std::move(other.matUtriCholFactor);
            useCholesky = other.useCholesky;
            lasso = other.lasso;
            lambda1 = other.lambda1;
            elasticNet = other.elasticNet;
            lambda2 = other.lambda2;
            tolerance = other.tolerance;
            betaPath = std::move(other.betaPath);
            lambdaPath = std::move(other.lambdaPath);
            activeSet = std::move(other.activeSet);
            isActive = std::move(other.isActive);
            ignoreSet = std::move(other.ignoreSet);
            isIgnored = std::move(other.isIgnored);
            return *this;
        }

        template <typename T>
        T LARS<T>::Train(const arma::Mat<T> &matX,
                         const arma::Row<T> &y,
                         arma::Col<T> &beta,
                         const bool transposeData)
        {
            // Clear any previous solution information.
            betaPath.clear();
            lambdaPath.clear();
            activeSet.clear();
            isActive.clear();
            ignoreSet.clear();
            isIgnored.clear();
            matUtriCholFactor.reset();

            // This matrix may end up holding the transpose -- if necessary.
            arma::Mat<T> dataTrans;
            // dataRef is row-major.
            const arma::Mat<T> &dataRef = (transposeData ? dataTrans : matX);
            if (transposeData)
                dataTrans = trans(matX);

            // Compute X' * y.
            arma::Col<T> vecXTy = trans(y * dataRef);

            // Set up active set variables.  In the beginning, the active set has size 0
            // (all dimensions are inactive).
            isActive.resize(dataRef.n_cols, false);

            // Set up ignores set variables. Initialized empty.
            isIgnored.resize(dataRef.n_cols, false);

            // Initialize yHat and beta.
            beta = arma::zeros<arma::Col<T>>(dataRef.n_cols);
            arma::Col<T> yHat = arma::zeros<arma::Col<T>>(dataRef.n_rows);
            arma::Col<T> yHatDirection(dataRef.n_rows);

            bool lassocond = false;

            // Compute the initial maximum correlation among all dimensions.
            arma::Col<T> corr = vecXTy;
            T maxCorr = 0;
            size_t changeInd = 0;
            for (size_t i = 0; i < vecXTy.n_elem; ++i)
            {
                if (fabs(corr(i)) > maxCorr)
                {
                    maxCorr = fabs(corr(i));
                    changeInd = i;
                }
            }

            betaPath.push_back(beta);
            lambdaPath.push_back(maxCorr);

            // If the maximum correlation is too small, there is no reason to continue.
            if (maxCorr < lambda1)
            {
                lambdaPath[0] = lambda1;
                return maxCorr;
            }

            // Compute the Gram matrix.  If this is the elastic net problem, we will add
            // lambda2 * I_n to the matrix.
            if (matGram->n_elem != dataRef.n_cols * dataRef.n_cols)
            {
                // In this case, matGram should reference matGramInternal.
                matGramInternal = trans(dataRef) * dataRef;

                if (elasticNet && !useCholesky)
                    matGramInternal += lambda2 * arma::eye<arma::Col<T>>(dataRef.n_cols, dataRef.n_cols);
            }

            // Main loop.
            while (((activeSet.size() + ignoreSet.size()) < dataRef.n_cols) &&
                   (maxCorr > tolerance))
            {
                // Compute the maximum correlation among inactive dimensions.
                maxCorr = 0;
                for (size_t i = 0; i < dataRef.n_cols; ++i)
                {
                    if ((!isActive[i]) && (!isIgnored[i]) && (fabs(corr(i)) > maxCorr))
                    {
                        maxCorr = fabs(corr(i));
                        changeInd = i;
                    }
                }

                if (!lassocond)
                {
                    if (useCholesky)
                    {
                        // arma::Col<T> newGramCol = arma::Col<T>(activeSet.size());
                        // for (size_t i = 0; i < activeSet.size(); ++i)
                        // {
                        //   newGramCol[i] = dot(matX.col(activeSet[i]), matX.col(changeInd));
                        // }
                        // This is equivalent to the above 5 lines.
                        arma::Col<T> newGramCol = matGram->elem(changeInd * dataRef.n_cols +
                                                                arma::conv_to<arma::uvec>::from(activeSet));

                        CholeskyInsert((*matGram)(changeInd, changeInd), newGramCol);
                    }

                    // Add variable to active set.
                    Activate(changeInd);
                }

                // Compute signs of correlations.
                arma::Col<T> s = arma::Col<T>(activeSet.size());
                for (size_t i = 0; i < activeSet.size(); ++i)
                    s(i) = corr(activeSet[i]) / fabs(corr(activeSet[i]));

                // Compute the "equiangular" direction in parameter space (betaDirection).
                // We use quotes because in the case of non-unit norm variables, this need
                // not be equiangular.
                arma::Col<T> unnormalizedBetaDirection;
                T normalization;
                arma::Col<T> betaDirection;
                if (useCholesky)
                {
                    // Check for singularity.
                    const T lastUtriElement = matUtriCholFactor(
                        matUtriCholFactor.n_cols - 1, matUtriCholFactor.n_rows - 1);
                    if (std::abs(lastUtriElement) > tolerance)
                    {
                        // Ok, no singularity.
                        /**
         * Note that:
         * R^T R % S^T % S = (R % S)^T (R % S)
         * Now, for 1 the ones vector:
         * inv( (R % S)^T (R % S) ) 1
         *    = inv(R % S) inv((R % S)^T) 1
         *    = inv(R % S) Solve((R % S)^T, 1)
         *    = inv(R % S) Solve(R^T, s)
         *    = Solve(R % S, Solve(R^T, s)
         *    = s % Solve(R, Solve(R^T, s))
         */
                        unnormalizedBetaDirection = solve(trimatu(matUtriCholFactor),
                                                          solve(trimatl(trans(matUtriCholFactor)), s));

                        normalization = T(1.0) / sqrt(dot(s, unnormalizedBetaDirection));
                        betaDirection = normalization * unnormalizedBetaDirection;
                    }
                    else
                    {
                        // Singularity, so remove variable from active set, add to ignores set,
                        // and look for new variable to add.
                        Deactivate(activeSet.size() - 1);
                        Ignore(changeInd);
                        CholeskyDelete(matUtriCholFactor.n_rows - 1);
                        continue;
                    }
                }
                else
                {
                    arma::Mat<T> matGramActive = arma::Mat<T>(activeSet.size(), activeSet.size());
                    for (size_t i = 0; i < activeSet.size(); ++i)
                        for (size_t j = 0; j < activeSet.size(); ++j)
                            matGramActive(i, j) = (*matGram)(activeSet[i], activeSet[j]);

                    // Check for singularity.
                    arma::Mat<T> matS = s * arma::ones<arma::Mat<T>>(1, activeSet.size());
                    const bool solvedOk = solve(unnormalizedBetaDirection,
                                                matGramActive % trans(matS) % matS,
                                                arma::ones<arma::Mat<T>>(activeSet.size(), 1));
                    if (solvedOk)
                    {
                        // Ok, no singularity.
                        normalization = T(1.0) / sqrt(sum(unnormalizedBetaDirection));
                        betaDirection = normalization * unnormalizedBetaDirection % s;
                    }
                    else
                    {
                        // Singularity, so remove variable from active set, add to ignores set,
                        // and look for new variable to add.
                        Deactivate(activeSet.size() - 1);
                        Ignore(changeInd);
                        continue;
                    }
                }

                // compute "equiangular" direction in output space
                ComputeYHatDirection(dataRef, betaDirection, yHatDirection);

                T gamma = maxCorr / normalization;

                // If not all variables are active.
                if ((activeSet.size() + ignoreSet.size()) < dataRef.n_cols)
                {
                    // Compute correlations with direction.
                    for (size_t ind = 0; ind < dataRef.n_cols; ind++)
                    {
                        if (isActive[ind] || isIgnored[ind])
                            continue;

                        T dirCorr = dot(dataRef.col(ind), yHatDirection);
                        T val1 = (maxCorr - corr(ind)) / (normalization - dirCorr);
                        T val2 = (maxCorr + corr(ind)) / (normalization + dirCorr);
                        if ((val1 > 0) && (val1 < gamma))
                            gamma = val1;
                        if ((val2 > 0) && (val2 < gamma))
                            gamma = val2;
                    }
                }

                // Bound gamma according to LASSO.
                if (lasso)
                {
                    lassocond = false;
                    T lassoboundOnGamma = std::numeric_limits<T>::max();
                    size_t activeIndToKickOut = -1;

                    for (size_t i = 0; i < activeSet.size(); ++i)
                    {
                        T val = -beta(activeSet[i]) / betaDirection(i);
                        if ((val > 0) && (val < lassoboundOnGamma))
                        {
                            lassoboundOnGamma = val;
                            activeIndToKickOut = i;
                        }
                    }

                    if (lassoboundOnGamma < gamma)
                    {
                        gamma = lassoboundOnGamma;
                        lassocond = true;
                        changeInd = activeIndToKickOut;
                    }
                }

                // Update the prediction.
                yHat += gamma * yHatDirection;

                // Update the estimator.
                for (size_t i = 0; i < activeSet.size(); ++i)
                {
                    beta(activeSet[i]) += gamma * betaDirection(i);
                }

                // Sanity check to make sure the kicked out dimension is actually zero.
                if (lassocond)
                {
                    if (beta(activeSet[changeInd]) != 0)
                        beta(activeSet[changeInd]) = 0;
                }

                betaPath.push_back(beta);

                if (lassocond)
                {
                    // Index is in position changeInd in activeSet.
                    if (useCholesky)
                        CholeskyDelete(changeInd);

                    Deactivate(changeInd);
                }

                corr = vecXTy - trans(dataRef) * yHat;
                if (elasticNet)
                    corr -= lambda2 * beta;

                T curLambda = 0;
                for (size_t i = 0; i < activeSet.size(); ++i)
                    curLambda += fabs(corr(activeSet[i]));

                curLambda /= ((T)activeSet.size());

                lambdaPath.push_back(curLambda);

                // Time to stop for LASSO?
                if (lasso)
                {
                    if (curLambda <= lambda1)
                    {
                        InterpolateBeta();
                        break;
                    }
                }
            }

            // Unfortunate copy...
            beta = betaPath.back();

            return ComputeError(matX, y, !transposeData);
        }

        template <typename T>
        T LARS<T>::Train(const arma::Mat<T> &data,
                         const arma::Row<T> &responses,
                         const bool transposeData)
        {
            arma::Col<T> beta;
            return Train(data, responses, beta, transposeData);
        }

        template <typename T>
        void LARS<T>::Predict(const arma::Mat<T> &points,
                              arma::Row<T> &predictions,
                              const bool rowMajor) const
        {
            // We really only need to store beta internally...
            if (rowMajor)
                predictions = trans(points * betaPath.back());
            else
                predictions = betaPath.back().t() * points;
        }

        // Private functions.
        template <typename T>
        void LARS<T>::Deactivate(const size_t activeVarInd)
        {
            isActive[activeSet[activeVarInd]] = false;
            activeSet.erase(activeSet.begin() + activeVarInd);
        }

        template <typename T>
        void LARS<T>::Activate(const size_t varInd)
        {
            isActive[varInd] = true;
            activeSet.push_back(varInd);
        }

        template <typename T>
        void LARS<T>::Ignore(const size_t varInd)
        {
            isIgnored[varInd] = true;
            ignoreSet.push_back(varInd);
        }

        template <typename T>
        void LARS<T>::ComputeYHatDirection(const arma::Mat<T> &matX,
                                           const arma::Col<T> &betaDirection,
                                           arma::Col<T> &yHatDirection)
        {
            yHatDirection.fill(0);
            for (size_t i = 0; i < activeSet.size(); ++i)
                yHatDirection += betaDirection(i) * matX.col(activeSet[i]);
        }

        template <typename T>
        void LARS<T>::InterpolateBeta()
        {
            int pathLength = static_cast<int>(betaPath.size());

            // interpolate beta and stop
            T ultimateLambda = lambdaPath[pathLength - 1];
            T penultimateLambda = lambdaPath[pathLength - 2];
            T interp = (penultimateLambda - lambda1) / (penultimateLambda - ultimateLambda);

            betaPath[pathLength - 1] = (1 - interp) * (betaPath[pathLength - 2]) + interp * betaPath[pathLength - 1];

            lambdaPath[pathLength - 1] = lambda1;
        }

        template <typename T>
        void LARS<T>::CholeskyInsert(const arma::Col<T> &newX, const arma::Mat<T> &X)
        {
            if (matUtriCholFactor.n_rows == 0)
            {
                matUtriCholFactor = arma::Mat<T>(1, 1);

                if (elasticNet)
                    matUtriCholFactor(0, 0) = sqrt(dot(newX, newX) + lambda2);
                else
                    matUtriCholFactor(0, 0) = norm(newX, 2);
            }
            else
            {
                arma::Col<T> newGramCol = trans(X) * newX;
                CholeskyInsert(dot(newX, newX), newGramCol);
            }
        }

        template <typename T>
        void LARS<T>::CholeskyInsert(T sqNormNewX, const arma::Col<T> &newGramCol)
        {
            int n = static_cast<int>(matUtriCholFactor.n_rows);

            if (n == 0)
            {
                matUtriCholFactor = arma::Mat<T>(1, 1);

                if (elasticNet)
                    matUtriCholFactor(0, 0) = sqrt(sqNormNewX + lambda2);
                else
                    matUtriCholFactor(0, 0) = sqrt(sqNormNewX);
            }
            else
            {
                arma::Mat<T> matNewR = arma::Mat<T>(n + 1, n + 1);

                if (elasticNet)
                    sqNormNewX += lambda2;

                arma::Col<T> matUtriCholFactork = solve(trimatl(trans(matUtriCholFactor)),
                                                        newGramCol);

                matNewR(arma::span(0, n - 1), arma::span(0, n - 1)) = matUtriCholFactor;
                matNewR(arma::span(0, n - 1), n) = matUtriCholFactork;
                matNewR(n, arma::span(0, n - 1)).fill(0.0);
                matNewR(n, n) = sqrt(sqNormNewX - dot(matUtriCholFactork,
                                                      matUtriCholFactork));

                matUtriCholFactor = matNewR;
            }
        }

        template <typename T>
        void LARS<T>::GivensRotate(const arma::Col<T> &x,
                                   arma::Col<T> &rotatedX,
                                   arma::Mat<T> &matG)
        {
            if (x(1) == 0)
            {
                matG = arma::eye<arma::Col<T>>(2, 2);
                rotatedX = x;
            }
            else
            {
                T r = norm(x, 2);
                matG = arma::Mat<T>(2, 2);

                T scaledX1 = x(0) / r;
                T scaledX2 = x(1) / r;

                matG(0, 0) = scaledX1;
                matG(1, 0) = -scaledX2;
                matG(0, 1) = scaledX2;
                matG(1, 1) = scaledX1;

                rotatedX = arma::Col<T>(2);
                rotatedX(0) = r;
                rotatedX(1) = 0;
            }
        }

        template <typename T>
        void LARS<T>::CholeskyDelete(const size_t colToKill)
        {
            size_t n = matUtriCholFactor.n_rows;

            if (colToKill == (n - 1))
            {
                matUtriCholFactor = matUtriCholFactor(arma::span(0, n - 2),
                                                      arma::span(0, n - 2));
            }
            else
            {
                matUtriCholFactor.shed_col(colToKill); // remove column colToKill
                n--;

                for (size_t k = colToKill; k < n; ++k)
                {
                    arma::Mat<T> matG;
                    arma::Col<T> rotatedVec(2);
                    GivensRotate(matUtriCholFactor(arma::span(k, k + 1), k), rotatedVec,
                                 matG);
                    matUtriCholFactor(arma::span(k, k + 1), k) = rotatedVec;
                    if (k < n - 1)
                    {
                        matUtriCholFactor(arma::span(k, k + 1), arma::span(k + 1, n - 1)) =
                            matG * matUtriCholFactor(arma::span(k, k + 1),
                                                     arma::span(k + 1, n - 1));
                    }
                }

                matUtriCholFactor.shed_row(n);
            }
        }

        template <typename T>
        T LARS<T>::ComputeError(const arma::Mat<T> &matX,
                                const arma::Row<T> &y,
                                const bool rowMajor)
        {
            if (rowMajor)
            {
                return arma::accu(arma::pow(y - trans(matX * betaPath.back()), 2.0));
            }
            else
            {
                return arma::accu(arma::pow(y - betaPath.back().t() * matX, 2.0));
            }
        }

} // namespace isx

#endif // ISX_LASSO_LARS_HPP
