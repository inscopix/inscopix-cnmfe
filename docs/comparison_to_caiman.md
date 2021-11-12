# Comparison of Inscopix-CNMFe and CaImAn CNMF-E
This section provides an in-depth comparison of Inscopix-CNMFe and CaImAn CNMF-E.
We highlight key differences between the two implementations, provided a parameter mapping between them,
a comparison of the outputs obtained by each algorithm, and processing time across various datasets.

## Input Parameters

### Parameter Mapping
The table below provides a mapping from the parameters used in Inscopix-CNMFe to the corresponding parameters in CaImAn.
Note that some parameters are unique to Inscopix-CNMFe and relate to features not available in open-source implementations.
This includes the trace output units and the explicit processing modes.

| Name in Inscopix-CNMFe  | Name in CaImAn CNMF-E | Description | Possible Values |
|:----------|:-------------|:-------------|:-------------|
| average_cell_diameter | gSiz | the average cell diameter of a representative cell in pixels | Positive integer |
| min_pixel_correlation | min_corr | the minimum correlation of a pixel with its immediate neighbors when searching for new cell centers | Decimal number between 0 and 1 |
| min_peak_to_noise_ratio | min_pnr | the minimum peak-to-noise ratio of a pixel when searching for new cell centers | Decimal number between 0 and 1 |
| gaussian_kernel_size | gSig | the width in pixels of the Gaussian kernel used for spatial filtering of the movie before cell initialization | Positive integer |
| closing_kernel_size | se | the size in pixels of the morphological closing kernel used for removing small disconnected components and connecting small cracks within individual cell footprints | Positive integer |
| background_downsampling_factor | ssub_B | the spatial downsampling factor to use when estimating the background activity | Positive integer |
| ring_size_factor | ring_size_factor | the multiple of the average cell diameter to use for computing the radius of the ring model used for estimating the background activity | Positive decimal number |
| merge_threshold | merge_thr | the temporal correlation threshold for merging cells that are spatially close | Decimal number between 0 and 1 |
| number_of_threads | n_processes | the number of threads to use for processing | Positive integer |
| processing_mode | N/A | the processing mode to use to run CNMFe | Integer in {0, 1, 2} </br> <ul><li>0: all in memory</li><li>1: sequential patches</li><li>2: parallel patches</li></ul> |
| patch_size | rf (used to specify the half-size of a patch) | the side length of an individual square patch of the field of view in pixels | Positive integer |
| patch_overlap | stride | the amount of overlap between adjacent patches in pixels | Positive integer |
| output_units | N/A | the units of the output temporal traces | Integer in {0, 1} </br> <ul><li>0: dF</li><li>1: dF over noise</li></ul> |
| output_filetype | N/A | the file types into which the output will be saved | Integer in {0, 1} </br> <ul><li>0: footprints saved to a tiff file and traces saved to a csv file</li><li>1: output saved to a h5 file under the keys footprints and traces</li></ul> |

### Parameters Automatically Set in Inscopix-CNMFe
Some parameters offered CaImAn CNMF-E are automatically determined or set to fixed value in Inscopix-CNMFe as described in the table below.

| Name in CaImAn CNMF-E  | Description | Value in Inscopix-CNMFe | Corresponding Value in CaImAn CNMF-E |
|:----------|:-------------|:-------------|:-------------|
| K | Maximum number of cells | Automatically determined | None |
| ssub | Spatial downsampling factor | N/A | 1 |
| tsub | Temporal downsampling factor | N/A | 1 |
| nb | Number of background components | N/A | 0 (returns background as b and W) |
| border_pix | Minimum distance from image boundary for neuron initialization | 0 | 0 |
| p | Order of the autoregressive model for temporal deconvolution | 1 | 1 |
| n_pixels_per_process | Number of pixels to process at once during spatial updates | 1000 | 1000 |
| method_deconvolution | Algorithm to use for temporal deconvolution | oasis | oasis |
| noise_range | Range of normalized frequencies to average to estimate noise | [0.25, 0.50] | [0.25, 0.50] |
| noise_method | Averaging method for estimating noise | mean | mean |
| lags | Number of lags for estimating the time constants of the autoregressive model | 5 | 5 |
| fudge_factor | Fudge factor for reducing time constant bias | 0.96 | 0.96 |

### Processing Modes
Three processing modes are available to run CNMFe: all in memory, sequential patches, parallel patches. 
The chosen processing mode determines the amount of computing resources that will be used by the algorithm and how work will be divided over such resources. 
In CaImAn CNMF-E, the processing mode of CNMFe is inferred based on user-specified parameters. 
In Incopix-CNMFe, the processing mode is explicitly specified by the user and mode-specific parameters rendered available as appropriate. 
The processing modes as implemented in Incopix-CNMFe are described and described in the table below.

| Processing Mode  | Description |
|:----------|:-------------|
| All in memory | The entire field of view is loaded into memory and processed at once. |
| Sequential patches | The field of view is first split into spatially distinct areas (referred to as patches) that can overlap. Each patch is then processed one at a time using the specified number of threads where parallelization is possible. |
| Parallel patches | The field of view is first split into spatially distinct areas (referred to as patches) that can overlap. Patches are then processed in parallel using a single thread for each. The total number of threads allocated to CNMFe sets an upper limit on the number of patches that can be processed simultaneously. |

One distinction between Inscopix-CNMFe and CaImAn CNMF-E is in how the boundaries of individual patches are determined. 
In Inscopix-CNMFe, the dimensions of individual patches are fixed while the amount of overlap is allowed to vary to account for uneven movie dimensions. 
By contrast, in CaImAn CNMF-E the dimensions of individual patches are specified but can vary depending on the dimensions of the field of view.

## Algorithm Outputs
The CNMFe algorithm outputs the spatial footprints and temporal activity of all cells identified during processing. 
An important distinction between Inscopix-CNMFe and CaImAn CNMF-E is that the temporal traces obtained using CaImAn CNMF-E 
are deconvolved using an autoregressive model while the traces obtained from Inscopix-CNMFe are referred to as the raw traces, 
i.e. the traces prior to such deconvolution. The temporal traces are also scaled differently between the two implementations depending on the selected output units. 
Two options are available in the Inscopix-CNMFe as described below.

| Output Units | Description |
|:----------|:-------------|
| dF | The temporal traces are on the same scale of pixel intensity as the original movie. dF is calculated as the average fluorescence activity of all pixels in a cell, scaled so that each spatial footprint has a magnitude of 1. |
| dF over noise | The temporal traces are divided by their respective estimated noise level. This can be interpreted similarly to a z-score, with the added benefit that the noise is a more robust measure of the variance in a temporal trace compared to the standard deviation. |

## Strategy for Comparing Inscopix-CNMFe and CaImAn
In this section we compare the results obtained by Inscopix-CNMFe to those obtained using CaImAn on the same datasets.
We refer to the footprints and associated temporal traces together as a cell set.

### Approach for Comparing Cell Sets
In order to compare the outputs from Inscopix-CNMFe to those obtained from CaImAn, 
we first defined a metric to quantify the similarity between two cell sets. 
The concept is best explained by first comparing one cell to another. 
A cell is defined by its spatial footprint (2D matrix) and its temporal activity (1D vector). 
Accordingly, the comparison of two cells can be achieved by measuring the similarity between their spatial footprints and the similarity between their temporal traces. 
We evaluated spatial similarity by computing the normalized cross-correlation of the footprints, and the temporal similarity by computing the Pearson’s correlation coefficient of the traces. 
We then combined these two metrics into a single similarity measurement by averaging them, giving equal weight to the spatial and temporal components. 
We consider two cells to be the same if their spatiotemporal similarity is above some threshold, referring to such an event as a match. 
Below is an example where we compared the spatiotemporal profiles of a cell identified using Inscopix-CNMFe (first row) to a cell identified using CaImAn (second row).

TO BE COMPLETED: REPLACE "IDPS" WITH INSCOPIX-CNMFE
![Single Cell Comparison](../img/single_cell_comparison.png?raw=true "Single Cell Comparison")

To adapt the spatiotemporal correlation metric to entire cell sets, we calculated the pairwise spatial similarity and the pairwise temporal similarity between all cells from the two cell sets. 
From these matrices, we identified for each cell in the first cell set the cell from the second cell set with which it had the maximum spatial correlation. 
We then employed a greedy approach to compare and match cells starting from the pair with the highest spatial correlation. 
For each such pair of cells, i.e. one cell from CaImAn and the cell from Inscopix-CNMFe with which it had the maximum spatial correlation, we evaluated the spatiotemporal correlation. 
We called two cells a match, i.e. considered to be the same, if their spatiotemporal correlation was above a given threshold.

Rather than arbitrarily choosing a matching threshold, we varied the threshold from 0 to 1 and computed the proportion of cells matched at each value. 
This process generates a curve that shows the effect of varying the matching threshold on the number of cells that were matched as shown below. 
For near-identical cell sets, we would expect that even when using strict (high) threshold values the majority of cells would be matched. 
To evaluate the overall similarity of the two cell sets across all possible thresholds, we computed the area under the curve by integrating it using the trapezoidal composite rule, 
a concept analogous to the measurement of the area under the receiver operating characteristic (ROC) curve used in classification problems. 
To account for the possibility that the number of cells in the two cell sets may be different, the proportion of cells matched was computed as the total number of matches over the number of cells in the cell set that had the most cells.

![ROC Curve](../img/roc_curve.png?raw=true "ROC Curve")

## Results

## Comparison of Outputs Across Various Brain Regions
TO BE COMPLETED

### Comparison of the Components Identified
TO BE COMPLETED

### Differences Between the Cell Sets Produced by Inscopix-CNMFe and CaImAn CNMF-E
TO BE COMPLETED

### Runtime Comparison
TO BE COMPLETED

### Consistency Comparison Across Processing Modes
In this section we take a closer look at the consistency of the output produced by CNMFe under the different processing modes. 
We quantify consistency using the AUC metric defined above, i.e. by comparing the cell sets produced under different processing modes. 
Since both parallel and sequential patch modes yield the same components, we compared all-in-memory mode with parallel patch mode, which we refer to simply as patch mode.

We ran CNMFe on a movie consisting of 1000 frames collected at 10 Hz, with a field of view of size 128x128 pixels. 
As shown in the table below, both Inscopix-CNMFe and CaImAn CNMF-E's results were consistent across the different processing modes.

|  | Inscopix-CNMFe | CaImAn CNMF-E |
|:----------|:----------|:-------------|
| Number of cells (all-in-memory mode) | 268 | 268 |
| Number of cells (patch mode) | 262 | 261 |
| AUC | 0.9873 | 0.9779 |

TO BE COMPLETED: VALIDATE CONTENT OF THE ABOVE TABLE WITH THE LATEST VERSION OF CNMFE

## Notes on Matching the Outputs from Inscopix-CNMFe and CaImAn CNMF-E
In order for CaIman and Inscopix-CNMFe to produce comparable results, all analysis parameters must be matched as per the parameter mapping table above. 
The parameters not listed in that table are automatically set to CaImAn’s default values. 
CaImAn’s default processing mode is parallel patch mode, with the patch size set through the “rf” parameter. 
The all-in-memory mode can be used by setting “rf” equal to “None”. 
The sequential patch mode can be used when “rf” is set to a positive value by setting the “dview” parameter of the CNMF object to “None” upon instantiation. 
In Inscopix-CNMFe, the processing mode can be specified explicitly via the input parameter `processing_mode`.
The output units in Inscopix-CNMFe should be set to "dF" to match CaImAn’s units. 
Note that Inscopix-CNMFe outputs raw temporal traces while CaImAn outputs denoised temporal traces.

TO BE COMPLETED

## Notes on MATLAB CNMF_E
CNMFe was originally developed in MATLAB and later ported to Python. 
Both versions are widely used despite some algorithmic differences between them. 
Specifically, the number of cells and the magnitude of the temporal traces can differ between these two versions of CNMFe.
Since Inscopix-CNMFe was developed based primarily on the Python implementation, the output from Inscopix-CNMFe is similar 
to that of CaImAn and shares the same differences with the MATLAB version.
