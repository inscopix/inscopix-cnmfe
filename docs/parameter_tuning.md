# Inscopix CNMFe Parameters
# Tips for Fine-Tuning CNMFe Parameters
This section provides guidance and tips on how to fine-tune the CNMFe input parameters
based on the results obtained. While the default parameters used in CNMFe were determined by 
applying them to several datasets, they may not be optimal for all datasets. 
As such, it is important to understand which parameter to tweak to obtain the desired output.

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

## Overview of Knobs (Module-Parameter Mapping)
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
### when to use each mode


## Common Issues

## Missing Cells

## Too Many Cells or Oversegmented Cells


## Slow Processing


## Limited Hardware Resources such as Insufficient Memory



## Output Units


## Auto-Estimated Parameters