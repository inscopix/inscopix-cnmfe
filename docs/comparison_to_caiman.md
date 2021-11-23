# Comparison of Inscopix-CNMFe and CaImAn CNMF-E
This section provides an in-depth comparison of Inscopix-CNMFe and CaImAn CNMF-E.
Key differences in the algorithm parameters, processing modes, and output formats are first highlighted.
We then present our approach for comparing the two implementations and present the results of such comparison
on various datasets.
We evaluated processing time and consistency between the outputs obtained using the two algorithms, as well as 
how processing time is affected by the input dimensions.

## Algorithm Parameters
In this section we summarize how the Inscopix-CNMFe parameters relate to or differ from those used in CaImAn CNMF-E.

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
Some parameters offered in CaImAn CNMF-E are automatically determined or set to fixed value in Inscopix-CNMFe as described in the table below.

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
In CaImAn CNMF-E, the processing mode is inferred based on user-specified parameters including the patch size (`rf`) and the view associated with the IPython Parallel computing cluster (`dview`). 
In Incopix-CNMFe, the processing mode and associated parallelism is explicitly specified by the user using the parameters ````
The processing modes as implemented in Incopix-CNMFe are described and described in the table below.

| Processing Mode  | Description |
|:----------|:-------------|
| All in memory | The entire field of view is loaded into memory and processed at once. |
| Sequential patches | The field of view is first split into spatially distinct areas (referred to as patches) that can partially overlap. Each patch is then processed one at a time using the specified number of threads where parallelization is possible. |
| Parallel patches | The field of view is first split into spatially distinct areas (referred to as patches) that can partially overlap. Patches are then processed in parallel using a single thread for each. The total number of threads allocated to CNMFe sets an upper limit on the number of patches that can be processed simultaneously. |

The boundaries of individual patches are determined using a different approach in each implementation.
In Inscopix-CNMFe, the dimensions of individual patches are fixed while the amount of overlap is allowed to vary to account for uneven movie dimensions. 
By contrast, in CaImAn CNMF-E the dimensions of individual patches are specified but can vary depending on the dimensions of the field of view.

### Output Units
The CNMFe algorithm outputs the spatial footprints and temporal activity of all cells identified during processing. 
An important distinction between Inscopix-CNMFe and CaImAn CNMF-E is that the temporal traces obtained using CaImAn CNMF-E 
are deconvolved using an autoregressive model while the default traces obtained from Inscopix-CNMFe are referred to as the raw traces, 
i.e. the traces prior to such deconvolution. The `deconvolve` parameter controls the type of traces produced by Inscopix-CNMFe (0: raw traces, 1: deconvolved traces).
The temporal traces are also scaled differently between the two implementations.
CaImAn CNMF-E traces are in terms of `dF` units whereas Inscopix-CNMFe allows the user to choose between `dF` and `dF over noise`
as described below.

| Output Units | Description |
|:----------|:-------------|
| dF | The temporal traces are on the same scale of pixel intensity as the original movie. dF is calculated as the average fluorescence activity of all pixels in a cell, scaled so that each spatial footprint has a magnitude of 1. |
| dF over noise | The temporal traces are divided by their respective estimated noise level. This can be interpreted similarly to a z-score, with the added benefit that the noise is a more robust measure of the variance in a temporal trace compared to the standard deviation. |

## Methods
In this section we present the approach we developed for comparing the footprints and temporal dynamics of neurons
identified using Inscopix-CNMFe to those obtained using CaImAn CNMF-E.
We refer to the footprints and corresponding temporal traces together as a cell set.
We also provide a summary of the hardware and software used to process the data, including the analysis parameters used for each dataset.

### Cell Set Comparison
In order to compare the outputs from Inscopix-CNMFe to those obtained from CaImAn, we first defined a metric to quantify the similarity between two cell sets.
The concept is best explained by first comparing one cell to another. 
A cell is defined by its spatial footprint (2D matrix) and its temporal activity (1D vector). 
Accordingly, two cells can be compared by measuring the similarity between their spatial footprints and the similarity between their temporal traces. 
We evaluated spatial similarity by computing the normalized cross-correlation of the footprints, and the temporal similarity by computing the Pearson’s correlation coefficient of the traces. 
We then combined these two metrics into a single similarity measurement by averaging them, giving equal weight to the spatial and temporal components. 
Under this approach, we consider two cells to be the same if their spatiotemporal similarity is above some threshold, referring to such an event as a match. 
Below is an example where we compared the spatiotemporal profiles of a cell identified using Inscopix-CNMFe (first row) to a cell identified using CaImAn (second row).

![Single Cell Comparison](../img/single_cell_comparison.png?raw=true "Single Cell Comparison")

![Spatiotemporal Similarity Formula](../img/spatiotemporal_similarity_formula.png?raw=true "Spatiotemporal Similarity Formula")

To adapt the spatiotemporal correlation metric to entire cell sets, we calculated the pairwise spatial similarity and the pairwise temporal similarity between all cells from the two cell sets. 
From these matrices, we identified for each cell in the first cell set the cell from the second cell set with which it had the maximum spatial correlation. 
We then employed a greedy approach to compare and match cells starting from the pair with the highest spatial correlation. 
We computed the spatiotemporal correlation for each such pair of cells, i.e. one cell from CaImAn and the cell from Inscopix-CNMFe with which it had the maximum spatial correlation.
We called two cells a match (i.e. considered to be the same) if their spatiotemporal correlation was above a given threshold.

Rather than arbitrarily choosing a matching threshold, we varied the threshold from 0 to 1 and computed the proportion of cells matched at each value. 
This process generates a curve that shows the effect of varying the matching threshold on the number of cells that were matched as shown below. 
For near-identical cell sets, we would expect that even when using strict (high) threshold values the majority of cells would be matched. 
To evaluate the overall similarity of the two cell sets across all possible thresholds, we computed the area under the curve by integrating it using the trapezoidal composite rule, 
a concept analogous to the measurement of the area under the receiver operating characteristic (ROC) curve.

![ROC Curve](../img/roc_curve.png?raw=true "ROC Curve")

### Hardware
All experiments were conducted on a laptop with the following specifications:

| Computer | MacBook Pro (16-inch, 2019) |
|:----------|:-------------|
| Operating System | macOS Catalina (version 10.15.7) |
| Processor | 2.4 GHz 8-Core Intel Core i9 |
| Memory | 64 GB 2667 MHz DDR4 |
| Graphics | AMD Radeon Pro 5300M 4 GB Intel UHD Graphics 630 1536 MB |

### Data Preprocessing
All datasets were downsampled both spatially and temporally by a factor of 2, bandpass filtered, and motion corrected prior to cell identification.
These operations were applied using [Inscopix Data Processing Software](https://www.inscopix.com/software-analysis#software_idps) (version 1.6.0).

### Analysis Parameters
The CNMFe parameters used for processing the data are listed below for each cell set.
We used the same analysis parameters for both Inscopix-CNMFe and CaImAn CNMF-E.
We used the same analysis parameters for both Inscopix-CNMFe and CaImAn CNMF-E.

| Parameter (as defined in Inscopix-CNMFe) | Value |
|:----------|:-------------|
| min_pixel_correlation | 0.8 |
| min_peak_to_noise_ratio | 10 |
| gaussian_kernel_size  | 3 |
| closing_kernel_size  | 3 |
| background_downsampling_factor | 2 |
| ring_size_factor | 1.4 |
| merge_threshold  | 0.7 |
| number_of_threads  | 4 |
| processing_mode  | 0 (all-in-memory mode) |
| patch_size  | 80 |
| patch_overlap  | 20 |
| deconvolve | 1 (return deconvolved traces) |
| output_units | dF |

The average cell diameter was specified for each dataset based on the average of manual diameter measurements taken across representative neurons.

| Dataset Identifier | Brain Region | Average Cell Diameter (in pixels) |
|:----------|:-------------|
| 1 | Prefrontal cortex | 15|
| 2 | Prefrontal cortex | 17 |
| 3 | Striatum | 14 |
| 4 | Striatum | 16 |
| 5 | Striatum | 13 |
| 6 | Hippocampus | 18 |
| 7 | Hippocampus | 15 |
| 8 | Hippocampus | 13 |

## Results
Using the methodology described above for comparing cell sets, we compared the components identified by Inscopix-CNMFe
to those obtained from CaImAn CNMF-E on 8 datasets collected across various brain regions including the prefrontal cortex, striatum, and hippocampus.

### Overview
An overview of the results is presented in the table below along with additional information about each dataset,
including the dimensions of the field of view, the number of frames, and the frame rate of the movie.
We obtained an average similarity (AUC) of 0.95 across all 8 datasets, indicating strong spatiotemporal similarity between the 
components identified by Inscopix-CNMFe and CaImAn CNMF-E.

| Dataset Identifier | Brain Region | Dimensions of the field of view (in pixels) | Number of frames | Frame rate (in Hz) | Average cell diameter (in pixels) | Number of cells identified by Inscopix-CNMFe | Number of cells identified by CaImAn CNMFe | AUC (similarity metric) |
|:----------|:-------------|:-------------|:-------------|:-------------|:-------------|:-------------|:-------------|:-------------|
| 1 | Prefrontal cortex | 161 x 207 | 12004 | 10 | 15 | 88 | 88 | 0.9841 |
| 2 | Prefrontal cortex | 235 x 266 | 17995 | 10 | 17 | 236 | 235 | 0.9630 |
| 3 | Striatum | 263 x 318 | 17996 | 10 | 14 | 612 | 608 | 0.9619 |
| 4 | Striatum | 226 x 272 | 17995 | 10 | 16 | 526 | 516 | 0.9628 |
| 5 | Striatum | 128 x 128 | 1000 | 10 | 13 | 268 | 268 | 0.9824 |
| 6 | Hippocampus | 159 x 219 | 17996 | 10 | 18 | 301 | 301 | 0.9616 |
| 7 | Hippocampus | 220 x 261 | 17995 | 10 | 15 | 665 | 652 | 0.9621 |
| 8 | Hippocampus | 196 x 242 | 12100 | 10 | 13 | 929 | 922 | 0.8490 |

### Components Identified Across Various Brain Regions
To visualize these comparison results, we generated a polyptych consistening of four panels for each dataset. The first panel is a summary image of the input movie which shows the correlation of each pixel in the FOV with its 8 neighbours. The second and third panels are the cell maps of the corresponding Inscopix CNMF-E and CaImAn CNMF-E outputs generated from the input movie. The fourth panel shows an overlay of the two cell maps in order to highlight the differences in the two outputs. Inscopix CNMF-E cells are shown in orange, CaImAn CNMF-E cells are shown in orange, and overlapping areas are shown in white.

**Dataset #1, Prefrontal Cortex**
![PFC_V3_38](../img/pfc_v3_38_comparison.png?raw=true "PFC_V3_38")

**Dataset #2, Prefrontal Cortex**
![PFC_IM2](../img/pfc_im2_comparison.png?raw=true "PFC_IM2")

**Dataset #3, Striatum**
![Striatum_IM1](../img/striatum_im1_comparison.png?raw=true "Striatum_IM1")

**Dataset #4, Striatum**
![Striatum_IM2](../img/striatum_im2_comparison.png?raw=true "Striatum_IM2")

**Dataset #5, Striatum**
![sample_128x128x1000](../img/sample_128x128x1000_comparison.png?raw=true "sample_128x128x1000")

**Dataset #6, Hippocampus**
![Hippocampus_BO4](../img/hippocampus_BO3_comparison.png?raw=true "Hippocampus_BO4")

**Dataset #7, Hippocampus**
![Hippocampus_BO4](../img/hippocampus_BO4_comparison.png?raw=true "Hippocampus_BO4")

**Dataset #8, Hippocampus**
![Hippocampus_DD1](../img/hippocampus_DD1_comparison.png?raw=true "Hippocampus_DD1")

### Differences Between the Cell Sets Produced by Inscopix-CNMFe and CaImAn CNMF-E
While the results above show strong similarity between the output produced by IDPS and CaImAn, one dataset in particular led to a lower AUC value when compared to the other datasets. We took a closer look at dataset #8 to better understand what gave rise to a slightly lower AUC value. First, it is worth noting that both cell sets have a similar number of cells, specifically 922 and 935 for CaImAn and IDPS, respectively. When we visualized the data, we found differences in the footprints produced by the two algorithms. Some spatial footprints in CaImAn were significantly larger than those obtained in IDPS. Below are some components from CaImAn showing irregularities in the footprints along with the corresponding temporal traces. These irregularities in the spatial footprint can have a dual effect on our AUC metric, first by reducing the spatial correlation with components from IDPS, and since these footprints are used internally by CNMFe to extract the temporal activity the traces will likely be noisier and thus result in lower temporal correlation with the traces from IDPS. While no such footprints were identified in IDPS, it remains clear that on this dataset both IDPS and CaImAn identified cells that would likely be filtered out when evaluating components.

![Caiman differences pt1](../img/caiman_differences_pt1.png?raw=true "Caiman differences pt1")

![Caiman differences pt2](../img/caiman_differences_pt2.png?raw=true "Caiman differences pt2")

### Processing Time Comparison
We compared the processing time of Inscopix-CNMFe and CaImAn CNMF-E on movies of different dimensions and durations.
We first measured processing time for a dataset consisting of 1000 frames of dimensions 128x128 pixels recorded in the striatum.
We then spatially expanded the data to measure the impact of the movie dimensions on processing time. 
Similarly, we expanded the movie temporally by concatenating copies of the same movie to measure the impact of the number of frames on processing time while keeping cell density constant. 
This provided us with a comprehensive set of movies that we used to compare the processing time of the two CNMFe implementations.

### Processing Time as a Function of the Dimensions of the Field of View
![Different FOV Dimensions](../img/different_fov_dimensions.png?raw=true "Different FOV Dimensions")

We compared the processing time of the two CNMFe implementations on datasets consisting of 100 frames with dimensions varying from 128x128 to 512x512 pixels. 
The components identified by both algorithms were almost identical for all frame dimensions as indicated by the high AUC values.
The minor differences reflected in the AUC values slightly under 1.0 may be due to how patches are handled in each implementation.
While both implementations identified roughly the same components, Inscopix-CNMFe was faster to complete across all datasets as shown below.

TODO: fill in table
|  | 128 x 128 | 256 x 256 | 384 x 384 | 512 x 512 |
|:----------|:-------------|:-------------|:-------------|:-------------|
| Number of cells identified by Inscopix-CNMFe | 78 | 309 | 688 | 1208 |
| Number of cells identified by CaImAn CNMF-E | 79 | 305 | 688 | 1217 |
| AUC (similarity metric) | 0.9847 | 0.9571 | 0.9655 | 0.9603 |

![Processing Time as a Function of FOV Dimensions](../img/processing_time_fov.png?raw=true "Processing Time as a Function of FOV Dimensions")

### Processing Time as a Function of the Number of Frames
![Different Number of Frames](../img/different_num_frames.png?raw=true "Different Number of Frames")

We compared the processing time of Inscopix-CNMFe and CaImAn CNMF-E as a function of the number of frames in the input movie.
We use a field of view of size 128 x 128 pixels and varied the number of frames from 1000 to 50000.
While both implementations identified roughly the same components, as indicated by the high AUC values, 
Inscopix-CNMFe was faster to complete across all datasets as shown below.

TODO: fill in table
|  | 1000 | 5000 | 10000 | 20000 | 30000 | 50000 |
|:----------|:-------------|:-------------|:-------------|:-------------|:-------------|:-------------|
| Number of cells identified by Inscopix-CNMFe | 262 | 265 | 261 | 263 | 262 | 263 |
| Number of cells identified by CaImAn CNMF-E | 261 | 266 | 263 | 262 | 263 | 261 |
| AUC (similarity metric) | 0.9797 | 0.9763 | 0.9846 | 0.9748 | 0.9799 | 0.9755 |

![Processing Time as a Function of the Number of Frames](../img/processing_time_number_of_frames.png?raw=true "Processing Time as a Function of the Number of Frames")

## Notes on Matching the Outputs from Inscopix-CNMFe and CaImAn CNMF-E
In order for Inscopix-CNMFe and CaImAn CNMF-E to produce comparable results, all analysis parameters must be matched as per the parameter mapping table above. 
The parameters not listed in that table are automatically set to CaImAn’s default values. 
CaImAn's default processing mode is parallel patch mode, with the patch size set through the `rf` parameter. 
The all-in-memory mode can be used by setting `rf` equal to `None`. 
The sequential patch mode is used when `rf` is set to a positive value by setting the `dview` parameter of the CNMF object to `None` upon instantiation. 
In Inscopix-CNMFe, the processing mode can be specified explicitly via the input parameter `processing_mode`.
The output units in Inscopix-CNMFe should be set to `dF` to match CaImAn’s units.