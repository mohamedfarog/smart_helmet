# Firmware Release Notes
---
## Version: SHIARS Band App_V1.0.1
**Release Date:** 30/12/2024
**Author:** Midhulaj Pulickal Jalaludheen
#### Changes
- Added basic skeleton for the application.
- Added new board file shiars band compatible with `nrf5340` and with required overlays for the current project.
- The connection digram of the shiars band board is as shown below: ![alt text](../documents/images/pin_diagram.png)
- Added storage folder with basic skeleton for thread-safe get/set methods.
- Created basic event registers using ZBus.
- Command for shiars band build
```sh
   west build --build-dir /home/navcon/nrf_zephyr_ws/shiars_band_app/application/build_shiarsband /home/navcon/nrf_zephyr_ws/shiars_band_app/application --pristine --board shiarsband/nrf5340/cpuapp -- -DNCS_TOOLCHAIN_VERSION=NONE -DBOARD_ROOT=/home/navcon/nrf_zephyr_ws/shiars_band_app;/home/navcon/nrf_zephyr_ws/shiars_band_app/application
```
---
**End of Release Notes**  
 
