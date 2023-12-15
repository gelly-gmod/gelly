# RTFR

RTFR refers to [Real Time Fluid Rendering](https://github.com/ttnghia/RealTimeFluidRendering) by Nghia Truong. It has a
very nice, simple to parse binary
file format for storing fluid data. This module provides a fluid simulation which loads and reads RTFR files and
provides a IFluidSimulation implementation which
outputs the data in the RTFR format. It is great for testing synthetic fluid data which has realistic shapes and
deformations.