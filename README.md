# shadow-rendering
Master's project implementing and testing chosen shadow rendering techniques.

![Exemplary view of the program, with UI visible.]()

The final thesis is available as a PDF under [/Master Thesis/main.pdf](Master%20Thesis/main.pdf). It contains theoretical descriptions of the techniques in the contexts of their introductory publications, details on implementation and tests with results and conclusions.

## Implemented techniques
This application and thesis focuses on cast shadow rendering methods. Six techniqes were implemented and five of them have been tested and compared reagrding performance and visual results.

Implemented techniques:
- **Planar shadows**: not included in tests, implemented as an exercise to build geometric understanding of shadows);
- **Basic shadow mapping**: implemented for directional light sources with partial fitting;
- **PCF - Percentage-closer Filtering**: implemented multiple versions using hardware bilinear filtering, variable size kernel with a box filter and multiple sets of sample offsets stored in a texture. The last method is also called **Adaptive PCF**;
- **VSM - Variance Shadow Maps**: VSM enabled filtering the shadows with a separable Gaussian blur;
- **PCSS - Percenatge-closer soft shadows**: PCSS was implemented using Adaptive PCF for sampling the shadow maps. It enables rendering of contact-hardening shadows.

### Examples

![Chinese Dragon model casting a shadow rendered and filtered with Adaptive PCF](Master%20Thesis/graf/tests/adaptive/cropped/dragon_adaptive_fhd_1024_24x24_8x8_offset5.png)
*Chinese Dragon model casting a shadow rendered and filtered using Adaptive PCF.*

---
![PCSS shadow showcasing contact-hardening](Master%20Thesis/graf/tests/pcss/cropped/cube_pcss_1.png)
*PCSS shadow showcasing contact-hardening.*

---
![Crytek Sponza scene rendered with VSM](Master%20Thesis/graf/tests/vsm/cropped/sponza_vsm_4096_23.png)
*Crytek Sponza scene rendered with VSM.*
