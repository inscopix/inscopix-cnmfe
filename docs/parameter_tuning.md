# Inscopix CNMFe Parameters
This section provides guidance and tips on how to fine-tune the CNMFe input parameters
based on the results obtained. While the default parameters used in CNMFe were determined by 
applying them to several datasets, they may not be optimal for all datasets. 
As such, it is important to understand which parameter to tweak to obtain the desired output.

**Table of Contents**
- [Recommended Workflow](#recommended-workflow)
- [Relationships Between CNMFe Modules and Individual Parameters](#relationships-between-cnmfe-modules-and-individual-parameters)
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
  - [Average Cell Diameter](#average-cell-diameter)
  - [Missing Cells](#missing-cells)
  - [Too Many Cells](#too-many-cells)
  - [Oversegmented Cells](#oversegmented-cells)
  - [Slow Processing](#slow-processing)
  - [Limited Computing Resources](#limited-computing-resources)
- [Output Units](#output-units)

## Recommended Workflow
Prior to running CNMFe on your movie, we recommend applying the following operations
to help improve the source extraction algorithm.
1. Spatial downsampling of the data by a factor of 2 to 4. This will help blur away 
minor spatial fluctuations and significantly reduce CNMFe processing time.
2. Spatial bandpass filtering with global mean subtraction. The removal of low spatial
frequency content will help to remove out-of-focus cells. Likewise, the removal of 
high spatial frequencies will reduce noise by smoothing the movie images.
3. Motion correction. The removal of motion artifacts will help ensure that the spatial 
location of cells identified by CNMFe is confined to their precise positions as
opposed to the pixels visited by their respective cell body over time. 
This will in turn ensure that the temporal dynamics extracted for each cell are due to 
fluctuations in the reporter fluorescence and not cellular displacements.

## Relationships Between CNMFe Modules and Individual Parameters
The diagram and table below provide a color-coded mapping from the CNMFe parameters to the
CNMFe processing modules they have an effect on.

![CNMFe Modules Overview](../img/cnmfe_modules_overview.png?raw=true "CNMFe Modules Overview")

![CNMFe Parameter-Module Mapping](../img/cnmfe_parameter_module_mapping.png?raw=true "CNMFe Parameter-Module Mapping")

## Initialization Parameters

### Preprocessing
One of the first steps of the initialization module is to apply a Gaussian filter with width roughly
equal to the radius of a cell in pixels.
This effectively applies a high-pass filter to the movie to make cells pop out in the correlation image,
which ultimately makes them easier to identify.
An example of the effect of using different Gaussian filter sizes is shown below.

![Initialization: Gaussian Filter Width](../img/initialization_gaussian_filter.png?raw=true "Initialization: Gaussian Filter Width")

### Seed Pixels
Seed pixels are pixels identified as potential neuron centers. In order for such seed pixels to actually be used to initialize
the spatial footprint of neuron, it must meet or exceed the initialization criteria. Specifically, a seed pixel must be 
sufficiently correlated with its immediate neighbor and must also have a level of temporal activity that exceed some multiple
of the estimated noise level. The two metrics can be computed for each pixel, providing two images that can then 
combined into an image from which to search for neurons by focusing specifically on the pixels that met the initialization criteria.
An example of such images is displayed below: PNR Image obtained using a peak-to-noise ratio of 10 (left), 
Correlation Image obtained using a minimum correlation of 0.8 with 8 immediate neighbor pixels (middle),
and a Search Image obtained by multiplying the PNR Image with the Correlation Image (right).

![Initialization: Seed Pixels](../img/initialization_seed_pixels.png?raw=true "Initialization: Seed Pixels")

## Background Parameters

### Downsampling
Spatial downsampling is used when estimating the background to reduce the effects of local fluctations on the background estimation.
The amount of downsampling thus control the balance between local and global background.
Larger downsampling values will result in a background estimation that is primarily dominated by global fluctuations whereas
lower downsampling values will account for small variations in the background across the field of view.
Since the background estimation procedure is one of the most expensive operation in the CNMFe algorithm, larger downsampling
values can significantly improve processind speed.

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
One advantage of CNMFe is that the cell identification process allows for overlapping cells to be identified.
However, in some cases a single cell may be identified as two overlapping regions during processing.
The merging module is responsible for merging such regions together into whole cells based on the
correlation between their temporal activities.
More specifically, the merging threshold parameter specifies the minimum temporal correlation required
for two overlapping components to be merged into a single one.
If the algorithm appears to oversegment cells, then one could benefit from relaxing the merging criteria
by reducing the merging threshold.

## Processing Parameters

### Processing Modes
Parallel patch mode is the fastest processing mode, but it requires a large amount of memory.
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
result in a higher level of parallelism in each CNMFe processing module.
A larger number of threads will not always lead to faster processing.
Specifically, if the number of threads exceed the number of cores available on your machine
you may see start seeing a plateau or even an increase in processing time as the overhead of managing
the extra threads start exceeding the benefits provided through parallelization.
When using parallel patch mode, the number of threads will instead control the number of patches
being processed in parallel at any given time.
Each thread will process one patch at a time, each time grabbing the next patch to process in the queue, until all patches are processed.

## Auto-Estimated Parameters
Inscopix CNMFe is able to automatically estimated an appropriate value for some of the input parameters based on
the specified average cell diameter. Specifically, the Gaussian filter size and closing kernel size are both automatically
estimated using the following formula if unspecified:

`floor((average cell diameter - 1) / 4)`

## Common Issues

### Average Cell Diameter
The average cell diameter should be obtained by measuring the diameter of a representative cell in pixels.
This parameter should not be deliberately under- or over-estimated as this could affect the quality of the output.
The user should instead try to adjust the other parameters based on the tips provide below and depending on the results obtained.

### Missing Cells
If you feel that CNMFe missed some cells that are visible in the field of view, try relaxing the initialization parameters
while maintaining the other parameters constant. 
More specifically, try reducing the minimum pixel correlation and the peak-to-noise ratio.
Note that it is not recommended to specify an inaccurate average cell diameter in an attempt to capture the missing cells
as this could impair the quality of the output obtained from CNMFe.
Below is an example showing a field of view where some cells are visible but were not identified by CNMFe using the default
initialization parameters.

![Missing Cells](../img/missing_cells.png?raw=true "Missing Cells")

### Too Many Cells
If you feel that CNMFe identified too many cells, specifically blobs that don't appear to be cells, try
making the initialization parameters more stringent by increase the minimum pixel correlation and the peak-to-noise ratio.
Also, make sure to use the appropriate average cell diameter as underestimating or overestimating this value could lead to 
the identification of additional blobs that may not be cells of interest.

### Oversegmented Cells
If you find that CNMFe is overfragmenting cells, i.e. the same cell appears to be identified as multiple distinct components,
try reducing the merging threshold to allow CNMFe to combine such fragments together during processing.
Again, make sure to use an appropriate average cell diameter and underestimating it could in some cases lead to the identification
of smaller blobs within cells.
Below is an example showing cells that were oversegmented and identified as distinct cells.
In this case it is best to validate the average cell diameter and reduce the merging threshold
to prevent such oversegmentation of the cell bodies.

![Oversegmented Cells](../img/oversegmented_cells.png?raw=true "Oversegmented Cells")

### Slow Processing
Processing time is largely affected by the chosen processing mode and the parallelism associated
with the specified processing parameters.
Parallel patch mode is the fastest processing mode, but it requires more memory.
If you noticed that while running CNMFe you still have a large amount of unused memory,
try increasing the number of threads used with parallel patch mode.
Background estimation is one of the most expensive operations in CNMFe.
As such, increasing the background downsampling factor can significantly reduce processing time
while shifting the background estimate toward a global rather than localized estimate.

### Limited Computing Resources
If you are attempting to run CNMFe on a system that has a very small amount of random-access memory,
you may not be able to process you movie using either the all-in-memory or parallel patch mode.
However, the sequential patch mode will allow you to process a single patch of data at a time, dramatically
reducing memory consumption at the expense of longer processing time.

If your system runs out of memory during processing, CNMFe will likely shut down.
Most operating systems offer built-in monitoring tools to keep track of memory consumption.
To circumvent this issue, try reducing the patch size or number of threads if using parallel patch mode.
If this still consumes too much memory, try using sequential patch mode to minimize memory consumption.

## Output Units
The temporal traces extracted by Inscopix CNMFe can be expressed in terms of different units.
Inscopix CNMFe offers two options, dF and dF over noise as described in the table below.
It is common for people to standardize traces by subtracting the mean and dividing by the standard deviation of the traces,
effectively converting each value to a Z-score.
dF over noise uses a similar formula, with the exception that the traces are divided by the estimated noise level in each trace.
Since noise is estimated by averaging the high-frequency fluctuations along the trace, these output units are more robust to outliers.
In particular, outliers would inflate the standard deviation measured and thus reduce the scores computed.
The approach used to measure noise here is less susceptible to the influence of outliers.

| Output Units  | Description |
|:----------|:-------------|
| dF | temporal traces on the same scale of pixel intensity as the original movie. dF is calculated as the average fluorescence activity of all pixels in a cell, scaled so that each spatial footprint has a magnitude of 1. |
| dF over noise | temporal traces divided by their respective estimated noise level. This can be interpreted similarly to a z-score, with the added benefit that the noise is a more robust measure of the variance in a temporal trace compared to the standard deviation. |
