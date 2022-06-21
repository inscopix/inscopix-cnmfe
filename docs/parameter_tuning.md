# Inscopix CNMF-E Parameters
This section provides guidance and tips on how to fine-tune the CNMF-E input parameters
based on the results obtained by the algorithm. The default parameters may not be optimal for all datasets. 
As such, it is important to understand how each parameter affects processing in order to
determine which ones to tweak to obtain the desired output.

## Table of Contents
- [Relationships between Individual Parameters and CNMF-E Processing Modules](#relationships-between-individual-parameters-and-cnmf-e-processing-modules)
- [Initialization Parameters](#initialization-parameters)
  - [Preprocessing](#preprocessing)
  - [Seed Pixels](#seed-pixels)
- [Background Parameters](#background-parameters)
  - [Downsampling](#downsampling)
  - [Background Model](#background-model)
- [Spatial Parameters](#spatial-parameters)
- [Merging Parameters](#merging-parameters)
- [Processing Parameters](#processing-parameters)
  - [Processing Modes](#processing-modes)
  - [Patch Size and Patch Overlap](#patch-size-and-patch-overlap)
  - [Number of Threads](#number-of-threads)
- [Auto-Estimated Parameters](#auto-estimated-parameters)
- [Common Issues](#common-issues)
  - [Inaccurate Average Cell Diameter](#inaccurate-average-cell-diameter)
  - [Missing Cells](#missing-cells)
  - [Too Many Cells](#too-many-cells)
  - [Oversegmented Cells](#oversegmented-cells)
  - [Slow Processing](#slow-processing)
  - [Limited Computing Resources](#limited-computing-resources)
- [Output Units](#output-units)

## Interactive parameter explorer

To build intuition about what some CNMF-E parameters do on real datasets, 
the following interactive tools provide instantaneous feedback showing
you what effect these parameters:

- the cell diameter
- Gaussian kernel size
- minimum peak-to-noise ratio
- minimum correlation between seed pixels

have on extracted cells and their footprints. 

[CNMF-E parameter explorer -- Striatal data](https://inscopix.github.io/inscopix-cnmfe/parameter-tuning/striatum_IM1/)

## Relationships between Individual Parameters and CNMF-E Processing Modules
The flow chart below shows how data flows through the different CNMF-E modules during processing.
The table underneath lists the input parameters that can be fine-tuned, with each row color-coded 
according to the module the parameter has an effect on.

![CNMF-E Modules Overview](../img/cnmfe_modules_overview.png?raw=true "CNMF-E Modules Overview")

![CNMF-E Parameter-Module Mapping](../img/cnmfe_parameter_module_mapping.png?raw=true "CNMF-E Parameter-Module Mapping")

## Initialization Parameters

### Preprocessing
One of the first steps of the initialization module is to apply a Gaussian filter with width roughly
equal to the radius of a cell in pixels.
This effectively make cells pop out in the correlation image, which ultimately makes them easier to identify.
An example of the effect of using different Gaussian filter sizes is shown below. 
In this case, a Gaussian filter width of 5 produces a crisp correlation image that leads to more identified cells.

![Initialization: Gaussian Filter Width](../img/initialization_gaussian_filter.png?raw=true "Initialization: Gaussian Filter Width")

### Seed Pixels
Seed pixels are pixels identified as potential neuron centers. In order for a seed pixel to actually be used to initialize
the spatial footprint of a neuron, it must meet or exceed the initialization criteria. Specifically, a seed pixel must be 
sufficiently correlated with its immediate neighbors and must also have a level of temporal activity that exceeds some multiple
of the estimated noise level. The two metrics can be computed for each pixel, providing two images that can be 
combined into one search image that can be used to identify neurons that meet the initialization criteria.
An example of such images is displayed below: PNR Image obtained using a minimum peak-to-noise ratio of 10 (left), 
Correlation Image obtained using a minimum correlation of 0.8 with 8 immediate neighbor pixels (middle),
and a Search Image obtained by multiplying the PNR Image with the Correlation Image (right).

![Initialization: Seed Pixels](../img/initialization_seed_pixels.png?raw=true "Initialization: Seed Pixels")

## Background Parameters

### Downsampling
Spatial downsampling is used when estimating the background to reduce the effects of local fluctations on the background estimation.
The amount of downsampling thus control the balance between local and global background.
Larger downsampling values will result in a background estimation that is primarily dominated by global fluctuations whereas
lower downsampling values will place more emphasis small variations in the background across the field of view.
Since the background estimation procedure is one of the most expensive operations in the CNMF-E algorithm, larger downsampling
values can significantly improve processing speed.

### Background Model
The background fluorescence is estimated for every pixel using a ring model, i.e. by considering the intensity
of pixels that form a ring around any given pixel. The radius of the ring controls how many pixels to use to estimate
the background and how far from the current pixel to look for background fluctuations.

## Spatial Parameters
The spatial module is responsible for processing and refining the spatial footprints of the neurons identified by the algorithm.
The closing kernel size is an input parameter that controls the morphological closing operation applied to each footprint during processing.
It enables the closing of small holes that may be visible in the center of a cellular footprint
that has a doughnut shape. The closing operation can also be used to remove small disconnected components
from cellular footprints.

## Merging Parameters
One advantage of CNMF-E is that the cell identification process allows for overlapping cells to be identified.
However, in some cases a single cell may be identified as two overlapping regions during processing.
The merging module is responsible for merging such regions together into whole cells based on the
correlation between their temporal activities.
More specifically, the merging threshold parameter specifies the minimum temporal correlation required
for two overlapping components to be merged into a single one.
If the algorithm appears to oversegment cells, then one could benefit from relaxing the merging criteria
by reducing the merging threshold.

## Processing Parameters

### Processing Modes
Three processing modes are offered in Inscopix CNMF-E, each with their own memory-speed tradeoff.
Parallel patch mode is the fastest processing mode, but it requires a large amount of memory.
Its speed advantage comes from processing smaller portions of the field of view in parallel. 
Sequential patch mode is the slowest processing mode, but requires the least amount of memory.
All-in-memory mode is also slower than parallel patch mode and requires a large amount of memory. 
The different processing modes are depicted in the figure below.

![Processing Modes](../img/processing_modes.png?raw=true "Processing Modes")

### Patch Size and Patch Overlap
Patch size refers to the side length of an individual square patch of the field of view in pixels.
Patch overlap refers to the amount of overlap between adjacent patches in pixels.
These two concepts are illustrated below.

![Patch Parameters](../img/patch_parameters.png?raw=true "Patch Parameters")

### Number of Threads
The number of threads controls the amount of parallelism used during processing.
When using all-in-memory or sequential patch mode, increasing the number of threads will
result in a higher level of parallelism in the individual CNMF-E processing modules.
However, a larger number of threads will not always lead to faster processing.
Specifically, if the number of threads exceeds the number of cores available on your machine
you may start seeing a plateau or even an increase in processing time as the overhead of managing
the extra threads start exceeding the benefits of parallelization.
When using parallel patch mode, the number of threads will instead control the number of patches
being processed in parallel at any given time.
In this mode, each thread will process one patch at a time, each time grabbing the next patch to process in the queue until all patches are processed.

## Auto-Estimated Parameters
Inscopix CNMF-E is able to automatically estimate an appropriate value for some of the input parameters based on
the specified average cell diameter. Specifically, the Gaussian filter size and closing kernel size are both automatically
estimated using the following formula if unspecified:

`floor((average cell diameter - 1) / 4)`

## Common Issues

### Inaccurate Average Cell Diameter
The average cell diameter should be obtained by measuring the diameter of a representative cell in pixels.
This parameter should not be deliberately under- or over-estimated as this could affect the quality of the output.
The user should instead try to adjust the other parameters based on the tips provided below.

### Missing Cells
If you feel that CNMF-E missed some cells that are visible in the field of view, try relaxing the initialization parameters
while maintaining the other parameters constant. 
More specifically, try reducing the minimum pixel correlation and peak-to-noise ratio.
Note that it is not recommended to specify an inaccurate average cell diameter in an attempt to capture the missing cells
as this could impair the quality of the output obtained from CNMF-E.
Below is an example showing the effect of relaxing the initialization criteria.
Some cells visible in the field of view were missed when using a minimum pixel correlation of 0.8 and
a minimum peak-to-noise ratio of 10 (left), but were identified when relaxing the initialization criteria (right).

![Missing Cells](../img/missing_cells.png?raw=true "Missing Cells")

### Too Many Cells
If you feel that CNMF-E identified too many cells, specifically blobs that don't appear to be cells, try
making the initialization parameters more stringent by increase the minimum pixel correlation and the peak-to-noise ratio.
Also, make sure to use the appropriate average cell diameter as underestimating or overestimating this value could lead to 
the identification of additional blobs that may not be cells of interest.

### Oversegmented Cells
If you find that CNMF-E is overfragmenting cells, i.e. the same cell appears to be identified as multiple distinct components,
try reducing the merging threshold to allow CNMF-E to combine such fragments together during processing.
Again, make sure to use an appropriate average cell diameter as underestimating it could lead to the identification
of smaller blobs within cells.
Below is an example showing cells that were oversegmented and identified as distinct cells.
In this case it is best to validate the average cell diameter and reduce the merging threshold
to prevent such oversegmentation of the cell bodies.

![Oversegmented Cells](../img/oversegmented_cells.png?raw=true "Oversegmented Cells")

### Slow Processing
Processing time is largely affected by the input dimensions, the chosen processing mode, and the parallelism associated
with the specified processing parameters.
Parallel patch mode is the fastest processing mode, but it requires more memory.
If you notice while running CNMF-E that you still have a large amount of unused memory available on your machine,
try increasing the number of threads used with parallel patch mode.
Background estimation is one of the most expensive operations in CNMF-E.
As such, increasing the background downsampling factor can also significantly reduce processing time
while shifting the background estimate toward a global rather than a localized estimate.
Another way to improve processing speed is to downsample the input data as per our recommended workflow.
Many operations are performed on every pixel. Downsampling can help reduce noise and also dramatically reduce processing time.

### Limited Computing Resources
If you are attempting to run CNMF-E on a system that has limited random-access memory,
you may not be able to process large movies using either the all-in-memory or parallel patch mode.
However, the sequential patch mode will allow you to process a single patch of data at a time, dramatically
reducing memory consumption at the expense of longer processing time.

If your system runs out of memory during processing, CNMF-E will likely shut down.
Most operating systems offer built-in monitoring tools to keep track of memory consumption.
To circumvent a lack of memory, try reducing the patch size or the number of threads if using parallel patch mode.
If this still consumes too much memory, try using sequential patch mode to minimize memory consumption.

Another way to significantly reduce memory consumption is to downsample the input data as per our recommended workflow.
For instance, downsampling the data by a factor of 2 along each dimension would reduce the memory footprint of the algorithm by a factor of 4.

## Output Units
The temporal traces extracted by Inscopix CNMF-E can be expressed in terms of different units.
Inscopix CNMF-E offers two options: `dF` and `dF over noise`.
It is common for people to standardize traces by subtracting the mean and dividing by the standard deviation,
effectively converting each value to a Z-score.
`dF over noise` uses a similar formula, with the exception that the traces are divided by the estimated noise level as opposed to the standard deviation.
Since noise is estimated by averaging the high-frequency fluctuations along the trace, `dF over noise` is more robust to outliers than standard scores.
More precisely, outliers would inflate the standard deviation measured and thus reduce the standard scores computed.
The approach used to measure noise here is less susceptible to the influence of outliers.

| Output Units  | Description |
|:----------|:-------------|
| dF | temporal traces on the same scale of pixel intensity as the original movie. dF is calculated as the average fluorescence activity of all pixels in a cell, scaled so that each spatial footprint has a magnitude of 1. |
| dF over noise | temporal traces divided by their respective estimated noise level. This can be interpreted similarly to a z-score, with the added benefit that the noise is a more robust measure of the variance in a temporal trace compared to the standard deviation. |
